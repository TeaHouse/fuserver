#ifndef __FU_IOCP_NETWORK_WIN32_SERVER_H__
#define __FU_IOCP_NETWORK_WIN32_SERVER_H__

#include "NetServer_Platform.h"

class NetServer_Win32 : public NetServer_Platform
{
#ifdef WIN32
public:
	enum class OP_TYPE
	{
		OP_ACCEPT = 1,
		OP_RECV,
		OP_SEND,
	};
public:
	NetServer_Win32();
	~NetServer_Win32();
public:
	bool CreateServer(const char* serverIp, int serverPort) final;
	void DestroyServer() final;	//¹Ø±Õ·þÎñÆ÷

	virtual void CloseSocket(int sock_id) final;
	
	bool SendText(int sock, const char* buf, int buflen) final;
public:
	CLASS_GET_OR_SET_PRIVATE(m_hCompletionPort, HANDLE, CompletionPort)

#endif
};




#endif