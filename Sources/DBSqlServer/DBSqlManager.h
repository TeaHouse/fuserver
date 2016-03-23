#ifndef __FU_SERVER_DB_SQLMANAGER_H__
#define __FU_SERVER_DB_SQLMANAGER_H__

#include "DBSqlServer.h"
#include "../Utils/UtilsSingleton.h"

class DBSqlManager : public UtilsSingleton<DBSqlManager>
{
public:
	DBSqlManager();
	~DBSqlManager();

	bool Init();

	void Destroy();

public:
	bool QueryRecord(const std::string& tkeyData, DBDataSet& dataSet); //����ͨ��������ѯ,������Ϊ�����ѯ����
	bool QueryRecord(const std::unordered_map<std::string, std::string>& condition, DBDataSet& dataSet);//ͨ���ֶ�������ѯ
	bool NewRecord(DBDataSet& dataSet); //�²���һ��������¼

	/*tkeyName=""����ֻ��������Ҫ��ֵ�ļ�¼��
	tkeyData=""���������������������Եļ�¼
	tkeyName=""��tkeyData="",������ֶ�,�ұ������������������������ֶ�û�зǿ�����
	*/

	bool CommitInsert(DBDataSet& dataSet);
	bool CommitModify(DBDataSet& dataSet); //���޸ĺ�ļ�¼д�����ݿ�

	bool ExcuteProc(const std::string& procName, DBProcData* procData, DBDataSet& dataSet);

	void CheckTableAttribute(const std::string& tname, std::string& keyName, int& columnNum, bool& isIdentity);
	
private:
	DBSqlServer		m_hSqlServer;
};



#endif