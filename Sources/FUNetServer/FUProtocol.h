#ifndef __FU_SERVER_PROTOCOL_H__
#define __FU_SERVER_PROTOCOL_H__

#include "../Platform/Include.h"

namespace FUProtocol
{
	bool  Init();

	void  Destroy(); //销毁，服务器关闭时候销毁
	bool  AddFile(const std::string& fileName);  //增加协议文件，格式和MyProt.json相同

	bool  CreateProt(int protId, Json::Value& outValue);	//创建发送的json协议结构体
	int   RecvProt(const char* pbuf, int buflen, Json::Value& outValue); //检测接收到的协议,解析并判断是否定义了该协议
	bool  SendProt(int socket, const Json::Value& inValue); //加密转换成发送字符串, 并发送
	bool  SendError(int socket, int protId);
};




#endif