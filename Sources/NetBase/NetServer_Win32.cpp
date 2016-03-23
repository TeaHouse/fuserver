
#include "NetServer_Win32.h"
#ifdef WIN32

#include "../Utils/UtilsMemPool.h"

#define  NET_FIRST_DELIVER_ACCEPTMSG	10	//首次投递ACCEPT的数量
#define  NET_ONECE_DELIVER_ACCEPTMSG	50	//若预先投递的操作消息不足，则一次增加投递的数量
#define  NET_DELIVER_RECVMSG_NUM		1	//表示多少个recv请求来处理一个socket的send请求，可能不同的send会异步到达
#define  NET_HEARTBEAT_TIME				3000	// 开始首次KeepAlive探测前的TCP空闲时间，即空闲多少毫秒就开始探测
#define  NET_HEARTBEAT_INTERVAL			2000 //每次心跳检测间隔时间

typedef struct
{
	WSAOVERLAPPED		m_ol;
	SOCKET				m_soket;
	NetServer_Win32::OP_TYPE	m_optype;	//操作类型
	int		m_iBufferLen;
	char	m_pRecvBuf[NET_BUFFER_MAXSIZE];
	char*	m_pSendBuf;
	CRITICAL_SECTION	m_lock;	//锁recv
}NET_OPERATION_MSG;	//操作结构体
typedef struct
{
	SOCKET	m_sock;
}NET_HANDLE_DATA;	//句柄数据

void FreeOperationMsgFunc(NET_OPERATION_MSG* pData)
{
	if (pData->m_soket != INVALID_SOCKET)
	{
		::closesocket(pData->m_soket);
		pData->m_soket = INVALID_SOCKET;
	}
}
//////////////////////////////////////////////////////////////////////////
HANDLE		g_hWaitThreadHandle;
HANDLE		g_hAcceptEvent;
HANDLE		g_hPostEvent;
HANDLE		g_EventHandleArray[20];
HANDLE		g_ThreadHandleArray[20];
int			g_iThreadSize = 0;
int			g_iEventSize = 0;
long		g_iRepostCount = 0;	//请求投递的数量
LPFN_ACCEPTEX					g_lpAcceptExFunc;
LPFN_GETACCEPTEXSOCKADDRS		g_lpGetAcceptExSocketAddrsFunc;
UtilsMemPool<NET_OPERATION_MSG, sizeof(NET_OPERATION_MSG)>  g_OperationMsgPool;
//UtilsMemPool<NET_HANDLE_DATA, sizeof(NET_HANDLE_DATA)>  g_HandleDataPool;
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI _WorkerThreadProc(LPVOID lpParam);
DWORD WINAPI  _WaitEeventThreadProc(LPVOID lpParam);

bool DeliverAccept(NET_OPERATION_MSG* pOpMsg, SOCKET listenSocket);
bool DeliverRecv(SOCKET toSocket, NET_OPERATION_MSG* pOpMsg);
void DisposeIOMsg(NetServer_Win32* pNetThis, NET_OPERATION_MSG* pOpMsg, DWORD dwTrans, int nError);
//////////////////////////////////////////////////////////////////////////
NetServer_Win32::NetServer_Win32()
{
	WORD wVersionRequested = MAKEWORD(2, 2); // 请求2.2版本的WinSock库  
	WSADATA wsaData;    // 接收Windows Socket的结构信息  
	DWORD err = WSAStartup(wVersionRequested, &wsaData);

	if (0 != err){  // 检查套接字库是否申请成功  
		::ExitProcess(::GetLastError());
	}
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){// 检查是否申请了所需版本的套接字库  
		WSACleanup();
		::ExitProcess(::GetLastError());
	}
}

NetServer_Win32::~NetServer_Win32()
{
	if (g_hWaitThreadHandle != NULL)
	{
		::CloseHandle(g_hWaitThreadHandle);
		g_hWaitThreadHandle = NULL;
	}
}

bool NetServer_Win32::CreateServer(const char* serverIp, int serverPort)
{
	//m_ListenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	m_ListenSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN si;
	si.sin_family = AF_INET;
	si.sin_port = ::htons(serverPort);
	si.sin_addr.S_un.S_addr = inet_addr(serverIp);

	BOOL bReuseaddr = TRUE;
	::setsockopt(m_ListenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&bReuseaddr, sizeof(BOOL));

	if (::bind(m_ListenSocket, (sockaddr*)&si, sizeof(si)) == SOCKET_ERROR)
	{
		::closesocket(m_ListenSocket);
		FUError("bind Error!\n");
		return false;
	}
	if (::listen(m_ListenSocket, NET_LISTEN_MAXSIZE) == SOCKET_ERROR)
	{
		::closesocket(m_ListenSocket);
		FUError("listen Error!\n");
		return false;
	}


	// 加载扩展函数AcceptEx
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes;
	int ioctlCode = ::WSAIoctl(m_ListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx,
		sizeof(GuidAcceptEx),
		&g_lpAcceptExFunc,
		sizeof(g_lpAcceptExFunc),
		&dwBytes,
		NULL,
		NULL);
	if (ioctlCode != 0)
	{
		FUError(fu_string_format("WSAIoctl ACCEPTEX Error[%d]", ::WSAGetLastError()).c_str());
	}
	// 加载扩展函数GetAcceptExSockaddrs
	GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	ioctlCode = ::WSAIoctl(m_ListenSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExSockaddrs,
		sizeof(GuidGetAcceptExSockaddrs),
		&g_lpGetAcceptExSocketAddrsFunc,
		sizeof(g_lpGetAcceptExSocketAddrsFunc),
		&dwBytes,
		NULL,
		NULL
		);

	if (ioctlCode != 0)
	{
		FUError(fu_string_format("WSAIoctl GETACCEPTEXSOCKADDRS Error[%d]", ::WSAGetLastError()).c_str());
	}
	m_hCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	// 将监听套节字关联到完成端口，注意，这里为它传递的CompletionKey为0
	::CreateIoCompletionPort((HANDLE)m_ListenSocket, m_hCompletionPort, (DWORD)0, 0);

	NET_OPERATION_MSG* pOpMsg = NULL;
	for (int i = 0; i < NET_FIRST_DELIVER_ACCEPTMSG; i++)
	{
		pOpMsg = (NET_OPERATION_MSG*)g_OperationMsgPool.Create();
		if (!DeliverAccept(pOpMsg, m_ListenSocket))
		{
			FUError("g_lpAcceptExFunc Error!\n");
			return false;
		}
	}

	g_hAcceptEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	g_hPostEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	g_hWaitThreadHandle = ::CreateThread(NULL, 0, _WaitEeventThreadProc, (LPVOID)this, 0, NULL);

	SetIsStart(true);
	SetIp(serverIp);
	SetPort(serverPort);
	std::string logStr = fu_string_format("服务器创建成功: ip=[%s], port=[%d]\n", serverIp, serverPort);
	FULOG(logStr.c_str());

	return true;
}

void NetServer_Win32::DestroyServer()
{
	if (!m_bStart)
	{
		return;
	}
	// 通知监听线程，马上停止服务
	SetShutDown(true);
	::SetEvent(g_hAcceptEvent);
	// 等待监听线程退出
	::WaitForSingleObject(g_hWaitThreadHandle, INFINITE);
	::CloseHandle(g_hWaitThreadHandle);
	g_hWaitThreadHandle = NULL;

	SetIsStart(false);
}

void NetServer_Win32::CloseSocket(int sock_id)
{
	::CloseHandle((HANDLE)sock_id);
}

//////////////////////////////////////////////////////////////////////////

DWORD WINAPI  _WaitEeventThreadProc(LPVOID lpParam)
{
	NetServer_Win32* pNetThis = (NetServer_Win32*)lpParam;
	SOCKET  listenSocket = pNetThis->GetListenSocket();
	g_EventHandleArray[g_iEventSize++] = g_hAcceptEvent;
	g_EventHandleArray[g_iEventSize++] = g_hPostEvent;

	WSAEventSelect(listenSocket, g_EventHandleArray[0], FD_ACCEPT);

	SYSTEM_INFO mySysInfo;
	GetSystemInfo(&mySysInfo);

	int nEventCount = mySysInfo.dwNumberOfProcessors * 2;
	for (int i = 0; i < nEventCount; i++)
	{
		HANDLE  hThread = ::CreateThread(NULL, 0, _WorkerThreadProc, lpParam, 0, NULL);
		g_EventHandleArray[g_iEventSize++] = hThread;
		g_ThreadHandleArray[g_iThreadSize++] = hThread;
	}
	int nWaitRet;
	NET_OPERATION_MSG* pOpMsg = NULL;
	while (true)
	{
		nWaitRet = ::WSAWaitForMultipleEvents(g_iEventSize, g_EventHandleArray, FALSE, WSA_INFINITE, FALSE);

		if (nWaitRet == WSA_WAIT_FAILED || pNetThis->GetShutDown())
		{
			FUError("WSAWaitForMultipleEvents WSA_WAIT_FAILED or ShutDown!\n");
			break;
		}
		else if (nWaitRet == WSA_WAIT_TIMEOUT)
		{
			FUError("WSAWaitForMultipleEvents WSA_WAIT_TIMEOUT!\n");
			continue;
		}
		else
		{
			nWaitRet = nWaitRet - WAIT_OBJECT_0;
			int nDeliverNum = 0;
			if (nWaitRet == 0)
			{
				WSANETWORKEVENTS  wsaWorkEvent;
				::WSAEnumNetworkEvents(listenSocket, g_hAcceptEvent, &wsaWorkEvent);
				if (wsaWorkEvent.lNetworkEvents & FD_ACCEPT)
				{
					nDeliverNum = NET_ONECE_DELIVER_ACCEPTMSG;  // 增加的个数，这里设为50个
				}
			}
			else if (nWaitRet == 1)	// 处理I/O的线程接受到新的客户
			{
				nDeliverNum = InterlockedExchange(&g_iRepostCount, 0);
			}
			else if (nWaitRet > 1)
			{
				pNetThis->SetShutDown(true);
				continue;
			}
			for (int i = 0; i < nDeliverNum; i++)
			{
				pOpMsg = g_OperationMsgPool.Create();
				if (!DeliverAccept(pOpMsg, listenSocket))
				{
					FUError("WaitEeventThreadProc DeliverAccept Error!\n");
					pNetThis->SetShutDown(true);
					continue;
				}
			}
		}

	}

	return 0;
}

DWORD WINAPI _WorkerThreadProc(LPVOID lpParam)
{
	NetServer_Win32* pNetThis = (NetServer_Win32*)lpParam;
	HANDLE  toCompletionPort = pNetThis->GetCompletionPort();
	SOCKET  listenSocket = pNetThis->GetListenSocket();

	BOOL	nBoolRet;
	DWORD	nTransferBytes;
	ULONG	toCompletionKey;
	LPOVERLAPPED  toOverlapped;
	NET_OPERATION_MSG*	pOperationMsg;
	//NET_HANDLE_DATA*	pHandleData;
	int		nError;
	DWORD	dwFlags = 0;
	SOCKET	toSocket;
	while (true)
	{
		nBoolRet = ::GetQueuedCompletionStatus(toCompletionPort, &nTransferBytes,
			&toCompletionKey, &toOverlapped, WSA_INFINITE);
		if (nTransferBytes == -1)
		{
			FUError("GetQueuedCompletionStatus Error!");
			
			::closesocket(listenSocket);
			pNetThis->SetListenSocket(INVALID_SOCKET);
			UtilsTime::SleepUsec(0);		// 给I/O工作线程一个执行的机会

			// 通知所有I/O处理线程退出
			for (int i = 2; i < g_iThreadSize; i++)
			{
				::PostQueuedCompletionStatus(toCompletionPort, -1, 0, NULL);
			}
			// 等待I/O处理线程退出
			::WaitForMultipleObjects(g_iThreadSize, g_ThreadHandleArray, TRUE, 3 * 1000);
			for (int i = 2; i < g_iThreadSize; i++)
			{
				::CloseHandle(g_ThreadHandleArray[i]);
			}
			::CloseHandle(toCompletionPort);

			break;//被通知退出
		}
		pOperationMsg = CONTAINING_RECORD(toOverlapped, NET_OPERATION_MSG, m_ol);

		nError = NO_ERROR;
		if (!nBoolRet)
		{
			toSocket = INVALID_SOCKET;
			if (pOperationMsg->m_optype == NetServer_Win32::OP_TYPE::OP_ACCEPT)
			{
				toSocket = listenSocket;
			}
			else 
			{
				toSocket = pOperationMsg->m_soket;
			}
			if (toSocket != INVALID_SOCKET && !::WSAGetOverlappedResult(toSocket, &pOperationMsg->m_ol, &nTransferBytes, FALSE, &dwFlags))
			{
				nError = ::WSAGetLastError();
			}
		}
		DisposeIOMsg(pNetThis, pOperationMsg, nTransferBytes, nError);
	}
	return 0;
}

bool NetServer_Win32::SendText(int sock, const char* buf, int buflen)
{
	if (!buf || buflen<=0)
	{
		return false;
	}
	NET_OPERATION_MSG* pOpMsg = g_OperationMsgPool.Create();

	pOpMsg->m_iBufferLen = 0;
#ifdef ENCRYPT_PROT_BUFFER
	pOpMsg->m_pSendBuf = fu_encrypt_stream(buf, pOpMsg->m_iBufferLen, NET_BUFFER_MAXSIZE);
#else
	pOpMsg->m_pSendBuf = new char[buflen + 1];
	memcpy(pOpMsg->m_pSendBuf, buf, buflen);
	pOpMsg->m_iBufferLen = buflen;
#endif
	

	pOpMsg->m_optype = OP_TYPE::OP_SEND;
	pOpMsg->m_soket = sock;
	::InitializeCriticalSection(&(pOpMsg->m_lock));

	::ZeroMemory(&(pOpMsg->m_ol), sizeof(WSAOVERLAPPED));
	::ZeroMemory(pOpMsg->m_pRecvBuf, sizeof(pOpMsg->m_pRecvBuf));

	// 投递此重叠I/O
	DWORD dwBytes;
	DWORD dwFlags = 0;
	WSABUF wsaBuf;
	wsaBuf.buf = pOpMsg->m_pSendBuf;
	wsaBuf.len = pOpMsg->m_iBufferLen;
	if (::WSASend(pOpMsg->m_soket,
		&wsaBuf, 1, &dwBytes, dwFlags, &pOpMsg->m_ol, NULL) != NO_ERROR)
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
			return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool DeliverAccept(NET_OPERATION_MSG* pOpMsg, SOCKET listenSocket)
{
	int  nAddrSize = sizeof(sockaddr_in)+16;
	pOpMsg->m_iBufferLen = NET_BUFFER_MAXSIZE;
	pOpMsg->m_optype = NetServer_Win32::OP_TYPE::OP_ACCEPT;
	pOpMsg->m_soket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	::ZeroMemory(&pOpMsg->m_ol, sizeof(WSAOVERLAPPED));
	DWORD dwBytes;
	BOOL nBRet = g_lpAcceptExFunc(listenSocket,
		pOpMsg->m_soket,
		pOpMsg->m_pRecvBuf,
		pOpMsg->m_iBufferLen - (nAddrSize * 2), //这一大小应不包括服务器的本地地址的大小或客户端的远程地址,他们被追加到输出缓冲区
		nAddrSize,
		nAddrSize,
		&dwBytes,
		&pOpMsg->m_ol);
	if (!nBRet && ::WSAGetLastError() != WSA_IO_PENDING)
	{
		FUError(fu_string_format("DeliverAccept Error[%d]", ::WSAGetLastError()).c_str());
		return false;
	}
	return true;
}

bool DeliverRecv(SOCKET toSocket, NET_OPERATION_MSG* pOpMsg)
{
	//尽量不要分配，而是从内存池拿出
	
	NET_OPERATION_MSG* pMsg = pOpMsg;
	if (!pMsg)
	{
		pMsg = g_OperationMsgPool.Create();
		::InitializeCriticalSection(&(pMsg->m_lock));
	}

	::EnterCriticalSection(&(pMsg->m_lock));
	pMsg->m_soket = toSocket;
	pMsg->m_optype = NetServer_Win32::OP_TYPE::OP_RECV;
	::ZeroMemory(&(pMsg->m_ol), sizeof(WSAOVERLAPPED));
	::ZeroMemory(pMsg->m_pRecvBuf, sizeof(pMsg->m_pRecvBuf));
	pMsg->m_iBufferLen = NET_BUFFER_MAXSIZE;
	pMsg->m_pSendBuf = NULL;

	DWORD dwBytes;
	DWORD dwFlags = 0;
	WSABUF buf;
	buf.buf = pMsg->m_pRecvBuf;
	buf.len = pMsg->m_iBufferLen;

	if (::WSARecv(toSocket, &buf, 1, &dwBytes, &dwFlags, &pMsg->m_ol, NULL) != NO_ERROR)
	{
		if (::WSAGetLastError() != WSA_IO_PENDING)
		{
			//解锁
			::LeaveCriticalSection(&(pMsg->m_lock));
			return false;
		}
	}
	
	//解锁
	::LeaveCriticalSection(&(pMsg->m_lock));

	return true;
}
bool DisposeHeartbeat(SOCKET  toSock)
{
	//设置底层心跳包
	BOOL bKeepAlive = TRUE;
	int nRet = ::setsockopt(toSock, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(bKeepAlive));
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}
	else
	{
		// 设置KeepAlive参数, 心跳包
		tcp_keepalive alive_in = { 0 };
		tcp_keepalive alive_out = { 0 };
		alive_in.keepalivetime = NET_HEARTBEAT_TIME;	// 开始首次KeepAlive探测前的TCP空闲时间
		alive_in.keepaliveinterval = NET_HEARTBEAT_INTERVAL;	// 两次KeepAlive探测间的时间间隔
		alive_in.onoff = 1;
		unsigned long ulBytesReturn = 0;
		nRet = ::WSAIoctl(toSock, SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in),
			&alive_out, sizeof(alive_out), &ulBytesReturn, NULL, NULL);
		if (nRet == SOCKET_ERROR)
		{
			return false;
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
void DisposeIOMsg(NetServer_Win32* pNetThis, NET_OPERATION_MSG* pOpMsg, DWORD dwTrans, int nError)
{
	if (nError != NO_ERROR)
	{
		if (nError == WSA_INVALID_HANDLE) FUError("WSA_INVALID_HANDLE");
		else if (nError == WSANOTINITIALISED) FUError("WSANOTINITIALISED");
		else if (nError == WSAENETDOWN) FUError("WSAENETDOWN");
		else if (nError == WSAENOTSOCK) FUError("WSAENOTSOCK");
		else if (nError == WSA_INVALID_PARAMETER) FUError("WSA_INVALID_PARAMETER");
		else if (nError == WSA_IO_INCOMPLETE) FUError("WSA_IO_INCOMPLETE");
		else if (nError == WSAECONNRESET) FUError("WSAECONNRESET");
		pNetThis->NotifyCloseLink(pOpMsg->m_soket, nError);
 		g_OperationMsgPool.Free(pOpMsg, FreeOperationMsgFunc);
		return;
	}
	if (pOpMsg->m_optype == NetServer_Win32::OP_TYPE::OP_ACCEPT)
	{
		if (!pNetThis->GetbHeartBeat() || DisposeHeartbeat(pOpMsg->m_soket))
		{
			// 取得客户地址
			int nLocalLen, nRmoteLen;
			LPSOCKADDR pLocalAddr, pRemoteAddr;
			g_lpGetAcceptExSocketAddrsFunc(
				pOpMsg->m_pRecvBuf,
				pOpMsg->m_iBufferLen - ((sizeof(sockaddr_in) + 16) * 2), // [2010.5.16 bak Lostyears]和AcceptEx相应参数对应
				sizeof(sockaddr_in) + 16,
				sizeof(sockaddr_in) + 16,
				(SOCKADDR **)&pLocalAddr,
				&nLocalLen,
				(SOCKADDR **)&pRemoteAddr,
				&nRmoteLen);
			//NET_HANDLE_DATA* pSockData = g_HandleDataPool.Create();
			// 关联新连接到完成端口对象, 并允许2个线程访问
			::CreateIoCompletionPort((HANDLE)pOpMsg->m_soket, pNetThis->GetCompletionPort(), NULL, 2);

			//通知用户，新的连接建立了
			pNetThis->NotifyEstablishLink(pOpMsg->m_soket, inet_ntoa(((sockaddr_in*)pRemoteAddr)->sin_addr));

			//使得非阻塞
			u_long iMode = 1;
			::ioctlsocket(pOpMsg->m_soket, FIONBIO, &iMode);
			//并且投递recv请求,可以投递多个recv，预接受多个send
			//DeliverRecv(pOpMsg->m_soket);
			for (int i = 0; i < NET_DELIVER_RECVMSG_NUM; i++)
			{
				if (!DeliverRecv(pOpMsg->m_soket, NULL))
				{
					break;
				}
			}
		}
		else
		{
			//关闭连接
			FreeOperationMsgFunc(pOpMsg);
		}
		// Accept请求完成，释放I/O缓冲区
		g_OperationMsgPool.Free(pOpMsg);

		// 通知监听线程继续再投递一个Accept请求
		::InterlockedIncrement(&g_iRepostCount);
		::SetEvent(g_hPostEvent);
	}
	else if (pOpMsg->m_optype == NetServer_Win32::OP_TYPE::OP_RECV)
	{
		//有接收到wsarecv，则用recv从tcp缓冲中拷贝出来
		std::string recvStr;
		if (dwTrans>0)
		{
			recvStr.assign(pOpMsg->m_pRecvBuf, dwTrans);
		}
		pOpMsg->m_iBufferLen = fu_socket_recv(pOpMsg->m_soket, recvStr);
		if (pOpMsg->m_iBufferLen == 0)
		{
			return; //已经拷贝完毕
		}
		else if (pOpMsg->m_iBufferLen < 0)//拷贝失败，或者客户端退出
		{
			//对方关闭连接, 发生错误
			pNetThis->NotifyCloseLink(pOpMsg->m_soket, nError);
			g_OperationMsgPool.Free(pOpMsg, FreeOperationMsgFunc);
			return;
		}
		pNetThis->NotifyRecvFinish(pOpMsg->m_soket, recvStr.c_str(), recvStr.length());
		DeliverRecv(pOpMsg->m_soket, pOpMsg);	//只有投递了recv才会接收消息
	}
	else if (pOpMsg->m_optype == NetServer_Win32::OP_TYPE::OP_SEND)
	{
		if (dwTrans == 0)
		{
			//关闭连接
			pNetThis->NotifyCloseLink(pOpMsg->m_soket, nError);
			g_OperationMsgPool.Free(pOpMsg, FreeOperationMsgFunc);
		}
		else
		{
			pOpMsg->m_iBufferLen = dwTrans;
			pNetThis->NotifySendFinish(pOpMsg->m_soket);
			FU_SAFE_DELETE(pOpMsg->m_pSendBuf);
			DeliverRecv(pOpMsg->m_soket, pOpMsg);
		}
	}
}

#endif