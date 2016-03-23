#ifndef __FU_IOCP_NETWORK_LINUX_SERVER_H__
#define __FU_IOCP_NETWORK_LINUX_SERVER_H__

#include "NetServer_Platform.h"

class NetServer_Linux : public NetServer_Platform
{
#ifndef WIN32
public:
	NetServer_Linux();
	~NetServer_Linux();

	bool CreateServer(const char* serverIp, int serverPort) final;
	void DestroyServer() final;	//¹Ø±Õ·þÎñÆ÷
	bool SendText(int sock, const char* buf, int buflen) final;

	void CloseSocket(int sock_id) final;
#endif
};






#endif