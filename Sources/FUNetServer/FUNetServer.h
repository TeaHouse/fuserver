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
	//���ڱ��̳���,��д�����麯��
	//��Ҫ��NotifyEstablishLink�������������շ���Ϣ����Ϊ��Ҫ������Ͷ��recv����,Ҫ��recv����֮����շ���Ϣ
	virtual void NotifyEstablishLink(int sock, const char* ip);//�½�����
	virtual void NotifyRecvFinish(int sock, const char* buf, int buflen);//������֪ͨ
	virtual void NotifySendFinish(int sock);//������֪ͨ
	virtual void NotifyCloseLink(int sock, int code);	//�ر�����֪ͨ
};






#endif