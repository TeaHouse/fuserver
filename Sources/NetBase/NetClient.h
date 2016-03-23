#ifndef __FU_NETBASE_CLIENT_H__
#define __FU_NETBASE_CLIENT_H__

#include "../Utils/UtilsFunction.h"
#include "../Utils/UtilsThread.h"
#include "NetBase.h"

class NetClient
{
public:
	NetClient();
	~NetClient();

	bool Connect(const char* serverIp, unsigned short port);

	void SendText(const char* pData, int datalen);

	virtual void NotifyRecvText(const char* pData, int datalen) = 0;

	int  GetSockId()	{ return m_SockId; }
	CLASS_GET_OR_SET_PRIVATE(m_bShutDown, bool, ShutDown)
private:
	int		m_SockId;
	UtilsThread	m_pThread;
};






#endif