#ifndef __FU_UTILS_LOG_H__
#define __FU_UTILS_LOG_H__

#include "../Platform/Include.h"

class UtilsLog;
#define FULOG(format, ...)			UtilsLog::Log(1, format, ##__VA_ARGS__)
#define FUWarning(format, ...)		UtilsLog::Log(2, format, ##__VA_ARGS__)
#define FUError(format, ...)		UtilsLog::Log(3, format, ##__VA_ARGS__)
#define FUFileLog(pstr, file)		{UtilsLog::LogFile(pstr, file);}

#ifdef _DEBUG
#define FUAssert(flag, str)			{assert(flag);}//��flagΪfalse���򱨴�
#else
#define FUAssert(flag, str)
#endif

class UtilsLog
{
public:
	enum 
	{
		ONEFILE_MAXSIZE = 1024*1024*4,	//4M
	};
	static bool Init();
	static void Exit();

	/*pstr��log�ַ����� level��ʾlog�ȼ���0��1��ʾ��ͨ��ӡ��2��ʾwarning��3��ʾerror*/
	static void Log(int level, const char* format, ...);
	static void LogFile(const std::string& pstr, const std::string& fileName);

	static void SetWriteFile(bool flag);
};







#endif