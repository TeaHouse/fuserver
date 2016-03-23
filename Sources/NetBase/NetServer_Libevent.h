#ifndef __FU_IOCP_NETWORK_LIBEVENT_SERVER_H__
#define __FU_IOCP_NETWORK_LIBEVENT_SERVER_H__

#include "NetServer_Platform.h"


class NetServer_Libevent : public NetServer_Platform
{
public:
	NetServer_Libevent();
	~NetServer_Libevent();
	virtual bool CreateServer(const char* serverIp, int serverPort) final;
	virtual void DestroyServer() final;	//¹Ø±Õ·þÎñÆ÷

	virtual bool SendText(int sock, const char* buf, int buflen) final;
	virtual bool SendText(int sock, const std::string& sendBuf) final;
	virtual void CloseSocket(int sock_id);


	CLASS_GET_OR_SET_PROTECTED(m_iEventBase, int, EventBase)

};


#endif