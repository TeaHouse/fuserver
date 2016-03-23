#ifndef __FUDATABASE_DBTABLE_H__
#define __FUDATABASE_DBTABLE_H__

#include "DBRecord.h"
#include "../Utils/UtilsLock.h"

class DBTable
{
public:
	DBTable(const std::vector<std::string>& nameList, const std::vector<unsigned short>& typeList, int iKeyIndex);
	~DBTable();

	void BackupTableCmd(std::unordered_map<DBRecord*, std::string>& cmdList);

	DBRecord*  InsertRecord(std::string dataKey);//��ΪĬ��ֵ����������ģʽ����,���۱���Ƿ�����
	DBRecord*  InsertRecord(DBRecord* pRecord);
	DBRecord*  InsertRecord(const std::unordered_map<std::string, std::string>& dataList);//����һ�е����ƺ�����pair
	DBRecord*  InsertRecord(const std::unordered_map<int, std::string>& dataList);//����һ����¼���±�����
	DBRecord*  InsertRecord(const std::vector<std::string>& dataList);

	bool  QueryRecord(const std::string& fieldName,
		const std::string& dataEquKey,
		const std::string& dataMinkey,
		const std::string& dataMaxkey,
		std::vector<std::vector<std::string> >& recordList);
	DBRecord*  ModifyRecord(DBRecord* pRecord); //ɾ���ɵģ������µ�pRecord  
	DBRecord*  ModifyRecord(const std::string& dataKey, const std::string& fieldName, const std::string& sData);
	DBRecord*  ModifyRecord(const std::string& dataKey, int index, const std::string& sData);
	DBRecord*  ModifyRecord(const std::string& dataKey, const std::unordered_map<std::string, std::string>& dataList);

	void  ClearnRecord();
	std::string CheckInsertSqlCmd(DBRecord* pRecord);
	std::string CheckModifySqlCmd(DBRecord* pRecord);
public:
	long long  GetRecordSize()			{ return m_RecordSet.size(); }
	long long  GetTableMemSize();	//��ȡ���������ռ���ڴ�
	std::string CreateDataKey(char* pData, int dataLen); //����keyֵ
public:
	const std::string& GetFieldName(int index)	{ return m_FieldNameList[index]; }
	void GetFieldName(std::vector<std::string>& nameList);
	int  GetFieldIndex(const std::string& fieldName);
	int  GetFieldSize()		{ return m_FieldNameList.size(); }
	unsigned short GetFieldType(int index);

	int  CompareDataKey(const std::string& data1, const std::string& data2, unsigned short iType);

	CLASS_GET_OR_SET_PRIVATE(m_sTableName, std::string, TableName)
	CLASS_GET_OR_SET_PRIVATE(m_sDBName, std::string, DBName)
	CLASS_GET_OR_SET_PRIVATE(m_iPriKeyIndex, int, PriKeyIndex)//�����±�,���Ǵ�0��ʼ
	CLASS_GET_OR_SET_PRIVATE(m_fBackupTime, float, BackupTime)
	CLASS_GET_OR_SET_PRIVATE(m_bAutoIncrement, bool, IsAutoIncrement)
private:
	std::string  CreateIncrementKey();
	void CheckIncrementField(const std::string& dataKey);
	std::string PriKeyIncrement();

	DBRecord* InsertSet(const std::string& dataKey, DBRecord* pRecord);
	DBRecord* RemoveSet(const std::string& dataKey);
	DBRecord* FindSet(const std::string& dataKey);

private:
	std::unordered_map<std::string, DBRecord*>	m_RecordSet; //��¼��
	UtilsLock		m_Lock;	// ��

private:
	std::unordered_map<std::string, int>	m_FieldIndexList;
	std::vector<std::string>		m_FieldNameList;
	std::vector<unsigned short>		m_FieldTypeList;
	std::string		m_sMinKey; //�����Сֵ����������
	std::string		m_sMaxKey;
};


#endif