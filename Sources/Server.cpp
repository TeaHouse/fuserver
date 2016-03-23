#include "FUNetServer/FUNetServer.h"
#include "Utils/UtilsTime.h"
#include "Utils/UtilsConfig.h"
#include "FUNetServer/FUProtocol.h"
#include "DBSqlServer/DBSqlManager.h"

static void InputCharAndBreakLoop(const char* pInputStr)
{
	int toPressNum = 0;
	char toPress;
	while ((toPress = getchar()))
	{
		if (pInputStr[toPressNum] == toPress)
		{
			toPressNum++;
			if (toPressNum == strlen(pInputStr)) break;
		}
		else
		{
			toPressNum = 0;
		}
	}
}

static void GameStart()
{
	UtilsLog::Init();
	//UtilsLog::SetWriteFile(true); 
	FUProtocol::Init();
	DBSqlManager::getSingletonPtr()->Init();
	FUNetServer* pServer = new FUNetServer();
	pServer->Start();

	InputCharAndBreakLoop("quit");
	
}

static void GameEnd()
{
	FUProtocol::Destroy();
	g_pNetServer->Destroy();
	FU_SAFE_DELETE(g_pNetServer);
	DBSqlManager::getSingletonPtr()->destroySelf();
	UtilsLog::Exit();
}

int main()
{
	GameStart();
	GameEnd();


	/*
	DBDataSet dataSet("person");
	dataSet.New(100);
	dataSet.printAll();

	dataSet["age"] = 300;
	dataSet.New(300);
	dataSet.Commit();

	DBDataSet dataSet1("person");

	char priKey[255] = {};
	DBProcData hParamData;

	hParamData.PushParam(priKey, sizeof(priKey), false);
	hParamData.PushParam("person", 6);
	dataSet1.NewProcedure("Proc_GET_PRIMARY_KEY", &hParamData);
	dataSet1.printAll();

	//DBSqlManager::getSingletonPtr()->ModifyCommit()
	*/

	//Json::Value protValue;
	//FUProtocol::CreateProt(1001, protValue);

	//FUPrint(protValue.toStyledString());



	return 0;
}