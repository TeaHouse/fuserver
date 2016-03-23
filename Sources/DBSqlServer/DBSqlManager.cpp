#include "DBSqlManager.h"

static DBDataSet	st_ZeroDataSet("");

DBSqlManager::DBSqlManager()
{
	
}

DBSqlManager::~DBSqlManager()
{
	Destroy();
}

bool DBSqlManager::Init()
{
	int datalen = 0;
	char* pFileData = fu_getfiledata("globalDef.json", datalen);
	Json::Reader  hReader;
	Json::Value	  hRoot;

	std::string document;
	document.assign(pFileData, datalen);
	delete pFileData;
	if (!hReader.parse(document, hRoot))
	{
		FUError("DBSqlManager::Init globalDef.json Error!");
		return false;
	}

	const char* IPConfig = "db_ip";
	const char* UIDConfig = "db_uid";
	const char* PWDConfig = "db_pwd";
	const char* DBConfig = "db_db";
	std::string ip = Json_FindKeyValue(hRoot, IPConfig)->asString();
	std::string uid = Json_FindKeyValue(hRoot, UIDConfig)->asString();
	std::string pwd = Json_FindKeyValue(hRoot, PWDConfig)->asString();
	std::string db = Json_FindKeyValue(hRoot, DBConfig)->asString();


	if (!m_hSqlServer.Connect(ip.c_str(), uid.c_str(), pwd.c_str(), db.c_str()))
	{
		FUError("DBSqlManager::Init ConnectSQL Error!");
		return false;
	}
	FULOG("DBSqlManager::Init ConnectSQL Success! \n\tdbip:\t%s\n\tdbuid:\t%s",
		  ip.c_str(), uid.c_str());
	return true;
}

void DBSqlManager::Destroy()
{
	m_hSqlServer.Destroy();
}
//////////////////////////////////////////////////////////////////////////
bool DBSqlManager::QueryRecord(const std::string& tkeyData, DBDataSet& dataSet)
{
	std::string sqlStr = fu_string_format("select * from %s", dataSet.GetTableName().c_str());
	if (tkeyData != "")
	{
		std::string toStr = fu_string_format(" where %s='%s'", dataSet.GetKeyName().c_str(), tkeyData.c_str());
		sqlStr += toStr;
	}
	sqlStr += ";";

	if (!m_hSqlServer.ExcuteCollectSQL(sqlStr, dataSet))
	{
		FUError("QueryRecord Error[%s]", sqlStr.c_str());
		return false;
	}
	return true;
}


bool DBSqlManager::QueryRecord(const std::unordered_map<std::string, std::string>& condition, DBDataSet& dataSet)
{
	if (condition.empty()) return false;
	std::string sqlStr = fu_string_format("select * from %s where", dataSet.GetTableName().c_str());

	for (auto iter : condition)
	{
		std::string toStr = fu_string_format(" %s='%s',", 
											 iter.first.c_str(), iter.second.c_str());
		sqlStr += toStr;
	}
	sqlStr[sqlStr.length() - 1] = ' ';

	if (!m_hSqlServer.ExcuteCollectSQL(sqlStr, dataSet))
	{
		FUError("QueryRecord Error[%s]", sqlStr.c_str());
		return false;
	}
	return true;
}

bool DBSqlManager::NewRecord(DBDataSet& dataSet)
{
	std::string sqlStr = fu_string_format("set nocount on;insert into %s default values; select @@identity as NewId;",
							  dataSet.GetTableName().c_str());
	if (!m_hSqlServer.ExcuteCollectSQL(sqlStr, dataSet))
	{
		FUError("NewRecord Error[%s]", sqlStr.c_str());
		return false;
	}
	return true;
}

bool DBSqlManager::CommitModify(DBDataSet& dataSet)
{
	std::string sqlStr = fu_string_format("update %s set ", dataSet.GetTableName().c_str());
	bool bHaveModify = false;
	bool fieldloop = dataSet.MoveBeginField();
	while (fieldloop)
	{
		DBDataField& field = dataSet.CurField();
		if (field.isModify())
		{
			std::string toStr = fu_string_format("%s='%s' ",
												 dataSet.CurFieldName().c_str(), field.Data().c_str());
			sqlStr += toStr;
			bHaveModify = true;
		}

		fieldloop = dataSet.MoveNextField();
	} 
	if (!bHaveModify) return false;
	DBDataField& field = dataSet.CurKeyField();
	int tofielLen = field.Size() + dataSet.GetKeyName().length() + 50;
	std::string toStr1 = fu_string_format("where %s='%s';",
										  dataSet.GetKeyName().c_str(), field.Data().c_str());
	sqlStr += toStr1;

	if (!m_hSqlServer.ExcuteCommitSQL(sqlStr))
	{
		FUError("CommitModify Error[%s]", sqlStr.c_str());
		return false;
	}
	return true;
}
bool DBSqlManager::CommitInsert(DBDataSet& dataSet)
{
	if (!dataSet.IsNewRow()) return false;

	std::string sqlStr = fu_string_format("insert into %s", dataSet.GetTableName().c_str());
	std::string keyStr = "(";
	std::string dataStr = "(";
	bool tmploop = dataSet.MoveBeginField();
	while (tmploop)
	{
		std::string str1 = fu_string_format("%s,",
											dataSet.CurFieldName().c_str());
		keyStr += str1;
		DBDataField& field = dataSet.CurField();
		std::string str2 = fu_string_format("'%s',", field.Data().c_str());
		dataStr += str2;
		tmploop = dataSet.MoveNextField();
	}
	keyStr[keyStr.length() - 1] = ')';
	dataStr[dataStr.length() - 1] = ')';

	sqlStr += keyStr;
	sqlStr += " values";
	sqlStr += dataStr;
	sqlStr += ";";

	if (!m_hSqlServer.ExcuteCommitSQL(sqlStr))
	{
		FUError("CommitInsert Error[%s]", sqlStr.c_str());
		return false;
	}
	return true;
}

bool DBSqlManager::ExcuteProc(const std::string& procName, DBProcData* procData, DBDataSet& dataSet)
{
	return m_hSqlServer.ExcuteProcSQL(procName.c_str(), procData, dataSet);
}

void DBSqlManager::CheckTableAttribute(const std::string& tname, std::string& keyName, int& columnNum, bool& isIdentity)
{
	m_hSqlServer.CheckTableAttribute(tname, keyName, columnNum, isIdentity);
}