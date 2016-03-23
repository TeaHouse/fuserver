#ifndef __FU_DATABASE_SERVER_H__
#define __FU_DATABASE_SERVER_H__

#include "../NetBase/NetServer.h"

class DBServer : public NetServer
{
public:
	DBServer();
	~DBServer();

	virtual void NotifyEstablishLink(int sock, const SOCKADDR_IN* localAddr, const SOCKADDR_IN* remoteAddr);//新进连接
	virtual void NotifyRecvFinish(int sock, const char* buf, int buflen);//接受完通知
	virtual void NotifySendFinish(int sock, const char* buf, int buflen);//发送完通知
	virtual void NotifyCloseLink(int sock, int code);	//关闭连接通知
};

#endif