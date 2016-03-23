#ifndef __FU_GAMELOGIC_GAMEBASELOGIC_H__
#define __FU_GAMELOGIC_GAMEBASELOGIC_H__

#include "../FUNetServer/FUProtocol.h"
#include "../FUNetServer/FUNetServer.h"

class GameBaseLogic
{
public:
	virtual ~GameBaseLogic()	{}
	virtual bool CheckProtocol(int sockid, int protId, const Json::Value& jsonProtValue) = 0; //返回true表示已经处理，否则过滤

	static void RegisterNetServer(FUNetServer* pServer);
};







#endif