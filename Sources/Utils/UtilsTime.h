#ifndef __FU_UTILS_TIME_H__
#define __FU_UTILS_TIME_H__

#include "../Platform/Include.h"

typedef struct 
{
	unsigned int   year;	//年按当前年
	unsigned char  month;	//月1~12
	unsigned char  day;	//日1~31
	unsigned char  hour; //时0~23，没有24时
	unsigned char  minute; //分0~59
	unsigned char  second;	//秒,正常范围为0-59，但允许至61秒
	unsigned char  weekday;	//周几 1~7
}fu_daytime_t;

class UtilsTime
{
public:
	UtilsTime();
	~UtilsTime();

	static void SleepUsec(unsigned long msTick);	//沉睡多少毫秒
	static void SleepSec(float second);	//沉睡多少秒

public:
	static fu_daytime_t CurDayTime();	//当前年月日、时分秒;
	static float CurSecondTime();	//秒
	static long long CurMsecTime();	//毫秒
};






#endif