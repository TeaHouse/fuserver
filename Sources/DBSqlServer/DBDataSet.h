#ifndef __FU_SERVER_DB_SQL_DATA_SET_H__
#define __FU_SERVER_DB_SQL_DATA_SET_H__

#include "DBDataRow.h"
#include "DBProcData.h"
#include "../Utils/UtilsFunction.h"

class DBDataSet
{
public:
	DBDataSet(const std::string& tname); 
	~DBDataSet();
	DBDataSet(const DBDataSet& set);
	DBDataSet& operator=(const DBDataSet& set);
public:
	void PushRow(const std::unordered_map<std::string, DBDataField>& row); //����һ����¼���ҶԷǿջ����Ȥ�ĸ�ֵ
public:
	bool New(int keyData);
	bool New(long long keyData);
	bool New(const std::string& keyData); //������������û��������һ����¼����ֻ�Ƕ�������ֵ, ��Ϊ����Լ���������������
	bool New(const std::unordered_map<std::string, std::string>& codition); //ͨ��������ѯ
	bool NewProcedure(const std::string& procName, DBProcData* paramData);
	void Commit();//�ύ�޸Ļ�����
	void printAll();
public:
	/*��ȡ��ǰ������ָ��ļ�¼��ĳ���ֶε�ֵ������
	 * �������ڼ�¼�У�������һ���ֶ�
	 */
	DBDataField& operator[](const std::string& fieldName);	
	DBDataField& CurKeyField();
	DBDataField& CurField();
	const std::string& CurFieldName();
	bool MoveNextRow();	//�ƶ�������
	bool MoveBeginRow();
	//------------------------------------
	bool MoveNextField();//�ƶ�������
	bool MoveBeginField();

	int  RowSize();
	bool IsNewRow();	//�Ƿ��������ӵļ�¼
	void SetCurNewRow(bool flag);
	void SetCurModify(bool flag);

	bool Empty();	//�Ƿ�Ϊ�գ��շ���true
	void Clear();
public:
	CLASS_GET_OR_SET_PROTECTED(m_sTableName, std::string, TableName)
	CLASS_GET_OR_SET_PROTECTED(m_sKeyName, std::string, KeyName)
	CLASS_GET_OR_SET_PROTECTED(m_iColumnNum, int, ColumnNum)
private:	
	typedef std::unordered_map<std::string, DBDataField>	DB_ROW_TYPE;
	DB_ROW_TYPE::iterator		m_RowIterator;
	std::vector<DBDataRow>		m_SetList; //�����
	int 	m_SetIndex;
	bool	m_bIdentity;
};



#endif