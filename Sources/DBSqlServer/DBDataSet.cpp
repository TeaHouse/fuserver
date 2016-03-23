#include "DBDataSet.h"
#include "../Utils/UtilsConvert.h"
#include "DBSqlManager.h"

static std::unordered_map<std::string, DBDataField> st_ZeroDataRow;
//////////////////////////////////////////////////////////////////////////

DBDataSet::DBDataSet(const std::string& tname)
	: m_sTableName(tname)
	, m_bIdentity(false)
	, m_SetIndex(0)
	, m_iColumnNum(0)
{
	if (m_sTableName != "")
	{
		DBSqlManager::getSingletonPtr()->CheckTableAttribute(m_sTableName, m_sKeyName, m_iColumnNum, m_bIdentity);
	}
}

DBDataSet::~DBDataSet()
{
	Clear();
}

DBDataSet::DBDataSet(const DBDataSet& set)
{
	operator=(set);
}

DBDataSet& DBDataSet::operator=(const DBDataSet& set)
{
	Clear();
	m_SetList = set.m_SetList;
	m_sTableName = set.m_sTableName;
	m_sKeyName = set.m_sKeyName;
	m_bIdentity = set.m_bIdentity;
	m_SetIndex = 0;
	return *this;
}

void DBDataSet::PushRow(const std::unordered_map<std::string, DBDataField>& row)
{
	m_SetList.push_back(row);
}

bool DBDataSet::New(const std::string& keyData)
{
	if (m_sKeyName == "") return false;
	bool toFlag = false;
	bool temp = DBSqlManager::getSingletonPtr()->QueryRecord(keyData, *this);
	toFlag = temp;
	if (!temp)
	{
		if (keyData=="")
		{
			temp = DBSqlManager::getSingletonPtr()->NewRecord(*this);
			toFlag = true;
		}
		else
		{
			DBDataField field(keyData);
			std::unordered_map<std::string, DBDataField> row;
			row.insert(std::make_pair(m_sKeyName, field));
			m_SetList.push_back(row);
			m_SetIndex = m_SetList.size() - 1;
			SetCurNewRow(true);
			temp = true;
			toFlag = false;
		}
	}
	if (toFlag)
	{
		SetCurNewRow(false);
		SetCurModify(false);
	}
	return temp;
}

bool DBDataSet::New(const std::unordered_map<std::string, std::string>& codition)
{
	return DBSqlManager::getSingletonPtr()->QueryRecord(codition, *this);
}

bool DBDataSet::New(int keyData)
{
	std::string dataStr = fu_int_to_str(keyData);
	return New(dataStr);
}

bool DBDataSet::New(long long keyData)
{
	std::string dataStr = fu_longlong_to_str(keyData);
	return New(dataStr);
}

bool DBDataSet::NewProcedure(const std::string& procName, DBProcData* paramData)
{
	return DBSqlManager::getSingletonPtr()->ExcuteProc(procName, paramData, *this);
}

void DBDataSet::Commit()
{
	bool rowloop = MoveBeginRow();
	while (rowloop)
	{
		bool temp = false;
		if (IsNewRow())
		{
			temp = DBSqlManager::getSingletonPtr()->CommitInsert(*this);
		}
		else
		{
			temp = DBSqlManager::getSingletonPtr()->CommitModify(*this);
		}
		if (temp)
		{
			SetCurNewRow(false);
			SetCurModify(false);
		}
		rowloop = MoveNextRow();
	}
}

void DBDataSet::printAll()
{
	printf("--1--printAll----\n");
	for (auto& row : m_SetList)
	{
		printf("{");
		for (auto& field : row.m_RowList)
		{
			printf("%s=%s, ", field.first.c_str(), field.second.Data().c_str());
		}
		printf("}\n");
	}
	printf("--2--printAll----\n");
}
//////////////////////////////////////////////////////////////////////////
DBDataField& DBDataSet::operator[](const std::string& fieldName)
{
	if (m_SetList.empty())
	{
		m_SetList.push_back(st_ZeroDataRow);
		m_SetIndex = 0;
		m_SetList[m_SetIndex].SetIsInsert(true);
	}
	return m_SetList[m_SetIndex].At(fieldName);
}

DBDataField& DBDataSet::CurKeyField()
{
	return m_SetList[m_SetIndex].At(m_sKeyName);
}

bool DBDataSet::MoveNextRow()
{
	if (m_SetIndex < (int)m_SetList.size()-1)
	{
		m_SetIndex++;
		return true;
	}
	return false;
}

bool DBDataSet::MoveBeginRow()
{
	if (m_SetList.empty()) return false;
	m_SetIndex = 0;
	return true;
}

bool DBDataSet::MoveNextField()
{
	if (m_RowIterator != m_SetList[m_SetIndex].m_RowList.end())
	{
		m_RowIterator++;
		if (m_RowIterator != m_SetList[m_SetIndex].m_RowList.end())
		{
			return true;
		}
	}
	return false;
}

bool DBDataSet::MoveBeginField()
{
	if (m_SetList[m_SetIndex].m_RowList.empty())
	{
		return false;
	}
	m_RowIterator = m_SetList[m_SetIndex].m_RowList.begin();
	return true;
}

DBDataField& DBDataSet::CurField()
{
	return m_RowIterator->second;
}

const std::string& DBDataSet::CurFieldName()
{
	return m_RowIterator->first;
}

void DBDataSet::Clear()
{
	m_SetList.clear();
}

bool DBDataSet::Empty()
{
	return m_SetList.empty();
}

int DBDataSet::RowSize()
{
	int totallen = 0;
	for (auto& field : m_SetList[m_SetIndex].m_RowList)
	{
		totallen += field.second.Size();
		totallen += field.first.length();
	}
	return totallen;
}

bool DBDataSet::IsNewRow()
{
	return m_SetList[m_SetIndex].isInsert();
}

void DBDataSet::SetCurNewRow(bool flag)
{
	m_SetList[m_SetIndex].SetIsInsert(flag);
}

void DBDataSet::SetCurModify(bool flag)
{
	m_SetList[m_SetIndex].SetModify(flag);
}

