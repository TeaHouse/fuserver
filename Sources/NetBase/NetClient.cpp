#include "NetClient.h"

static void* st_WaitEeventThreadProc(void* lpParam)
{
	NetClient* pClientThis = (NetClient*)lpParam;
	int sockeId = pClientThis->GetSockId();
	std::string recvStr;
	while (!pClientThis->GetShutDown())
	{
		recvStr.clear();
		fu_socket_recv(sockeId, recvStr);
		if (!recvStr.empty())
		{
			pClientThis->NotifyRecvText(recvStr.c_str(), recvStr.length());
		}
	}
	return 0;
}

NetClient::NetClient()
: m_SockId(-1)
, m_bShutDown(true)
{

}

NetClient::~NetClient()
{

}

bool NetClient::Connect(const char* serverIp, unsigned short port)
{
	if (!m_bShutDown) return false;
#ifdef WIN32
	WORD verRequest = MAKEWORD(1, 1);
	WSADATA wasData;
	int error = WSAStartup(verRequest, &wasData);
	if (error)
	{
		return false;
	}
#endif
	m_SockId = ::socket(AF_INET, SOCK_STREAM, 0);
	sockaddr_in toAddr;
	toAddr.sin_family = AF_INET;
	toAddr.sin_addr.s_addr = inet_addr(serverIp);
	toAddr.sin_port = htons(port);

	int errcode = ::connect(m_SockId, (sockaddr*)&toAddr, sizeof(sockaddr));
	if (errcode==-1)
	{
		return false;
	}
	m_bShutDown = false;

	m_pThread.Destroy(false);
	m_pThread.Create(st_WaitEeventThreadProc, (char*)this);


	std::string logStr = fu_string_format("连接服务器成功: ip=[%s], port=[%d]\n", serverIp, port);
	FULOG(logStr.c_str());

	return true;
}

void NetClient::SendText(const char* pData, int datalen)
{
	if (m_SockId != -1 && pData && datalen>0)
	{
		int encryptLen = datalen;
		bool temp = false;
#ifdef ENCRYPT_PROT_BUFFER
		const char* pEncrytData = fu_encrypt_stream(pData, encryptLen, NET_BUFFER_MAXSIZE);
		temp = true;
#else
		const char* pEncrytData = pData;
#endif
		fu_socket_send(m_SockId, pEncrytData, encryptLen);
		if (temp)delete pEncrytData;
	}
}

