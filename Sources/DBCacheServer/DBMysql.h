#ifndef __FU_DATABASE_MYSQL_H__
#define __FU_DATABASE_MYSQL_H__

#include "DBTable.h"

class DBMysql
{
public:
	DBMysql();
	~DBMysql();
	
	bool Connect(const char* host, const char* uname, const char* pwd, const char* dbname, unsigned int port = 3306);	//创建连接
	bool ChangeDB(const char* dbname);
	void Destroy();

	bool RunSqlCMD(const char* cmdStr, bool bResult); //bResult表示是否要获取结果集
	unsigned short ExchangeDataType(int mysqlType);//传入mysql的数据类型，传出dbbase定义的类型 
public:
	//针对结果集
	bool CheckQueryRecord(std::vector<DBRecord*>& recordList);//通过RunSqlCMD后的结果集，转换成一堆记录
	bool CheckToTable(const std::string& tname, DBTable* pTable);//加载select*from table，加载整张表
public:
	//针对字段集
	int  CheckFieldInfo(const std::string& tname, std::vector<std::string>& nameList, std::vector<unsigned short>& typeList);//返回keyIndex

public:
	char*	m_pMysql;
	char*	m_pMysqlRes;
};








#endif