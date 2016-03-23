#include "FULogin.h"

FULogin::FULogin()
{

}

FULogin::~FULogin()
{

}

bool FULogin::CheckProtocol(int sockid, int protId, const Json::Value& jsonProtValue)
{
	switch (protId)
	{
		case 1001:
		{
			const Json::Value& hMsgData = jsonProtValue["msgData"];
			//printf("--------")
			std::string loginName = hMsgData["loginName"].asString();
			std::string loginPwd = hMsgData["loginPwd"].asString();
				
			FULOG("----CheckProtocol--[%s]--[%s]---", loginName.c_str(), loginPwd.c_str());
			Json::Value hOutValue;
			FUProtocol::CreateProt(1002, hOutValue);
			if (loginPwd == "111111")
			{
				hOutValue["msgData"]["loginIn"] = 1;
			}
			else
			{
				hOutValue["msgData"]["loginIn"] = 0;
			}						
			return FUProtocol::SendProt(sockid, hOutValue);
		}
		break;
	}
	return false;
}

