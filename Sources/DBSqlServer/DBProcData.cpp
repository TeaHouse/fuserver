
#include "DBProcData.h"

#ifdef UNICODE
#undef UNICODE
#endif
#include <sql.h>   
#include <sqlext.h>   
#include <sqltypes.h>   

struct CParam
{
	char*	_udata;
	int		_len;//数据长度
	int		_num;	//当前参数的下标，输出参数放在前面，其他放在后面
	SQLSMALLINT		_ctype;
	SQLSMALLINT		_sqltype;
	SQLSMALLINT		_iotype;

	CParam(int num)
	{
		_len = 0;
		_num = num;
		_iotype = SQL_PARAM_INPUT;
		_ctype = SQL_C_DEFAULT;
		_sqltype = SQL_DEFAULT;
	}
	CParam(double* param, int num, bool bInput)
	{
		_udata = (char*)param;
		_len = sizeof(double);
		_num = num;
		_iotype = SQL_PARAM_INPUT;
		if (!bInput) _iotype = SQL_PARAM_OUTPUT;
		_ctype = SQL_C_DOUBLE;
		_sqltype = SQL_DOUBLE;
	}
	CParam(float* param, int num, bool bInput)
	{
		_udata = (char*)param;
		_len = sizeof(float);
		_num = num;
		_iotype = SQL_PARAM_INPUT;
		if (!bInput) _iotype = SQL_PARAM_OUTPUT;
		_ctype = SQL_C_FLOAT;
		_sqltype = SQL_REAL;
	}
	CParam(short* param, int num, bool bInput)
	{
		_udata = (char*)param;
		_len = sizeof(short);
		_num = num;
		_iotype = SQL_PARAM_INPUT;
		if (!bInput) _iotype = SQL_PARAM_OUTPUT;
		_ctype = SQL_C_SHORT;
		_sqltype = SQL_SMALLINT;
	}
	CParam(long* param, int num, bool bInput)
	{
		_udata = (char*)param;
		_len = sizeof(double);
		_num = num;
		_iotype = SQL_PARAM_INPUT;
		if (!bInput) _iotype = SQL_PARAM_OUTPUT;
		_ctype = SQL_C_LONG;
		_sqltype = SQL_INTEGER;
	}
	CParam(char* param, int len, int num, bool bInput)
	{
		_udata = param;
		_len = len;
		_num = num;
		_iotype = SQL_PARAM_INPUT;
		if (!bInput) _iotype = SQL_PARAM_OUTPUT;
		_ctype = SQL_C_CHAR;
		_sqltype = SQL_VARCHAR;
	}	
};

DBProcData::DBProcData()
	:m_pReturnData(NULL)
{

}

DBProcData::~DBProcData()
{
	ClearParam();
}


char* DBProcData::Excute(char* phdbc, const char* procName)
{
	int numParam = 0;
	std::string sqlStr;
	if (m_pReturnData)
	{
		sqlStr = fu_string_format("{?=call %s", procName);
		numParam++;
	}
	else
	{
		sqlStr = fu_string_format("{call %s", procName);
	}
	if (!m_InputParamList.empty())
	{
		sqlStr += "(";
		for (auto param : m_InputParamList)
		{
			sqlStr += "?,";
		}
		sqlStr[sqlStr.length() - 1] = ')';
		numParam += m_InputParamList.size();
	}
	sqlStr += "};";
	RETCODE retcode;
	SQLHSTMT hstmt = SQL_NULL_HSTMT;
	SQLSMALLINT sqlNumParam = 0;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, (SQLHANDLE)phdbc, &hstmt);  //初始化语句句柄
	SQLPrepare(hstmt, (SQLCHAR *)sqlStr.c_str(), sqlStr.length());
	SQLNumParams(hstmt, &sqlNumParam);
	if (numParam != sqlNumParam)
	{
		FUError("Excute Procedure Error: the param count Illegal!");
		SQLCloseCursor(hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		return NULL;
	}
	int toIndex = 1;
	if (m_pReturnData)
	{
		CParam* pParam = (CParam*)m_pReturnData;
		SQLBindParameter(hstmt, toIndex, pParam->_iotype, pParam->_ctype, pParam->_sqltype,
						 pParam->_len, 0, (SQLPOINTER)(pParam->_udata), pParam->_len, NULL);
		toIndex++;
	}
 	for (auto param : m_InputParamList)
 	{
 		CParam* pParam = (CParam*)param;
 		SQLBindParameter(hstmt, toIndex, pParam->_iotype, pParam->_ctype, pParam->_sqltype,
 						 pParam->_len, 0, (SQLPOINTER)(pParam->_udata), pParam->_len, NULL);
 		toIndex++;
	}
// 	const char* pTabName = "person";
// 	char pPriKeyStr[255] = {};
// 	SQLBindParameter(hstmt, 1, SQL_PARAM_OUTPUT, SQL_C_CHAR, SQL_VARCHAR,
// 					 sizeof(pPriKeyStr), 0, (SQLPOINTER)pPriKeyStr, sizeof(pPriKeyStr), NULL);
// 	SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
// 					 strlen(pTabName), 0, (SQLPOINTER)pTabName, strlen(pTabName), NULL);
	retcode = SQLExecute(hstmt);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
	{
		FUError("Excute Procedure Error: [%s],[%d]!\n", sqlStr.c_str(), retcode);
		SQLCloseCursor(hstmt);
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
		return NULL;
	}
	return (char*)hstmt;
}


void DBProcData::ClearParam()
{
	for (auto param : m_InputParamList)
	{
		delete param;
	}
	m_InputParamList.clear();
	if (m_pReturnData) delete m_pReturnData;
	m_pReturnData = NULL;
}

void DBProcData::PushParam(long* param, bool bInput)	//按堆栈形式压入，第一个入栈表示第一个参数，第二个表示第二个参数
{
	m_InputParamList.push_back((char*)new CParam(param, m_InputParamList.size() + 1, bInput));
}
void DBProcData::PushParam(short* param, bool bInput)
{
	m_InputParamList.push_back((char*)new CParam(param, m_InputParamList.size() + 1, bInput));
}
void DBProcData::PushParam(float* param, bool bInput)
{
	m_InputParamList.push_back((char*)new CParam(param, m_InputParamList.size() + 1, bInput));
}
void DBProcData::PushParam(double* param, bool bInput)
{
	m_InputParamList.push_back((char*)new CParam(param, m_InputParamList.size() + 1, bInput));
}
void DBProcData::PushParam(char* param, int len, bool bInput)
{
	m_InputParamList.push_back((char*)new CParam(param, len, m_InputParamList.size() + 1, bInput));
}
void DBProcData::PushDefaultParam()	//压入默认参数，默认参数写在了存储过程里面
{
	m_InputParamList.push_back((char*)new CParam(m_InputParamList.size() + 1));
}
//////////////////////////////////////////////////////////////////////////
void DBProcData::SetReturnData(long* param)
{
	m_pReturnData = (char*)new CParam(param, 0, false);
}
void DBProcData::SetReturnData(short* param)
{
	m_pReturnData = (char*)new CParam(param, 0, false);
}
void DBProcData::SetReturnData(float* param)
{
	m_pReturnData = (char*)new CParam(param, 0, false);
}
void DBProcData::SetReturnData(double* param)
{
	m_pReturnData = (char*)new CParam(param, 0, false);
}
void DBProcData::SetReturnData(char* param, int len)
{
	m_pReturnData = (char*)new CParam(param, len, 0, false);
}