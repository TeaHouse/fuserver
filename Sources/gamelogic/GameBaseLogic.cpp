#include "GameBaseLogic.h"
#include "login/FULogin.h"

void GameBaseLogic::RegisterNetServer(FUNetServer* pServer)
{
	pServer->RegisterGameLogic(new FULogin());
};

