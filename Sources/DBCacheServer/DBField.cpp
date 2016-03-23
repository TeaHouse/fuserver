#include "DBField.h"

DBField::DBField(unsigned short iType/*=0*/)
: m_usType(iType)
, m_sData("")
, m_usIndex(0)
, m_bKey(false)
, m_bNull(true)
{

}

DBField::~DBField()
{

}

void DBField::Serialize(char* buf, int& size)
{
	int dataLen = (int)m_sData.length();
	if (m_bNull) dataLen = 0;

	memcpy(buf + size, &(dataLen), sizeof(int));
	size += sizeof(int);
	memcpy(buf + size, &m_usType, sizeof(unsigned short));
	size += sizeof(unsigned short);
	memcpy(buf + size, &m_usIndex, sizeof(unsigned short));
	size += sizeof(unsigned short);
	memcpy(buf + size, &m_bKey, sizeof(bool));
	size += sizeof(bool);
	memcpy(buf + size, &m_bNull, sizeof(bool));
	size += sizeof(bool);

	if (!m_bNull){
		memcpy(buf + size, m_sData.c_str(), dataLen);
		size += dataLen;
	}
}

void DBField::unSerialize(const char* buf, int& size)
{
	int dataLen = *(int*)(buf + size);
	size += sizeof(int);
	this->m_usType = *(unsigned short*)(buf + size);
	size += sizeof(unsigned short);
	this->m_usIndex = *(unsigned short*)(buf + size);
	size += sizeof(unsigned short);
	this->m_bKey = *(bool*)(buf + size);
	size += sizeof(bool);
	this->m_bNull = *(bool*)(buf + size);
	size += sizeof(bool);

	if (!this->m_bNull){
		m_sData.assign(buf + size, dataLen);
		size += dataLen;
	}
}

int DBField::SerializeSize()
{
	int dataSize = (int)m_sData.length();
	if (m_bNull) dataSize = 0;
	dataSize += sizeof(int);
	dataSize += sizeof(unsigned short);
	dataSize += sizeof(unsigned short);
	dataSize += sizeof(bool);
	dataSize += sizeof(bool);
	return dataSize;
}

int DBField::CompareWith(DBField* pField)
{
	if (this->m_usIndex != pField->m_usIndex ||
		this->m_usType != pField->m_usType ||
		this->m_bKey != pField->m_bKey){
		return -1;
	}
	if (this->m_bNull != pField->m_bNull)
	{
		if (!this->m_bNull)
			return -1;
		else if (pField->m_bNull)
			return 1;
	}

	return DB_CompareDataKey(pField->m_sData, pField->m_sData, m_usType);
}

//////////////////////////////////////////////////////////////////////////
DBField* DBField::Clone()
{
	DBField* pField = new DBField();
	pField->m_bKey = this->m_bKey;
	pField->m_bNull = this->m_bNull;
	pField->m_usIndex = this->m_usIndex;
	pField->m_usType = this->m_usType;
	if (!this->m_bNull)
	{
		pField->m_sData.assign(this->m_sData);
	}
	return pField;
}

void DBField::ResetData(DBField* pField)
{
	m_bKey = pField->m_bKey;
	m_bNull = pField->m_bNull;
	m_usIndex = pField->m_usIndex;
	m_usType = pField->m_usType;
	m_sData = "";
	if (!m_bNull)
	{
		m_sData.assign(pField->m_sData);
	}
}

void DBField::ResetData(const std::string& sData)
{
	m_bNull = false;
	m_sData.assign(sData);
	if (m_sData=="")
	{
		m_bNull = true;
	}
}

void DBField::SetData(const std::string& sData)
{
	m_sData = sData;
	m_bNull = false;
	if (m_sData=="")
	{
		m_bNull = true;
	}
}

void DBField::SetData(const char* pData, int len)
{
	m_sData = "";
	m_bNull = true;
	if (pData && len>0)
	{
		m_sData.assign(pData, len);
		m_bNull = false;
	}
}

void DBField::Print()
{
	printf("+node=[%p]:\n    -type=[%u]\n    -length=[%d]\n    -index=[%u]\n    -iskey=[%d]\n    -isnull=[%s]\n    -",
		this, this->m_usType, this->m_sData.length(), this->m_usIndex, this->m_bKey, (this->m_bNull ? "true" : "false"));
	if (this->m_bNull)
	{
		printf("data=[NULL]\n");
		return;
	}
	printf("data=[%s]\n", m_sData.c_str());
}
