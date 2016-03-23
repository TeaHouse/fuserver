#ifndef __FU_SERVER_DB_SQL_QUERY_RESULT_H__
#define __FU_SERVER_DB_SQL_QUERY_RESULT_H__

#include "../Platform/Include.h"


class DBDataSet;
class DBDataField
{
public:
	DBDataField();
	DBDataField(const std::string& data);
	DBDataField(const DBDataField& field);
	DBDataField& operator=(const DBDataField& field);
	void operator=(int data);
	void operator=(long long data);
	void operator=(float data);
	void operator=(double data);
	void operator=(const std::string& data);
	int	 Size();
	const std::string& Data();
	bool isModify();
	void SetModify(bool flag);
protected:
	std::string		m_sData;
	bool	m_bModify;	//是否修改过
};

class DBDataRow
{
	friend DBDataSet;
public:
	DBDataRow();
	DBDataRow(const DBDataRow& row);
	DBDataRow(const std::unordered_map<std::string, DBDataField>& rowlist);
	DBDataRow& operator=(const DBDataRow& row);
	~DBDataRow();
	DBDataField& operator[](const std::string& fname);
	DBDataField& At(const std::string& fname);
	void SetIsInsert(bool flag);
	void SetModify(bool flag);
	bool isInsert();
private:
	std::unordered_map<std::string, DBDataField>   m_RowList;
	bool	m_bInsert;	//是否新增加
};

#endif