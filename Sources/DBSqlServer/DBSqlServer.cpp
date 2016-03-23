
#include "DBSqlServer.h"
#include "../Utils/UtilsConvert.h"
#ifdef UNICODE
#undef UNICODE
#endif
#include <sql.h>   
#include <sqlext.h>   
#include <sqltypes.h>   
#include <odbcss.h>   

static bool st_ExcuteSql(SQLHDBC hdbc, const char* sqlStr, SQLHSTMT* phstmt)
{
	RETCODE retcode;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, (SQLHDBC)hdbc, phstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
		return false;
	}
	retcode = SQLExecDirect(*phstmt, (SQLCHAR*)sqlStr, strlen(sqlStr));
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
		return false;
	}
	return true;
}
static void st_ReleaseHstmt(SQLHSTMT hstmt)
{
	//释放语句句柄
	if (hstmt != SQL_NULL_HSTMT) {
		SQLCloseCursor(hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		hstmt = SQL_NULL_HSTMT;
	}
}
static void st_CollectField(SQLHSTMT hstmt, int index, std::string& fieldStr)
{
	char	pFieldBuf[255] = {};
	SQLLEN	nBytesLen = 0;
	while (SQLGetData(hstmt, index, SQL_C_CHAR/*SQL_C_BINARY*/, pFieldBuf, sizeof(pFieldBuf), &nBytesLen) != SQL_NO_DATA)
	{
		if (nBytesLen<=0) break;
		fieldStr.append(pFieldBuf, nBytesLen);
	}
}
static bool st_CollectRecordSet(SQLHSTMT hstmt, DBDataSet& dataSet)
{
	//查询结果集
	int retColumnNum = 0;
	bool temp = false;
	SQLRETURN	ret;
	SQLSMALLINT columnNum;
	SQLNumResultCols(hstmt, &columnNum);
	ret = SQLFetch(hstmt);
	if (ret == SQL_NO_DATA || columnNum <= 0) return temp;

	std::unordered_map<std::string, DBDataField>  toRow;
	char	pFieldName[255] = {};
	SQLSMALLINT   fieldNameLen = 0;
	do
	{
		toRow.clear();
		for (int i = 1; i <= columnNum; i++)
		{
			fieldNameLen = 0;
			SQLDescribeCol(hstmt, i, (SQLCHAR*)pFieldName, sizeof(pFieldName), &fieldNameLen, NULL, NULL, NULL, NULL);
			
			std::string fieldStr;
			st_CollectField(hstmt, i, fieldStr);
			toRow.insert(std::make_pair(pFieldName, fieldStr));
		}
		dataSet.PushRow(toRow);
		temp = true;
	} while ((ret = SQLFetch(hstmt)) != SQL_NO_DATA);
	return temp;
}
//////////////////////////////////////////////////////////////////////////
DBSqlServer::DBSqlServer()
{
	SetDBIP("");
	SetDBUID("");
	SetDBPwd("");
	SetDBName("");
	SetIsLinking(false);
}

DBSqlServer::~DBSqlServer()
{
	Destroy();
}

bool DBSqlServer::Connect(const char* ip, const char* uid, const char* pwd, const char* db)
{
	if (GetIsLinking()) return false;
	SetDBIP(ip);
	SetDBUID(uid);
	SetDBPwd(pwd);
	SetDBName(db);

	std::string connString = GenerateLinkString();
	//SQLCHAR ConnStrIn[255] = "DRIVER={SQL Server}; SERVER=127.0.0.1;UID=test; PWD=test123456;Trusted_Connection=yes;DATABASE=mytest; ";
	
	//FULOG(connString);
	RETCODE retcode;
	SQLHENV henv = SQL_NULL_HENV;
	SQLHDBC hdbc = SQL_NULL_HDBC;
	//1.连接数据源
	//1.环境句柄
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv); /*第二个参数原NULL*/
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	//2.连接句柄 
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	retcode = SQLDriverConnect(hdbc, NULL, (SQLCHAR*)(connString.c_str()), SQL_NTS, NULL, NULL, NULL, SQL_DRIVER_NOPROMPT);
	//判断连接是否成功
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))  
	{
		FULOG("connect db failed [%s],[%d]!", connString.c_str(), retcode);
		Destroy();
		return false;
	}
	m_hEnv = (char*)henv;
	m_hDbc = (char*)hdbc;

	SetIsLinking(true);
	return true;
}

void DBSqlServer::Destroy()
{
	if (GetIsLinking())
	{
		SQLDisconnect((SQLHDBC)m_hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, (SQLHDBC)m_hDbc);
		SQLFreeHandle(SQL_HANDLE_ENV, (SQLHENV)m_hEnv);
		SetIsLinking(false);

		for (auto iter : m_TableAttriMap)
		{
			delete iter.second;
		}
		m_TableAttriMap.clear();
	}
}

bool DBSqlServer::ExcuteCollectSQL(const std::string& sqlStr, DBDataSet& dataSet)
{
	if (!GetIsLinking())
	{
		FULOG("the sql server have not start!");
		return false;
	}
	bool bSuccess = true;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	do
	{
		if (!st_ExcuteSql((SQLHDBC)m_hDbc, sqlStr.c_str(), &hstmt))
		{
			bSuccess = false;
			break;
		}
		bSuccess = st_CollectRecordSet((char*)hstmt, dataSet);
	} while (0);
	if (!bSuccess)
	{
		FUError("ExcuteCollectSQL[%s] failed!", sqlStr.c_str());
	}
	//释放语句句柄
	st_ReleaseHstmt(hstmt);
	return bSuccess;
}

bool DBSqlServer::ExcuteCommitSQL(const std::string& sqlStr)
{
	if (!GetIsLinking())
	{
		FULOG("the sql server have not start!");
		return false;
	}
	bool bSuccess = true;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	if (!st_ExcuteSql((SQLHDBC)m_hDbc, sqlStr.c_str(), &hstmt))
	{
		FUError("ExcuteCommitSQL[%s] failed!", sqlStr.c_str());
		bSuccess = false;
	}
	//释放语句句柄
	st_ReleaseHstmt(hstmt);
	return bSuccess;
}

bool DBSqlServer::ExcuteProcSQL(const char* procStr, DBProcData* procData, DBDataSet& dataSet)
{
	if (!GetIsLinking())
	{
		FULOG("the sql server have not start!");
		return false;
	}
	SQLHSTMT pHstmt = SQL_NULL_HSTMT;
	bool bSuccess = true;
	do
	{
		pHstmt = (char*)procData->Excute(m_hDbc, procStr);
		if (!pHstmt)
		{
			bSuccess = false;
			break;
		}
		bSuccess = st_CollectRecordSet((SQLHSTMT)pHstmt, dataSet);
	} while (0);
	//释放语句句柄
	st_ReleaseHstmt((SQLHSTMT)pHstmt);
	return bSuccess;
}

void DBSqlServer::CheckTableAttribute(const std::string& tname, std::string& keyName, int& columnNum, bool& isIdentity)
{
	if (!GetIsLinking())
	{
		FULOG("the sql server have not start!");
		return ;
	}
	auto iter = m_TableAttriMap.find(tname);
	if (iter != m_TableAttriMap.end())
	{
		isIdentity = iter->second->_identity;
		keyName = iter->second->_primarykey;
		columnNum = iter->second->_columnNum;
		return;
	}

	std::string sqlStr = fu_string_format(
			"select tab1.name, tab2.is_identity, tab3.columnNum from (select a.name from sysobjects t, syscolumns a, sysindexkeys b \
where t.name = '%s' and t.id = a.id and t.id = b.id and b.keyno = 1 and a.colid = b.colid) tab1, sys.columns tab2, \
(select count(*) as columnNum from sys.columns where object_id = object_id('%s')) tab3 \
where tab1.name = tab2.name and tab2.object_id = object_id('%s'); ", tname.c_str(), tname.c_str(), tname.c_str());

	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	if (!st_ExcuteSql((SQLHDBC)m_hDbc, sqlStr.c_str(), &hstmt))
	{
		FUError("CheckTableAttribute[%s] failed!", sqlStr);
		return;
	}
	if (SQLFetch(hstmt) != SQL_NO_DATA)
	{
		std::string keyStr, idenStr, columnStr;
		st_CollectField(hstmt, 1, keyStr);
		st_CollectField(hstmt, 2, idenStr);
		CTabAttri_t* pAtrri = new CTabAttri_t;
		pAtrri->_primarykey = keyStr;
		pAtrri->_identity = (idenStr == "1");
		pAtrri->_columnNum = (int)fu_str_to_integer(columnStr.c_str(), columnStr.length());

		keyName = pAtrri->_primarykey;
		isIdentity = pAtrri->_identity;
		columnNum = pAtrri->_columnNum;
		m_TableAttriMap.insert(std::make_pair(tname, pAtrri));
	}

	//释放语句句柄
	st_ReleaseHstmt(hstmt);
}

//////////////////////////////////////////////////////////////////////////
std::string DBSqlServer::GenerateLinkString()
{
	//return "DRIVER={SQL Server}; SERVER=(local);UID=test; PWD=test123456;Trusted_Connection=yes;DATABASE=mytest; ";
	return fu_string_format("DRIVER={SQL Server};SERVER=%s;UID=%s; PWD=%s;Trusted_Connection=yes;DATABASE=%s;",
							GetDBIP().c_str(), GetDBUID().c_str(), GetDBPwd().c_str(), GetDBName().c_str());
}

