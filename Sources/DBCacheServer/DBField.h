#ifndef __FUDATABASE_DBSTRUCT_H__
#define __FUDATABASE_DBSTRUCT_H__

#include "DBBase.h"

class DBField { //数据节点
public:
	DBField(unsigned short iType=0);
	~DBField();
	void Serialize(char* buf, int& size);
	void unSerialize(const char* buf, int& size);
	int  SerializeSize();
	void Print();
	int  CompareWith(DBField* pField); //比较大小，大于返回1，小于返回-1，等于返回0
	
public:
	DBField* Clone();
	void ResetData(DBField* pField);
	void ResetData(const std::string& sData);
public:
	void SetIsKey(bool flag)				{ m_bKey = flag; }
	void SetIndex(unsigned short index)		{ m_usIndex = index; }
	void SetData(const std::string& sData);
	void SetData(const char* pData, int len);
	void SetDataType(unsigned short iType)	{ m_usType = iType; }

	const std::string& GetData()		{ return m_sData; }
	unsigned short GetDataType()		{ return m_usType; }
	unsigned short GetIndex()			{ return m_usIndex; }
	bool IsPriKey()		{ return m_bKey; }
	bool IsNull()		{ return m_bNull; }
private:
	std::string		m_sData;	//数据，用字符串存取
	unsigned short	m_usType;		//数据类型
	unsigned short 	m_usIndex;		//下标
	bool			m_bKey;		//是否是主键
	bool			m_bNull;		//是否为空,data是否有可用数据
};

#endif