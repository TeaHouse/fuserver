
#include "DBMysql.h"
#include "mysql.h"
#include "../Utils/UtilsConvert.h"
#include "../Utils/UtilsSerialize.h"
#include "../Utils/UtilsLog.h"

#define st_mysql_handler(pMysql)	((MYSQL*)pMysql)
#define st_result_handler(pMysqlRes)   ((MYSQL_RES*)pMysqlRes)
#define ST_IS_AUTO_INCREMENT(flag)		(flag&AUTO_INCREMENT_FLAG)

MYSQL_RES* st_MysqlQuery(MYSQL* pMysql, const char* sqlStr, bool bResult)
{
	if (NULL == pMysql || NULL == sqlStr){
		return NULL;
	}
	if (0 != mysql_ping(pMysql) || 0 != mysql_query(pMysql, sqlStr)){
		printf("st_MysqlQuery Error: mysql_query sqlstr=[%s], errstr=[%s]", sqlStr, mysql_error(pMysql));
		return NULL;
	}
	if (bResult)
	{
		return mysql_store_result(pMysql);
	}
	return (MYSQL_RES * )1;
}
void st_FreeQuery(MYSQL_RES* result)
{
	mysql_free_result(result);
}
//////////////////////////////////////////////////////////////////////////
DBMysql::DBMysql()
: m_pMysql(NULL)
, m_pMysqlRes(NULL)
{

}

DBMysql::~DBMysql()
{
	Destroy();
}

bool DBMysql::Connect(const char* host, const char* uname, const char* pwd, const char* dbname, unsigned int port /*= 0*/)
{
	if (m_pMysql)
	{
		return false;
	}
	m_pMysql = (char*)mysql_init(NULL);
	if (!m_pMysql)
	{
		printf("DBMysql::Create Error: mysql_init Error=[%s]", mysql_error(st_mysql_handler(m_pMysql)));
		return false;
	}
	if (!mysql_real_connect(st_mysql_handler(m_pMysql), host, uname, pwd, dbname, port, NULL, 0)){
		printf("DBMysql::Create Error: mysql_real_connect [%s]", mysql_error(st_mysql_handler(m_pMysql)));
		return false;
	}
	return true;
}

bool DBMysql::ChangeDB(const char* dbname)
{
	if (m_pMysql && 0 == mysql_select_db(st_mysql_handler(m_pMysql), dbname))
	{
		return true;
	}
	return false;
}

void DBMysql::Destroy()
{
	if (m_pMysqlRes)
	{
		mysql_free_result(st_result_handler(m_pMysqlRes));
		m_pMysqlRes = NULL;
	}
	if (m_pMysql)
	{
		mysql_close(st_mysql_handler(m_pMysql));
		m_pMysql = NULL;
	}
}

bool DBMysql::RunSqlCMD(const char* cmdStr, bool bResult)
{
	m_pMysqlRes = (char*)st_MysqlQuery(st_mysql_handler(m_pMysql), cmdStr, bResult);

	return m_pMysqlRes != NULL;
}

unsigned short DBMysql::ExchangeDataType(int mysqlType)
{
	switch (mysqlType){
	case MYSQL_TYPE_TINY:
	case MYSQL_TYPE_SHORT:
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
		return db_int_t;
	case MYSQL_TYPE_FLOAT:
	case MYSQL_TYPE_DOUBLE:
		return db_double_t;
	case MYSQL_TYPE_LONGLONG:
		return db_longlong_t;
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VAR_STRING:
	case MYSQL_TYPE_BIT:
	case MYSQL_TYPE_BLOB:
		return db_string_t;
	default:
		FUAssert(false, fu_string_format(100, "ExchangeDataType Error Type[%d]", mysqlType));
	}
	return 0;
}

bool DBMysql::CheckQueryRecord(std::vector<DBRecord*>& recordList)
{
	if (!m_pMysqlRes) return false;
	MYSQL_RES* pResult = st_result_handler(m_pMysqlRes);
	int fieldCount = mysql_num_fields(pResult);
	unsigned long long rowCount = mysql_num_rows(pResult);
	for (unsigned long long j = 0; j < rowCount; j++)
	{
		MYSQL_ROW pDataArray = mysql_fetch_row(pResult);
		unsigned long* pLenArray = mysql_fetch_lengths(pResult);
		DBRecord* pRecord = new DBRecord();
		for (int i = 0; i < fieldCount; i++)
		{
			MYSQL_FIELD* pField = mysql_fetch_field(pResult);
			DBField* pMyField = new DBField(ExchangeDataType(pField->type));
			pMyField->SetData(pDataArray[i], pLenArray[i]);
			pMyField->SetIndex(i);
			pMyField->SetIsKey(IS_PRI_KEY(pField->flags) != 0);
			pRecord->PushBack(pMyField);
		}
		recordList.push_back(pRecord);
	}
	return true;
}

bool DBMysql::CheckToTable(const std::string& tname, DBTable* pTable)
{
	MYSQL_RES* pResult = st_result_handler(m_pMysqlRes);
	int fieldCount = mysql_num_fields(pResult);
	unsigned long long rowCount = mysql_num_rows(pResult);

	int* pSqlFieldArray = new int[fieldCount];
	memset(pSqlFieldArray, 0, fieldCount*sizeof(int));
	for (unsigned long long j = 0; j < rowCount; j++)
	{
		MYSQL_ROW pDataArray = mysql_fetch_row(pResult);
		unsigned long* pLenArray = mysql_fetch_lengths(pResult);
		DBRecord* pRecord = new DBRecord();
		for (int i = 0; i < fieldCount; i++)
		{
			MYSQL_FIELD* pField = NULL;
			if (pSqlFieldArray[i])
			{
				pField = (MYSQL_FIELD*)pSqlFieldArray[i];
			}
			else
			{
				pField = mysql_fetch_field(pResult);
				pSqlFieldArray[i] = (int)pField;
			}			 
			DBField* pMyField = new DBField(ExchangeDataType(pField->type));
			pMyField->SetData(pDataArray[i], pLenArray[i]);
			pMyField->SetIndex(i);
			pMyField->SetIsKey(IS_PRI_KEY(pField->flags) != 0);
			if (pMyField->IsPriKey())
			{
				pTable->SetIsAutoIncrement(ST_IS_AUTO_INCREMENT(pField->flags) != 0);
			}
			pRecord->PushBack(pMyField);
		}
		if (!pTable->InsertRecord(pRecord))
		{
			FU_SAFE_DELETE_ARRAY(pSqlFieldArray);
			return false;
		}
	}
	FU_SAFE_DELETE_ARRAY(pSqlFieldArray);
	return true;
}

int DBMysql::CheckFieldInfo(const std::string& tname, std::vector<std::string>& nameList, std::vector<unsigned short>& typeList)
{
	MYSQL_RES* pResult = mysql_list_fields(st_mysql_handler(m_pMysql), tname.c_str(), NULL);

	int fieldCount = mysql_num_fields(pResult);
	int priKeyIndex = -1;
	for (int i = 0; i < fieldCount; i++)
	{
		MYSQL_FIELD* pField = mysql_fetch_field(pResult);
		nameList.push_back(pField->name);
		typeList.push_back(ExchangeDataType(pField->type));
		if (IS_PRI_KEY(pField->flags) != 0)
		{
			priKeyIndex = i;
		}
		/*
		int fieldType = ExchangeDataType(pField->type);
		printf("--name=[%s]--type=[%d]---len=[%d]--maxlen=[%d]--isKey=[%d]----\n",
			pField->name, fieldType, pField->length, pField->max_length,
			IS_PRI_KEY(pField->flags));
		*/
	}
	st_FreeQuery(pResult);
	//printf("-----priKeyIndex=[%d]----\n", priKeyIndex);
	return priKeyIndex;
}

