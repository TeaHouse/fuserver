
#include "DBTable.h"
#include "../Utils/UtilsConvert.h"
#include "../Utils/UtilsSerialize.h"

DBTable::DBTable(const std::vector<std::string>& nameList, const std::vector<unsigned short>& typeList, int iKeyIndex)
	: m_bAutoIncrement(false)
	, m_iPriKeyIndex(-1)
	, m_sTableName("")
	, m_sDBName("")
	, m_fBackupTime(0)
{
	this->ClearnRecord();
	for (int i = 0; i < (int)nameList.size(); i++)
	{
		m_FieldIndexList.insert(std::make_pair(nameList[i], i));
		m_FieldNameList.push_back(nameList[i]);
		m_FieldTypeList.push_back(typeList[i]);
	}
	m_iPriKeyIndex = iKeyIndex;
}

DBTable::~DBTable()
{
	this->ClearnRecord();
}

void DBTable::BackupTableCmd(std::unordered_map<DBRecord*, std::string>& cmdList)
{
	for (auto& iter : m_RecordSet)
	{
		DBRecord* pRecord = iter.second;
		if (pRecord->GetIsInsert())
		{
			cmdList.insert(std::make_pair(pRecord, CheckInsertSqlCmd(pRecord)));
			
		}
		else if (pRecord->GetIsModify())
		{
			cmdList.insert(std::make_pair(pRecord, CheckModifySqlCmd(pRecord)));
		}
	}
}

DBRecord* DBTable::InsertRecord(std::string dataKey)
{
	if (dataKey == "") dataKey = PriKeyIncrement();
	if (dataKey == "") return NULL;
	if (this->FindSet(dataKey)) return NULL;//是否已经存在

	m_Lock.Lock();
	DBRecord* pRecord = new DBRecord();
	DBField* pField = NULL;
	int fieldCount = m_FieldNameList.size();
	for (int i = 0; i < fieldCount; i++)
	{
		pField = new DBField(this->GetFieldType(i));
		pField->SetIsKey(i == m_iPriKeyIndex);
		if (pField->IsPriKey())
		{
			pField->SetData(dataKey);
		}
		pRecord->PushBack(pField);
	}
	pRecord = InsertSet(dataKey, pRecord);
	m_Lock.unLock();
	return pRecord;
}
DBRecord* DBTable::InsertRecord(DBRecord* pRecord)
{
	DBField* pKeyField = pRecord->GetField(m_iPriKeyIndex);
	if (pKeyField->IsNull() || this->FindSet(pKeyField->GetData())) return NULL;

	m_Lock.Lock();

	pRecord = InsertSet(pKeyField->GetData(), pRecord);

	m_Lock.unLock();
	return pRecord;
}
DBRecord* DBTable::InsertRecord(const std::unordered_map<std::string, std::string>& dataList)
{
	if (dataList.empty())
	{
		return InsertRecord("");
	}
	std::string keyName = this->GetFieldName(m_iPriKeyIndex);
	std::string dataKey = "";
	auto diter = dataList.find(keyName); //找出主键
	if (diter != dataList.end())
	{
		dataKey = diter->second;
	}
	else
	{
		dataKey = PriKeyIncrement();
	}
	
	if (this->FindSet(dataKey)) return NULL;//是否已经存在

	m_Lock.Lock();
	DBRecord* pRecord = new DBRecord();
	int fieldCount = m_FieldNameList.size();
	for (int i = 0; i < fieldCount; i++)
	{
		std::string fieldName = this->GetFieldName(i);
		DBField* pField = new DBField(this->GetFieldType(i));
		pField->SetIsKey(fieldName == keyName);
		pField->SetIndex(i);
		if (pField->IsPriKey())
		{
			pField->SetData(dataKey);
		}
		else
		{
			auto iter = dataList.find(fieldName);
			if (iter != dataList.end())
			{
				pField->SetData(iter->second);
			}
		}		
		pRecord->PushBack(pField);
	}
	pRecord = InsertSet(dataKey, pRecord);
	m_Lock.unLock();
	return pRecord;
}

DBRecord* DBTable::InsertRecord(const std::vector<std::string>& dataList)
{
	int fieldCount = m_FieldNameList.size();
	if (fieldCount != (int)dataList.size()) return NULL;

	std::string dataKey = dataList[m_iPriKeyIndex];
	if (this->FindSet(dataKey)) return NULL;//是否已经存在
	m_Lock.Lock();

	DBRecord* pRecord = new DBRecord();
	for (int i = 0; i < fieldCount; i++)
	{
		DBField* pField = new DBField(this->GetFieldType(i));
		pField->SetIsKey(i == m_iPriKeyIndex);
		pField->SetIndex(i);
		pField->SetData(dataList[i]);

		pRecord->PushBack(pField);
	}
	pRecord = InsertSet(dataKey, pRecord);
	m_Lock.unLock();
	return pRecord;
}

DBRecord* DBTable::InsertRecord(const std::unordered_map<int, std::string>& dataList)
{
	std::string dataKey;
	auto diter = dataList.find(m_iPriKeyIndex);
	if (diter != dataList.end())
	{
		dataKey = diter->second;
	}
	else
	{
		dataKey = PriKeyIncrement();
	}

	if (this->FindSet(dataKey)) return NULL;//是否已经存在
	m_Lock.Lock();
	DBRecord* pRecord = new DBRecord();
	int fieldCount = m_FieldNameList.size();
	for (int i = 0; i < fieldCount; i++)
	{
		DBField* pField = new DBField(this->GetFieldType(i));
		pField->SetIsKey(i == m_iPriKeyIndex);
		pField->SetIndex(i);
		if (pField->IsPriKey())
		{
			pField->SetData(dataKey);
		}
		else
		{
			auto iter = dataList.find(i);
			if (iter != dataList.end())
			{
				pField->SetData(iter->second);
			}
		}
		pRecord->PushBack(pField);
	}
	pRecord = InsertSet(dataKey, pRecord);
	m_Lock.unLock();
	return pRecord;
}

bool DBTable::QueryRecord(const std::string& fieldName, 
	const std::string& dataEquKey,
	const std::string& dataMinkey, 
	const std::string& dataMaxkey, 
	std::vector<std::vector<std::string> >& recordList)
{
	bool temp = false;
	int fieldIndex = GetFieldIndex(fieldName);
	if (fieldIndex == m_iPriKeyIndex && dataEquKey != "")
	{
		auto iter = m_RecordSet.find(dataEquKey);
		if (iter != m_RecordSet.end())
		{
			std::vector<std::string> dataList;
			iter->second->CheckDataList(dataList);
			recordList.push_back(dataList);
			temp = true;
		}
	}
	else if (m_RecordSet.size() > 0)
	{
		auto iter01 = m_RecordSet.begin();
		auto iter02 = m_RecordSet.end();
		iter02--;
		int beginIndex = 0;
		int endIndex = m_RecordSet.size();
		bool insertFlag01 = false;
		bool insertFlag02 = false;
		std::vector<std::string> dataList01;
		std::vector<std::string> dataList02;
		DBField* pField01 = NULL;
		DBField* pField02 = NULL;
		do
		{
			insertFlag01 = false;
			insertFlag02 = false;
			pField01 = iter01->second->GetField(fieldIndex);
			pField02 = iter02->second->GetField(fieldIndex);
			if (dataEquKey != "")
			{
				if (DB_CompareDataKey(pField01->GetData(), dataEquKey, pField01->GetDataType()) == 0)
				{
					insertFlag01 = true;
				}
				if (pField01 != pField02 &&
					DB_CompareDataKey(pField02->GetData(), dataEquKey, pField02->GetDataType()) == 0)
				{
					insertFlag02 = true;
				}
			}
			else if (dataMinkey != "" && dataMaxkey != "")
			{
				if (DB_CompareDataKey(pField01->GetData(), dataMinkey, pField01->GetDataType()) >= 0 && 
					DB_CompareDataKey(pField01->GetData(), dataMaxkey, pField01->GetDataType()) <= 0)
				{
					insertFlag01 = true;
				}
				if (pField01 != pField02 &&
					DB_CompareDataKey(pField02->GetData(), dataMinkey, pField02->GetDataType()) >= 0 &&
					DB_CompareDataKey(pField02->GetData(), dataMaxkey, pField02->GetDataType()) <= 0)
				{
					insertFlag02 = true;
				}
			}
			else
			{
				if (dataMinkey != "")
				{
					if (DB_CompareDataKey(pField01->GetData(), dataMinkey, pField01->GetDataType()) >= 0)
					{
						
						insertFlag01 = true;
					}
					if (pField01 != pField02 &&
						DB_CompareDataKey(pField02->GetData(), dataMinkey, pField02->GetDataType()) >= 0)
					{
						insertFlag02 = true;
					}
				}
				if (pField01 != pField02 && dataMaxkey != "")
				{
					if (DB_CompareDataKey(pField01->GetData(), dataMaxkey, pField01->GetDataType()) <= 0)
					{
						insertFlag01 = true;
					}
					if (pField01 != pField02 &&
						DB_CompareDataKey(pField02->GetData(), dataMaxkey, pField02->GetDataType()) <= 0)
					{						
						insertFlag02 = true;
					}
				}
			}
			if (insertFlag01)
			{
				dataList01.clear();
				iter01->second->CheckDataList(dataList01);
				recordList.push_back(dataList01);
				temp = true;
			}
			if (pField01 != pField02 && insertFlag02)
			{
				dataList02.clear();
				iter02->second->CheckDataList(dataList02);
				recordList.push_back(dataList02);
				temp = true;
			}
			beginIndex++;
			endIndex--;
			iter01++;
			iter02--;
		} while (beginIndex < endIndex);
	}
	return temp;
}

DBRecord* DBTable::ModifyRecord(DBRecord* pRecord)
{
	DBField* pKeyField = pRecord->GetField(m_iPriKeyIndex);
	if (pKeyField->IsNull()) return NULL;

	auto iter = m_RecordSet.find(pKeyField->GetData());
	if (iter == m_RecordSet.end()) return NULL;

	m_Lock.Lock();
	iter->second->ResetData(pRecord);
	iter->second->SetIsModify(true);
	m_Lock.unLock();
	return iter->second;
}

DBRecord* DBTable::ModifyRecord(const std::string& dataKey, const std::string& fieldName, const std::string& sData)
{
	int index = GetFieldIndex(fieldName);
	return ModifyRecord(dataKey, index, sData);
}

DBRecord* DBTable::ModifyRecord(const std::string& dataKey, int index, const std::string& sData)
{
	auto iter = m_RecordSet.find(dataKey);
	if (iter != m_RecordSet.end())
	{
		m_Lock.Lock();
		iter->second->ResetData(sData, index);
		iter->second->SetIsModify(true);
		m_Lock.unLock();
		return iter->second;
	}
	return NULL;
}

DBRecord* DBTable::ModifyRecord(const std::string& dataKey, const std::unordered_map<std::string, std::string>& dataList)
{
	auto iter = m_RecordSet.find(dataKey);
	if (iter == m_RecordSet.end()) return NULL;
	DBRecord* pRecord = iter->second;
	m_Lock.Lock();
	for (auto& toIter : dataList)
	{
		int index = this->GetFieldIndex(toIter.first);
		if (index != m_iPriKeyIndex)
		{
			pRecord->ResetData(toIter.second, index);
			pRecord->SetIsModify(true);
		}
	}
	m_Lock.unLock();

	return pRecord;
}

void DBTable::ClearnRecord()
{
	m_Lock.Lock();
	m_sTableName = "";
	m_iPriKeyIndex = -1;	//主键下标
	m_sMinKey = "";
	m_sMaxKey = "";
	m_FieldNameList.clear();
	m_FieldIndexList.clear();
	m_FieldTypeList.clear();
	for (auto& iter : m_RecordSet)
	{
		delete iter.second;
	}
	m_RecordSet.clear();
	m_Lock.unLock();
}

std::string DBTable::CheckInsertSqlCmd(DBRecord* pRecord)
{
	return pRecord->ChcekInsertSqlCmd(m_sTableName, m_FieldNameList, m_iPriKeyIndex);
}

std::string DBTable::CheckModifySqlCmd(DBRecord* pRecord)
{
	return pRecord->CheckModifySqlCmd(m_sTableName, m_FieldNameList, m_iPriKeyIndex);
}

long long DBTable::GetTableMemSize()
{
	long long size = 0;
	for (auto& iter : m_RecordSet)
	{
		size += iter.second->SerializeSize();
	}
	return size;
}

void DBTable::GetFieldName(std::vector<std::string>& nameList)
{
	for (auto& name : m_FieldNameList)
	{
		nameList.push_back(name);
	}
}

//////////////////////////////////////////////////////////////////////////
int DBTable::GetFieldIndex(const std::string& fieldName)
{
	auto iter = m_FieldIndexList.find(fieldName);
	if (iter != m_FieldIndexList.end())
	{
		return iter->second;
	}
	return -1;
}

unsigned short DBTable::GetFieldType(int index)
{
	return m_FieldTypeList[index];
}


//////////////////////////////////////////////////////////////////////////
void DBTable::CheckIncrementField(const std::string& dataKey)
{
	if (m_sMinKey == "") m_sMinKey = dataKey;
	if (m_sMaxKey == "") m_sMaxKey = dataKey;

	unsigned short iType = GetFieldType(m_iPriKeyIndex);
	if (DB_CompareDataKey(dataKey, m_sMinKey, iType) < 0)
	{
		m_sMinKey = dataKey;
	}
	if (DB_CompareDataKey(dataKey, m_sMaxKey, iType) > 0)
	{
		m_sMaxKey = dataKey;
	}
}

std::string DBTable::PriKeyIncrement()
{
	int priKeyType = this->GetFieldType(m_iPriKeyIndex);
	switch (priKeyType)
	{
	case db_byte_t:
	case db_int_t:
	case db_usint_t:
	case db_long_t:
	case db_longlong_t:
	case db_string_t:
		{
			return fu_str_increment(1, m_sMaxKey.c_str(), m_sMaxKey.length());
		}
		break;
	case db_float_t:
		{
			float toData = (float)(m_sMaxKey.c_str(), m_sMaxKey.length()) + 1;
			return fu_float_to_str(toData);
		}
	case db_double_t:
		{
			double toData = fu_str_to_double(m_sMaxKey.c_str(), m_sMaxKey.length()) + 1;
			return fu_double_to_str(toData);
		}
	default:
		printf("DBTable::PriKeyIncrement: 未定义数据类型! type=[%d]\n", priKeyType);
		break;
	}
	return "";
}

DBRecord* DBTable::InsertSet(const std::string& dataKey, DBRecord* pRecord)
{
	m_RecordSet.insert(std::make_pair(dataKey, pRecord));
	CheckIncrementField(dataKey);
	return pRecord;
}

DBRecord* DBTable::RemoveSet(const std::string& dataKey)
{
	auto iter = m_RecordSet.find(dataKey);
	if (iter!=m_RecordSet.end())
	{
		DBRecord* pRecord = iter->second;
		m_RecordSet.erase(iter);
		CheckIncrementField(dataKey);
		return pRecord;
	}
	return NULL;
}

DBRecord* DBTable::FindSet(const std::string& dataKey)
{
	auto iter = m_RecordSet.find(dataKey);
	if (iter != m_RecordSet.end())
	{
		return iter->second;
	}
	return NULL;
}

