#ifndef __FU_IOCP_NETWORK_PLATFORM_SERVER_H__
#define __FU_IOCP_NETWORK_PLATFORM_SERVER_H__

#include "../Utils/UtilsFunction.h"
#include "NetBase.h"

class NetServer_Platform
{
public:
	NetServer_Platform();
	virtual ~NetServer_Platform();
	
	virtual bool SendText(int sock, const char* buf, int buflen) = 0;
	virtual bool SendText(int sock, const std::string& sendBuf) = 0;
	virtual void CloseSocket(int sock_id) = 0;
public:
	//不要在NotifyEstablishLink创建连接这里收发消息，因为需要创建新投递recv请求,要等recv到来之后才收发消息
	virtual void NotifyEstablishLink(int sock, const char* ip) = 0;//新进连接
	virtual void NotifyRecvFinish(int sock, const char* buf, int buflen) = 0;//接受完通知
	virtual void NotifySendFinish(int sock) = 0;//发送完通知
	virtual void NotifyCloseLink(int sock, int code) = 0;	//关闭连接通知
protected:
	virtual bool CreateServer(const char* serverIp, int serverPort) = 0;
	virtual void DestroyServer() = 0;	//关闭服务器
public:
	CLASS_GET_OR_SET_PROTECTED(m_ListenSocket, int, ListenSocket)

	CLASS_GET_OR_SET_PROTECTED(m_bShutDown, bool, ShutDown)
	CLASS_GET_OR_SET_PROTECTED(m_bStart, bool, IsStart)

	//是否使用底层的心跳，若使用，则对方断开时候会调用NotifyCloseLink
	CLASS_GET_OR_SET_PROTECTED(m_bHeartBeat, bool, bHeartBeat)
	CLASS_GET_OR_SET_PROTECTED(m_sIp, std::string, Ip)
	CLASS_GET_OR_SET_PROTECTED(m_iPort, int, Port)
};



#endif