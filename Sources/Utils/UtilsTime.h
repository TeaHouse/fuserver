#ifndef __FU_UTILS_TIME_H__
#define __FU_UTILS_TIME_H__

#include "../Platform/Include.h"

typedef struct 
{
	unsigned int   year;	//�갴��ǰ��
	unsigned char  month;	//��1~12
	unsigned char  day;	//��1~31
	unsigned char  hour; //ʱ0~23��û��24ʱ
	unsigned char  minute; //��0~59
	unsigned char  second;	//��,������ΧΪ0-59����������61��
	unsigned char  weekday;	//�ܼ� 1~7
}fu_daytime_t;

class UtilsTime
{
public:
	UtilsTime();
	~UtilsTime();

	static void SleepUsec(unsigned long msTick);	//��˯���ٺ���
	static void SleepSec(float second);	//��˯������

public:
	static fu_daytime_t CurDayTime();	//��ǰ�����ա�ʱ����;
	static float CurSecondTime();	//��
	static long long CurMsecTime();	//����
};






#endif