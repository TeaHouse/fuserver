#ifndef __FUDATABASE_DBRECORD_H__
#define __FUDATABASE_DBRECORD_H__

#include "DBField.h"
#include "../Utils/UtilsFunction.h"

class DBRecord
{
public:
	DBRecord();
	~DBRecord();
	void Serialize(char* buf, int& size);
	void unSerialize(const char* buf, int& size);
	int  SerializeSize();
	void CheckDataList(std::vector<std::string>& dataList);
	void Print();
	std::string CheckModifySqlCmd(const std::string& tname, const std::vector<std::string>& nameList, int keyIndex);
	std::string ChcekInsertSqlCmd(const std::string& tname, const std::vector<std::string>& nameList, int keyIndex);
public:
	void ResetData(DBRecord* pRecord);
	void ResetData(DBField* pField, int index);
	void ResetData(const std::string& sData, int index);

	void PushBack(DBField* pField, int index = -1); //压入节点，节点的压入顺序必须和index顺序一致
	CLASS_GET_OR_SET_PRIVATE(m_bInsert, bool, IsInsert)
	CLASS_GET_OR_SET_PRIVATE(m_bModify, bool, IsModify)
public:
	DBField* GetField(const int& index);
private:
	std::vector<DBField*>   m_FieldList;
};

#endif