#ifndef __FU_GAMELOGIC_LOGIN_H__
#define __FU_GAMELOGIC_LOGIN_H__

#include "../GameBaseLogic.h"

class FULogin : public GameBaseLogic
{
public:
	FULogin();
	~FULogin();

	virtual bool CheckProtocol(int sockid, int protId, const Json::Value& jsonProtValue);
};






#endif