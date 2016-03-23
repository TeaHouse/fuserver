#ifndef __FUDATABASE_DBSTRUCT_H__
#define __FUDATABASE_DBSTRUCT_H__

#include "DBBase.h"

class DBField { //���ݽڵ�
public:
	DBField(unsigned short iType=0);
	~DBField();
	void Serialize(char* buf, int& size);
	void unSerialize(const char* buf, int& size);
	int  SerializeSize();
	void Print();
	int  CompareWith(DBField* pField); //�Ƚϴ�С�����ڷ���1��С�ڷ���-1�����ڷ���0
	
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
	std::string		m_sData;	//���ݣ����ַ�����ȡ
	unsigned short	m_usType;		//��������
	unsigned short 	m_usIndex;		//�±�
	bool			m_bKey;		//�Ƿ�������
	bool			m_bNull;		//�Ƿ�Ϊ��,data�Ƿ��п�������
};

#endif