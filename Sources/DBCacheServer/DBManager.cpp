#include "DBManager.h"
#include "../Utils/UtilsSerialize.h"
#include "../Utils/UtilsTime.h"
#include "../Utils/UtilsLog.h"


void* st_BackupSQLThreadProc(void* lpParam)
{
	DBManager* pDBMgr = (DBManager*)lpParam;

	while (!pDBMgr->IsClose())
	{
		pDBMgr->Update(0.1f);
		UtilsTime::SleepUsec(100);
	}
	pDBMgr->Update(0);

	return 0;
}
//////////////////////////////////////////////////////////////////////////
DBManager::DBManager()
: m_pDBLink(NULL)
, m_sCurrentDB("")
, m_pHandler(NULL)
, m_bClose(false)
{
	m_pDBLink = new DBMysql();
	m_pHandler = new UtilsThread();
}

DBManager::~DBManager()
{
	m_bClose = true;
	m_pHandler->Destroy(true);
	FU_SAFE_DELETE(m_pDBLink);
	FU_SAFE_DELETE(m_pHandler);
}

void DBManager::Init()
{
	m_pHandler->Create(st_BackupSQLThreadProc, (char*)this);
}

void DBManager::Update(float dt)
{
	if (!m_TableSet.empty())
	{
		static std::unordered_map<DBRecord*, std::string>  st_backupCmdList;
		st_backupCmdList.clear();
		for (auto& iter : m_TableSet)
		{
			DBTable* pTable = iter.second;
			float backtime = pTable->GetBackupTime() + dt;
			if (backtime>=DB_BACKUP_LIMITE_TIME || this->IsClose())
			{
				backtime = 0;

				pTable->BackupTableCmd(st_backupCmdList);
				for (auto& cmdIter : st_backupCmdList)
				{
					if (!m_pDBLink->RunSqlCMD(cmdIter.second.c_str(), false))
					{
						//FUFileLog(cmdStr.c_str(), "sqlcmd.txt");
						FUAssert(false, "RunSqlCMD ERROR!");
						FUFileLog(cmdIter.second.c_str(), "sqlcmderror.txt");
					}
					cmdIter.first->SetIsInsert(false);
					cmdIter.first->SetIsModify(false);
				}
			}
			pTable->SetBackupTime(backtime);
		}
	}
}

void DBManager::CreateLink(const char* host, const char* uname, const char* pwd, const char* dbname, unsigned int port /*= 3306*/)
{
	if (m_pDBLink->Connect(host, uname, pwd, dbname, port))
	{
		m_sCurrentDB = dbname;
	}
}

void DBManager::DestroyLink()
{
	m_pDBLink->Destroy();
}

bool DBManager::LoadTable(const std::string& dbname, const std::string& tname)
{
	auto iter = m_TableSet.find(tname);
	if (iter != m_TableSet.end())return false;
	if (dbname != "" && !ChangeDB(dbname)) return false;
	
	std::vector<std::string>		nameList;
	std::vector<unsigned short>		typeList;
	int priKeyIndex = m_pDBLink->CheckFieldInfo(tname, nameList, typeList);
	if (priKeyIndex<0) return false;
	
	std::string sqlCmdStr = fu_string_format(1024, "select * from %s", tname.c_str());
	if (!m_pDBLink->RunSqlCMD(sqlCmdStr.c_str(), true))
	{
		return false;
	}
	DBTable* pTable = new DBTable(nameList, typeList, priKeyIndex);
	if (!m_pDBLink->CheckToTable(tname, pTable))
	{
		FU_SAFE_DELETE(pTable);
		return false;
	}
	pTable->SetTableName(tname);
	pTable->SetDBName(m_sCurrentDB);
	m_TableSet.insert(std::make_pair(tname, pTable));

	return true;
}

bool DBManager::ChangeDB(const std::string& dbname)
{
	if (dbname == "") return false;
	std::string sqlCmdStr = fu_string_format(1024, "use %s;", dbname.c_str());
	if (!m_pDBLink->RunSqlCMD(sqlCmdStr.c_str(), false))
	{
		return false;
	}
	m_sCurrentDB = dbname;
	return true;
}

bool DBManager::QueryDB(std::vector<std::vector<std::string> >& recordList, 
	std::vector<std::string>& nameList, 
	const std::string& tname, 
	const std::string& fieldName,
	const std::string& dataEquKey,
	const std::string& dataMinkey, 
	const std::string& dataMaxkey)
{
	if (fieldName == "" || tname == "") return false;
	if (dataEquKey == "" && dataMinkey == "" && dataMaxkey == "") return false;

	auto iter = m_TableSet.find(tname);
	if (iter == m_TableSet.end())return false;
	DBTable* pTable = iter->second;

	pTable->GetFieldName(nameList);
	return pTable->QueryRecord(fieldName, dataEquKey, dataMinkey, dataMaxkey, recordList);
}

bool DBManager::ModifyDB(const std::string& tname, const std::string& dataKey, std::unordered_map<std::string, std::string>& dataList)
{
	if (dataKey == "" || tname=="") return false;

	auto iter = m_TableSet.find(tname);
	if (iter == m_TableSet.end())return false;
	DBTable* pTable = iter->second;
	
	return pTable->ModifyRecord(dataKey, dataList) != NULL;
}

std::string DBManager::InsertDB(const std::string& tname, const std::unordered_map<std::string, std::string>& dataList)
{
	if (tname == "") return "";

	auto iter = m_TableSet.find(tname);
	if (iter == m_TableSet.end())return false;
	DBTable* pTable = iter->second;

	DBRecord* pRecord = pTable->InsertRecord(dataList);
	if (!pRecord) return "";
	
	pRecord->SetIsInsert(true);
	return pRecord->GetField(pTable->GetPriKeyIndex())->GetData();
}

