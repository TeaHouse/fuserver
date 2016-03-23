#ifndef __FU_SERVER_DB_SQLMANAGER_H__
#define __FU_SERVER_DB_SQLMANAGER_H__

#include "DBSqlServer.h"
#include "../Utils/UtilsSingleton.h"

class DBSqlManager : public UtilsSingleton<DBSqlManager>
{
public:
	DBSqlManager();
	~DBSqlManager();

	bool Init();

	void Destroy();

public:
	bool QueryRecord(const std::string& tkeyData, DBDataSet& dataSet); //仅仅通过主键查询,若主键为空则查询所有
	bool QueryRecord(const std::unordered_map<std::string, std::string>& condition, DBDataSet& dataSet);//通过字段条件查询
	bool NewRecord(DBDataSet& dataSet); //新插入一条自增记录

	/*tkeyName=""插入只有主键需要赋值的记录，
	tkeyData=""，则插入的主键有自增属性的记录
	tkeyName=""和tkeyData="",插入空字段,且必须在主键自增，而且其他字段没有非空属性
	*/

	bool CommitInsert(DBDataSet& dataSet);
	bool CommitModify(DBDataSet& dataSet); //将修改后的记录写入数据库

	bool ExcuteProc(const std::string& procName, DBProcData* procData, DBDataSet& dataSet);

	void CheckTableAttribute(const std::string& tname, std::string& keyName, int& columnNum, bool& isIdentity);
	
private:
	DBSqlServer		m_hSqlServer;
};



#endif