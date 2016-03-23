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
	void PushParam(long* param, bool bInput = true);	//按堆栈形式压入，第一个入栈表示第一个参数，第二个表示第二个参数
	void PushParam(short* param, bool bInput = true);
	void PushParam(float* param, bool bInput = true);
	void PushParam(double* param, bool bInput = true);
	void PushParam(char* param, int len, bool bInput = true);
	void PushDefaultParam();	//压入默认参数，默认参数写在了存储过程里面



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