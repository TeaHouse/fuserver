#ifndef __FU_DATABASE_MANAGER_H__
#define __FU_DATABASE_MANAGER_H__

#include "../Utils/UtilsSingleton.h"
#include "../Utils/UtilsThread.h"
#include "DBTable.h"
#include "DBMysql.h"

class DBManager : public UtilsSingleton<DBManager>
{
public:
	DBManager();
	~DBManager();
	void Init();
	void Update(float dt);
	
	void CreateLink(const char* host, const char* uname, const char* pwd, const char* dbname, unsigned int port = 3306);
	void DestroyLink();
	bool LoadTable(const std::string& dbname, const std::string& tname);//将一张表加载进内存
	bool ChangeDB(const std::string& dbname);
	bool QueryDB(std::vector<std::vector<std::string> >& recordList, 
		std::vector<std::string>& nameList,
		const std::string& tname,
		const std::string& fieldName,
		const std::string& dataEquKey,
		const std::string& dataMinkey,
		const std::string& dataMaxkey);
	bool ModifyDB(const std::string& tname, const std::string& dataKey, std::unordered_map<std::string, std::string>& dataList);
	std::string InsertDB(const std::string& tname, const std::unordered_map<std::string, std::string>& dataList);
public:
	bool IsClose()	{ return m_bClose; }

private:
	std::unordered_map<std::string, DBTable*>		m_TableSet; //表格列表，用名称区分，不用db名称区分
	//std::unordered_map<DBTable*, float>				m_RefreshTimeList;	//刷新数据表格的时间
	DBMysql*		m_pDBLink;
	UtilsThread*	m_pHandler;
	std::string		m_sCurrentDB;
	float		m_fRefreshDBTime;	//多少秒就写一次数据库
	bool		m_bClose;

};






#endif