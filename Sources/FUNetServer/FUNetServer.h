#ifndef __FU_NETWORK_SERVER_H__
#define __FU_NETWORK_SERVER_H__

#include "../NetBase/NetServer_Libevent.h"

class GameBaseLogic;
class FUNetServer;
extern FUNetServer*	g_pNetServer;

class FUNetServer : public NetServer_Libevent
{
public:
	FUNetServer();
	~FUNetServer();
	bool Start();
	void Destroy();

	void RegisterGameLogic(GameBaseLogic* pLogic);
	const std::unordered_map<int, std::string>&  GetLinkSockList();
public:
	//用于被继承类,重写以下虚函数
	//不要在NotifyEstablishLink创建连接这里收发消息，因为需要创建新投递recv请求,要等recv到来之后才收发消息
	virtual void NotifyEstablishLink(int sock, const char* ip);//新进连接
	virtual void NotifyRecvFinish(int sock, const char* buf, int buflen);//接受完通知
	virtual void NotifySendFinish(int sock);//发送完通知
	virtual void NotifyCloseLink(int sock, int code);	//关闭连接通知
};






#endif