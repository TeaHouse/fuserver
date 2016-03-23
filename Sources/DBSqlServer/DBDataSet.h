#ifndef __FU_SERVER_DB_SQL_DATA_SET_H__
#define __FU_SERVER_DB_SQL_DATA_SET_H__

#include "DBDataRow.h"
#include "DBProcData.h"
#include "../Utils/UtilsFunction.h"

class DBDataSet
{
public:
	DBDataSet(const std::string& tname); 
	~DBDataSet();
	DBDataSet(const DBDataSet& set);
	DBDataSet& operator=(const DBDataSet& set);
public:
	void PushRow(const std::unordered_map<std::string, DBDataField>& row); //增加一条记录，且对非空或感兴趣的赋值
public:
	bool New(int keyData);
	bool New(long long keyData);
	bool New(const std::string& keyData); //查找主键，若没有则新增一条记录，但只是对主键赋值, 若为空则约定表格主键是自增
	bool New(const std::unordered_map<std::string, std::string>& codition); //通过条件查询
	bool NewProcedure(const std::string& procName, DBProcData* paramData);
	void Commit();//提交修改或新增
	void printAll();
public:
	/*获取当前迭代器指向的记录的某个字段的值的引用
	 * 若不存在记录中，则增加一个字段
	 */
	DBDataField& operator[](const std::string& fieldName);	
	DBDataField& CurKeyField();
	DBDataField& CurField();
	const std::string& CurFieldName();
	bool MoveNextRow();	//移动迭代器
	bool MoveBeginRow();
	//------------------------------------
	bool MoveNextField();//移动迭代器
	bool MoveBeginField();

	int  RowSize();
	bool IsNewRow();	//是否是新增加的记录
	void SetCurNewRow(bool flag);
	void SetCurModify(bool flag);

	bool Empty();	//是否为空，空返回true
	void Clear();
public:
	CLASS_GET_OR_SET_PROTECTED(m_sTableName, std::string, TableName)
	CLASS_GET_OR_SET_PROTECTED(m_sKeyName, std::string, KeyName)
	CLASS_GET_OR_SET_PROTECTED(m_iColumnNum, int, ColumnNum)
private:	
	typedef std::unordered_map<std::string, DBDataField>	DB_ROW_TYPE;
	DB_ROW_TYPE::iterator		m_RowIterator;
	std::vector<DBDataRow>		m_SetList; //结果集
	int 	m_SetIndex;
	bool	m_bIdentity;
};



#endif