
#include "DBDataRow.h"
#include "../Utils/UtilsConvert.h"

DBDataField::DBDataField()
	: m_sData("")
{
	m_bModify = false;
}

DBDataField::DBDataField(const DBDataField& field)
{
	operator=(field);
}

DBDataField::DBDataField(const std::string& data)
	:m_sData(data)
{
	m_bModify = false;
}

DBDataField& DBDataField::operator=(const DBDataField& field)
{
	m_sData = field.m_sData;
	m_bModify = field.m_bModify;
	return *this;
}

void DBDataField::operator=(int data)
{
	operator=(fu_int_to_str(data));
}

void DBDataField::operator=(long long data)
{
	operator=(fu_longlong_to_str(data));
}

void DBDataField::operator=(double data)
{
	operator=(fu_double_to_str(data));
}

void DBDataField::operator=(float data)
{
	operator=(fu_float_to_str(data));
}


void DBDataField::operator=(const std::string& data)
{
	if (!m_bModify && m_sData != data)
	{
		m_bModify = true;
	}
	m_sData = data;
}

int DBDataField::Size()
{
	return m_sData.length();
}

bool DBDataField::isModify()
{
	return m_bModify;
}

void DBDataField::SetModify(bool flag)
{
	m_bModify = flag;
}

const std::string& DBDataField::Data()
{
	return m_sData;
}
//////////////////////////////////////////////////////////////////////////

DBDataRow::DBDataRow()
	: m_bInsert(false)
{
	m_RowList.clear();
}

DBDataRow::DBDataRow(const DBDataRow& row)
{
	operator=(row);
}

DBDataRow::DBDataRow(const std::unordered_map<std::string, DBDataField>& rowlist)
{
	m_RowList = rowlist;
	m_bInsert = false;
}

DBDataRow& DBDataRow::operator=(const DBDataRow& row)
{
	m_RowList = row.m_RowList;
	m_bInsert = row.m_bInsert;
	return *this;
}

DBDataRow::~DBDataRow()
{
	m_RowList.clear();
}

DBDataField& DBDataRow::operator[](const std::string& fname)
{
	auto iter = m_RowList.find(fname);
	if (iter != m_RowList.end())
	{
		return iter->second;
	}

	auto toIter = m_RowList.insert(std::make_pair(fname, DBDataField("")));
	return toIter.first->second;
}

DBDataField& DBDataRow::At(const std::string& fname)
{
	return operator[](fname);
}

void DBDataRow::SetIsInsert(bool flag)
{
	m_bInsert = flag;
}

void DBDataRow::SetModify(bool flag)
{
	for (auto iter : m_RowList)
	{
		iter.second.SetModify(flag);
	}
}

bool DBDataRow::isInsert()
{
	return m_bInsert;
}