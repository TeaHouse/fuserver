
#include "DBRecord.h"
#include "../Utils/UtilsSerialize.h"
#include "../Utils/UtilsLog.h"

DBRecord::DBRecord()
: m_bInsert(false)
, m_bModify(false)
{
	m_FieldList.clear();
}

DBRecord::~DBRecord()
{
	for (auto& pField : m_FieldList)
	{
		delete pField;
	}
	m_FieldList.clear();
}

void DBRecord::Serialize(char* buf, int& size)
{
	int numField = m_FieldList.size();
	memcpy(buf + size, &(numField), sizeof(int));
	size += sizeof(int);
	for (auto& pField : m_FieldList)
	{
		pField->Serialize(buf, size);
	}
}

void DBRecord::unSerialize(const char* buf, int& size)
{
	int numField = *(int*)(buf + size);
	size += sizeof(int);
	for (int i = 0; i < numField; i++)
	{
		DBField* pField = new DBField();
		pField->unSerialize(buf, size);
		PushBack(pField);
	}
}

int DBRecord::SerializeSize()
{
	int dataSize = sizeof(int);
	for (auto& pField : m_FieldList)
	{
		dataSize += pField->SerializeSize();
	}
	return dataSize;
}

void DBRecord::Print()
{
	for (auto& pField : m_FieldList)
	{
		pField->Print();
	}
}

std::string DBRecord::CheckModifySqlCmd(const std::string& tname, const std::vector<std::string>& nameList, int keyIndex)
{
	std::string toCmdStr = fu_string_format(tname.length() + 50, "update %s set", tname.c_str());

	for (int i = 0; i < (int)nameList.size(); i++)
	{
		if (i != keyIndex)
		{
			DBField* pField = m_FieldList[i];
			int maxCharSize = pField->SerializeSize() + nameList[i].length();
			toCmdStr += fu_string_format(maxCharSize, " %s='%s' ",
				nameList[i].c_str(), pField->GetData().c_str());
		}
	}
	const std::string& keyName = nameList[keyIndex];
	DBField* pKeyField = m_FieldList[keyIndex];
	int keyCharSize = pKeyField->SerializeSize() + keyName.length();
	toCmdStr += fu_string_format(keyCharSize, "where %s='%s';",
		keyName.c_str(), pKeyField->GetData().c_str());

	FULOG(toCmdStr);
	return toCmdStr;
}

std::string DBRecord::ChcekInsertSqlCmd(const std::string& tname, const std::vector<std::string>& nameList, int keyIndex)
{
	std::string nameStr = "(";
	std::string valueStr = "(";
	int fieldCount = nameList.size();
	for (int i = 0; i < fieldCount; i++)
	{
		if (i != fieldCount-1)
		{
			nameStr += fu_string_format(nameList[i].length() + 5, "%s, ", nameList[i].c_str());
			valueStr += fu_string_format(GetField(i)->SerializeSize(), "'%s', ", GetField(i)->GetData().c_str());
		}
		else
		{
			nameStr += fu_string_format(nameList[i].length() + 5, "%s)", nameList[i].c_str());
			valueStr += fu_string_format(GetField(i)->SerializeSize(), "'%s')", GetField(i)->GetData().c_str());
		}
	}
	std::string toCmdStr = fu_string_format(tname.length() + nameStr.length() + valueStr.length() + 50,
				"insert into %s%s values%s;", tname.c_str(), nameStr.c_str(), valueStr.c_str());

	FULOG(toCmdStr);
	return toCmdStr;
}

void DBRecord::ResetData(DBRecord* pRecord)
{
	for (auto& pField : m_FieldList)
	{
		pField->ResetData(pField);
	}
}

void DBRecord::ResetData(DBField* pField, int index)
{
	m_FieldList[index]->ResetData(pField);
}

void DBRecord::ResetData(const std::string& sData, int index)
{
	m_FieldList[index]->ResetData(sData);
}

void DBRecord::PushBack(DBField* pField, int index /*= -1*/)
{
	if (index==-1)
	{
		m_FieldList.push_back(pField);
	}
	else
	{
		m_FieldList.insert(m_FieldList.begin() + index, pField);
	}
}

DBField* DBRecord::GetField(const int& index)
{
	return m_FieldList[index];
}

void DBRecord::CheckDataList(std::vector<std::string>& dataList)
{
	for (auto& pField : m_FieldList)
	{
		dataList.push_back(pField->GetData());
	}
}

