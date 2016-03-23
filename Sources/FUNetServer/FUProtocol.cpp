#include "FUProtocol.h"
#include "../Utils/UtilsFile.h"
#include "FUNetServer.h"

using namespace FUProtocol;
std::vector<Json::Value*>   g_JsonProtList;

Json::Value const* st_IsProtIdInList(const std::string& protId)
{
	for (auto iter : g_JsonProtList)
	{
		Json::Value const* pValue = Json_FindKeyValue(*iter, protId.c_str());
		if (pValue) return pValue;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
bool FUProtocol::AddFile(const std::string& fileName)
{
	int datalen = 0;
	char* pFileData = fu_getfiledata(fileName.c_str(), datalen);

	Json::Reader  hReader;
	Json::Value*  pToValue = new Json::Value();

	std::string document;
	document.assign(pFileData, datalen);
	delete pFileData;
	if (!hReader.parse(document, *pToValue))
	{
		return false;
	}
	g_JsonProtList.push_back(pToValue);
	return true;
}

bool FUProtocol::Init()
{
	int datalen = 0;
	char* pFileData = fu_getfiledata("globalProt.json", datalen);
	Json::Reader  hReader;
	Json::Value  hValue;

	std::string document;
	document.assign(pFileData, datalen);
	delete pFileData;
	if (!hReader.parse(document, hValue))
	{
		return false;
	}
	const char* protFileKey = "protfile";
	Json::Value const* pFileValue = Json_FindKeyValue(hValue, protFileKey);
	if (!pFileValue) return false;

	for (Json::ValueConstIterator iter = pFileValue->begin();
		 iter != pFileValue->end(); iter++)
	{
		std::string loadStr = fu_string_format("InitGlobalProt: [%s]", iter->asString().c_str());
		if (!AddFile(iter->asString()))
		{
			loadStr += " Error!";
			FULOG(loadStr.c_str());
			return false;
		}
		else
		{
			loadStr += " Success!";
			FULOG(loadStr.c_str());
		}
	}
	return true;
}

void FUProtocol::Destroy()
{
	for (auto iter : g_JsonProtList)
	{
		delete iter;
	}
	g_JsonProtList.clear();
}
//////////////////////////////////////////////////////////////////////////
bool FUProtocol::CreateProt(int protId, Json::Value& outValue)
{
	std::string toKey = fu_string_format("%d", protId);
	Json::Value const* pValue = st_IsProtIdInList(toKey);
	if (!pValue)
	{
		return false;
	}
	outValue = *pValue;
	outValue["msgId"] = protId;
	return true;
}

int FUProtocol::RecvProt(const char* pbuf, int buflen, Json::Value& outValue)
{
	Json::Reader  hReader;
	if (!hReader.parse(pbuf, pbuf + buflen, outValue))
	{
		return -1; //协议错误，非有效协议
	}
	const char* pMsgIdStr = "msgId";
	const char* pMsgDataStr = "msgData";
	Json::Value const* pIdValue = Json_FindKeyValue(outValue, pMsgIdStr);
	if (!pIdValue) return -1; //协议错误，非有效协议

	if (!Json_FindKeyValue(outValue, pMsgDataStr)) return -1; //协议错误，非有效协议
	
	std::string protIdStr = pIdValue->asString();
	if (!st_IsProtIdInList(protIdStr)) return -2; //没有定义过该协议
	

	//outValue = *pValue;
	return atoi(protIdStr.c_str());
}

bool FUProtocol::SendProt(int socket, const Json::Value& inValue)
{
	if (g_pNetServer)
	{
		std::string outString = inValue.toStyledString();
		g_pNetServer->SendText(socket, outString.c_str(), outString.length());
	}
	return true;
}

bool FUProtocol::SendError(int socket, int protId)
{
	if (g_pNetServer)
	{
		Json::Value hValue;
		CreateProt(protId, hValue);
		SendProt(socket, hValue);
	}
	return true;
}

