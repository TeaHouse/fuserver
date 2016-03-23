#ifndef __FU_DATABASE_MYSQL_H__
#define __FU_DATABASE_MYSQL_H__

#include "DBTable.h"

class DBMysql
{
public:
	DBMysql();
	~DBMysql();
	
	bool Connect(const char* host, const char* uname, const char* pwd, const char* dbname, unsigned int port = 3306);	//��������
	bool ChangeDB(const char* dbname);
	void Destroy();

	bool RunSqlCMD(const char* cmdStr, bool bResult); //bResult��ʾ�Ƿ�Ҫ��ȡ�����
	unsigned short ExchangeDataType(int mysqlType);//����mysql���������ͣ�����dbbase��������� 
public:
	//��Խ����
	bool CheckQueryRecord(std::vector<DBRecord*>& recordList);//ͨ��RunSqlCMD��Ľ������ת����һ�Ѽ�¼
	bool CheckToTable(const std::string& tname, DBTable* pTable);//����select*from table���������ű�
public:
	//����ֶμ�
	int  CheckFieldInfo(const std::string& tname, std::vector<std::string>& nameList, std::vector<unsigned short>& typeList);//����keyIndex

public:
	char*	m_pMysql;
	char*	m_pMysqlRes;
};








#endif