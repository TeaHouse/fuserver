#ifndef __FU_SERVER_DB_SQL_PROCDATA_H__
#define __FU_SERVER_DB_SQL_PROCDATA_H__

#include "../Platform/Include.h"

class DBProcData
{
public:
	DBProcData();
	~DBProcData();

	char* Excute(char* phdbc, const char* procName);
	void ClearParam();

public:
	void PushParam(long* param, bool bInput = true);	//����ջ��ʽѹ�룬��һ����ջ��ʾ��һ���������ڶ�����ʾ�ڶ�������
	void PushParam(short* param, bool bInput = true);
	void PushParam(float* param, bool bInput = true);
	void PushParam(double* param, bool bInput = true);
	void PushParam(char* param, int len, bool bInput = true);
	void PushDefaultParam();	//ѹ��Ĭ�ϲ�����Ĭ�ϲ���д���˴洢��������



	void SetReturnData(long* param);
	void SetReturnData(short* param);
	void SetReturnData(float* param);
	void SetReturnData(double* param);
	void SetReturnData(char* param, int len);

private:
	std::vector<char*>		m_InputParamList;
	char*	m_pReturnData;
};


#endif