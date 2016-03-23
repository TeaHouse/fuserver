#ifndef __FU_SERVER_PROTOCOL_H__
#define __FU_SERVER_PROTOCOL_H__

#include "../Platform/Include.h"

namespace FUProtocol
{
	bool  Init();

	void  Destroy(); //���٣��������ر�ʱ������
	bool  AddFile(const std::string& fileName);  //����Э���ļ�����ʽ��MyProt.json��ͬ

	bool  CreateProt(int protId, Json::Value& outValue);	//�������͵�jsonЭ��ṹ��
	int   RecvProt(const char* pbuf, int buflen, Json::Value& outValue); //�����յ���Э��,�������ж��Ƿ����˸�Э��
	bool  SendProt(int socket, const Json::Value& inValue); //����ת���ɷ����ַ���, ������
	bool  SendError(int socket, int protId);
};




#endif