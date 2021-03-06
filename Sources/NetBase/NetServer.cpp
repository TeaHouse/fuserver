#include "FUNetServer.h"
#include "../Utils/UtilsQueue.h"
#include "../NetServer/FUProtocol.h"
#include "../Utils/UtilsThread.h"
#include "../Utils/UtilsTime.h"

struct CRecvBuffer
{
	std::string		_buffer;
	int		_socket;
};
NetServer*	g_pNetServer = NULL;
static UtilsQueue	st_BufferQueue;
static std::unordered_map<int, std::string>  st_LinkSocketList;
static std::vector<GameBaseLogic*>		st_RegGameLogicList;
static UtilsThread  st_ThreadHandle;
static bool st_bStopThread = false;
static void  st_AddLinkSock(int socket, const std::string& ip)
{
	auto iter = st_LinkSocketList.find(socket);
	if (iter == st_LinkSocketList.end())
	{
		st_LinkSocketList.insert(std::make_pair(socket, ip));
	}
}
static void st_RidLinkSock(int socket)
{
	auto iter = st_LinkSocketList.find(socket);
	if (iter != st_LinkSocketList.end())
	{
		st_LinkSocketList.erase(iter);
	}
}
static void CheckProtocolBuffer(int sock, const char* buf, int buflen)
{
	Json::Reader hReader;
	Json::Value  hValue;
	int protId = FUProtocol::CheckProt(buf, buflen, hValue);
	if (protId < 0)
	{
		if (protId == -1)
		{
			g_pNetServer->SendText(sock, FUProtocol::GetErrcodeBuf(100));
		}
		else if (protId == -2)
		{
			g_pNetServer->SendText(sock, FUProtocol::GetErrcodeBuf(101));
		}
	}
	else
	{
		for (auto& pLogic : st_RegGameLogicList)
		{
			if (pLogic->CheckProtocol(sock, protId, hValue))
			{
				return;
			}
		}
		FULOG("protId=[%d] not define Excute GameLogic!", protId);
	}
}
static void* st_ExcuteProtQueueFunc(void* lpParam)
{
	while (!st_bStopThread)
	{
		if (!st_BufferQueue.Empty())
		{
			CRecvBuffer* pBufData = (CRecvBuffer*)st_BufferQueue.PopFront();
			CheckProtocolBuffer(pBufData->_socket, pBufData->_buffer.c_str(), pBufData->_buffer.length());
			FU_SAFE_DELETE(pBufData);
		}
		UtilsTime::SleepUsec(0);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
NetServer::NetServer()
{
	st_bStopThread = true;
}

NetServer::~NetServer()
{
	st_bStopThread = true;
	st_ThreadHandle.Destroy(true);
	for (auto& pLogic : st_RegGameLogicList)
	{
		FU_SAFE_DELETE(pLogic);
	}
	st_RegGameLogicList.clear();
	st_LinkSocketList.clear();
	st_BufferQueue.Clear();
}

bool NetServer::Start()
{
	int datalen = 0;
	char* pFileData = fu_getfiledata("globalDef.json", datalen);
	Json::Reader  hReader;
	Json::Value	  hRoot;

	std::string document;
	document.assign(pFileData, datalen);
	delete pFileData;
	if (!hReader.parse(document, hRoot))
	{
		FUError("DBSqlManager::Init globalDef.json Error!");
		return false;
	}

	const char* IPConfig = "serverIp";
	const char* PortConfig = "serverPort";
	std::string srvIp = Json_FindKeyValue(hRoot, IPConfig)->asString();
	std::string srvPort = Json_FindKeyValue(hRoot, PortConfig)->asString();

	g_pNetServer = this;
	st_bStopThread = false;
	st_ThreadHandle.Create(st_ExcuteProtQueueFunc, (char*)this);
	return this->CreateServer(srvIp.c_str(), atoi(srvPort.c_str()));
}

void NetServer::Destroy()
{
	this->DestroyServer();
}

void NetServer::RegisterGameLogic(GameBaseLogic* pLogic)
{
	st_RegGameLogicList.push_back(pLogic);
}

const std::unordered_map<int, std::string>& NetServer::GetLinkSockList()
{
	return st_LinkSocketList;
}

//////////////////////////////////////////////////////////////////////////
void NetServer::NotifyEstablishLink(int sock, const char* ip)
{
#ifndef Define_Publish
	FULOG("--NotifyEstablishLink---[%d]---remoteAddr=[%s]--", sock, ip);
#endif
	st_AddLinkSock(sock, ip);
}

void NetServer::NotifyRecvFinish(int sock, const char* buf, int buflen)
{
	if (buflen>0)
	{
// 		CRecvBuffer* pData = new CRecvBuffer();
// 		pData->_buffer.assign(buf, buflen);
// 		pData->_socket = sock;
// 		st_BufferQueue.PushBack((char*)pData);

		CheckProtocolBuffer(sock, buf, buflen);
		//FU_SAFE_DELETE(pData);
	}
}

void NetServer::NotifySendFinish(int sock)
{
#ifndef Define_Publish
	FULOG("--NotifySendFinish---[%d]--\n", sock);
#endif
}

void NetServer::NotifyCloseLink(int sock, int code)
{
#ifndef Define_Publish
	FULOG("--NotifyCloseLink---[%d]--\n", sock);
#endif
	st_RidLinkSock(sock);
}
