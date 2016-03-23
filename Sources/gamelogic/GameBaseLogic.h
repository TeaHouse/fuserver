#ifndef __FU_GAMELOGIC_GAMEBASELOGIC_H__
#define __FU_GAMELOGIC_GAMEBASELOGIC_H__

#include "../FUNetServer/FUProtocol.h"
#include "../FUNetServer/FUNetServer.h"

class GameBaseLogic
{
public:
	virtual ~GameBaseLogic()	{}
	virtual bool CheckProtocol(int sockid, int protId, const Json::Value& jsonProtValue) = 0; //����true��ʾ�Ѿ������������

	static void RegisterNetServer(FUNetServer* pServer);
};







#endif