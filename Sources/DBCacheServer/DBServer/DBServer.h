#ifndef __FU_DATABASE_SERVER_H__
#define __FU_DATABASE_SERVER_H__

#include "../NetBase/NetServer.h"

class DBServer : public NetServer
{
public:
	DBServer();
	~DBServer();

	virtual void NotifyEstablishLink(int sock, const SOCKADDR_IN* localAddr, const SOCKADDR_IN* remoteAddr);//�½�����
	virtual void NotifyRecvFinish(int sock, const char* buf, int buflen);//������֪ͨ
	virtual void NotifySendFinish(int sock, const char* buf, int buflen);//������֪ͨ
	virtual void NotifyCloseLink(int sock, int code);	//�ر�����֪ͨ
};

#endif