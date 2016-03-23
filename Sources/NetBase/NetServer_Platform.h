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
	//��Ҫ��NotifyEstablishLink�������������շ���Ϣ����Ϊ��Ҫ������Ͷ��recv����,Ҫ��recv����֮����շ���Ϣ
	virtual void NotifyEstablishLink(int sock, const char* ip) = 0;//�½�����
	virtual void NotifyRecvFinish(int sock, const char* buf, int buflen) = 0;//������֪ͨ
	virtual void NotifySendFinish(int sock) = 0;//������֪ͨ
	virtual void NotifyCloseLink(int sock, int code) = 0;	//�ر�����֪ͨ
protected:
	virtual bool CreateServer(const char* serverIp, int serverPort) = 0;
	virtual void DestroyServer() = 0;	//�رշ�����
public:
	CLASS_GET_OR_SET_PROTECTED(m_ListenSocket, int, ListenSocket)

	CLASS_GET_OR_SET_PROTECTED(m_bShutDown, bool, ShutDown)
	CLASS_GET_OR_SET_PROTECTED(m_bStart, bool, IsStart)

	//�Ƿ�ʹ�õײ����������ʹ�ã���Է��Ͽ�ʱ������NotifyCloseLink
	CLASS_GET_OR_SET_PROTECTED(m_bHeartBeat, bool, bHeartBeat)
	CLASS_GET_OR_SET_PROTECTED(m_sIp, std::string, Ip)
	CLASS_GET_OR_SET_PROTECTED(m_iPort, int, Port)
};



#endif