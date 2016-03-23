#ifndef __FU_UTILS_LOG_H__
#define __FU_UTILS_LOG_H__

#include "../Platform/Include.h"

class UtilsLog;
#define FULOG(format, ...)			UtilsLog::Log(1, format, ##__VA_ARGS__)
#define FUWarning(format, ...)		UtilsLog::Log(2, format, ##__VA_ARGS__)
#define FUError(format, ...)		UtilsLog::Log(3, format, ##__VA_ARGS__)
#define FUFileLog(pstr, file)		{UtilsLog::LogFile(pstr, file);}

#ifdef _DEBUG
#define FUAssert(flag, str)			{assert(flag);}//若flag为false，则报错
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

	/*pstr是log字符串， level表示log等级，0和1表示普通打印，2表示warning，3表示error*/
	static void Log(int level, const char* format, ...);
	static void LogFile(const std::string& pstr, const std::string& fileName);

	static void SetWriteFile(bool flag);
};







#endif