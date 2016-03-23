#ifndef __FU_SERVER_DB_SQLSERVER_H__
#define __FU_SERVER_DB_SQLSERVER_H__

#include "DBDataSet.h"
#include "DBProcData.h"
#include "../Utils/UtilsFunction.h"


class DBSqlServer
{
public:
	DBSqlServer();
	~DBSqlServer();

	bool Connect(const char* ip, const char* uid, const char* pwd, const char* db);
	void Destroy();

	/*
		sqlStr为查询语句字符串
		recordDataList为查询结果集中的数据集
		pDescResult表示数据集合的描述
		返回值，字段数量
	*/
	bool ExcuteCollectSQL(const std::string& sqlStr, DBDataSet& dataSet);
	bool ExcuteCommitSQL(const std::string& sqlStr);
	bool ExcuteProcSQL(const char* procStr, DBProcData* procData, DBDataSet& dataSet);

public:
	void CheckTableAttribute(const std::string& tname, std::string& keyName, int& columnNum, bool& isIdentity);

private:
	std::string GenerateLinkString();
	CLASS_GET_OR_SET_PROTECTED(m_sDBIP, std::string, DBIP)
	CLASS_GET_OR_SET_PROTECTED(m_sDBUID, std::string, DBUID)
	CLASS_GET_OR_SET_PROTECTED(m_sDBName, std::string, DBName)
	CLASS_GET_OR_SET_PROTECTED(m_sDBPwd, std::string, DBPwd)
	CLASS_GET_OR_SET_PROTECTED(m_bLinking, bool, IsLinking)

private:
	struct CTabAttri_t
	{
		std::string		_primarykey;	//主键名称
		int		_columnNum;	//表字段数量
		bool	_identity;		//是否自增 
	};
	std::unordered_map<std::string, CTabAttri_t*>	m_TableAttriMap;
	char*	m_hEnv;
	char*	m_hDbc;

};




#endif