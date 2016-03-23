#include "UtilsTime.h"
#include "UtilsSerialize.h"
#include "UtilsConvert.h"

int st_getSysTime(struct timeval * val, struct timezone *tz)
{
	if (val)
	{
#ifdef WIN32
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency(&liFreq);
		QueryPerformanceCounter(&liTime);
		val->tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
		val->tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0);
#else
		return gettimeofday(val, tz);
#endif
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
UtilsTime::UtilsTime()
{

}

UtilsTime::~UtilsTime()
{

}

void UtilsTime::SleepUsec(unsigned long msTick)
{
#ifdef WIN32
	::Sleep(msTick);
#else
	::usleep(msTick * 1000)
#endif
}

void UtilsTime::SleepSec(float second)
{
#ifdef WIN32
	::Sleep(DWORD(second * 1000));
#else
	::sleep(second);
#endif
}

float UtilsTime::CurSecondTime()
{
	struct timeval nowTime;
	st_getSysTime(&nowTime, NULL);
	return nowTime.tv_sec + nowTime.tv_usec / 1000000.0f;
}

long long UtilsTime::CurMsecTime()
{
	struct timeval nowTime;
	st_getSysTime(&nowTime, NULL);
	return nowTime.tv_sec * 1000 + nowTime.tv_usec/1000;
}

fu_daytime_t UtilsTime::CurDayTime()
{
#ifdef WIN32
	time_t rawtime;
	::time(&rawtime);
	struct tm* ptm = ::localtime(&rawtime);
#else
	time_t rawtime;
	::time(&rawtime);
	struct tm* pTm = ::localtime(&rawtime);
#endif
	static fu_daytime_t	curDayTime;

	curDayTime.year = ptm->tm_year + 1900;
	curDayTime.month = ptm->tm_mon + 1;
	curDayTime.day = ptm->tm_mday;

	curDayTime.hour = ptm->tm_hour;
	curDayTime.minute = ptm->tm_min;
	curDayTime.second = ptm->tm_sec;

	curDayTime.weekday = ptm->tm_wday;
	if (curDayTime.weekday==0)
	{
		curDayTime.weekday = 7;
	}
	return curDayTime;
}


