#include "UtilsConvert.h"
#include <stdarg.h>

#define FU_kMaxStringLen	(1024*100)

long long fu_str_to_integer(const char* pData, int len)
{
	long long toInt = 0;
	for (int i = 0; i < len; i++)
	{
		//printf("----i=[%d]---char=[%d]--toInt=[%lld]-----\n", i, int(pData[i] - '0'), toInt);
		if (pData[i] >= '0' && pData[i] <= '9')
		{
			toInt = toInt * 10 + int(pData[i] - '0');
		}
		else if (toInt != 0 || pData[i] == '.')
		{
			break;
		}
	}
	//printf("---fu_str_to_int----[%lld]-----\n", toInt);
	return toInt;
}
double fu_str_to_double(const char* pData, int len)
{
	long double toDouble = 0;
	int index = 0;
	for (; index < len; index++)
	{
		//printf("--1--i=[%d]---char=[%d]--toDouble=[%lf]-----\n", index, int(pData[index] - '0'), toDouble);
		if (pData[index] == '.')
		{
			index++;
			break;
		}
		else if (pData[index] >= '0' && pData[index] <= '9')
		{
			toDouble = toDouble * 10 + int(pData[index] - '0');
		}
	}
	long double toLeftData = 0;
	for (int i = len-1; i >= index; i--)
	{
		if (pData[i] >= '0' && pData[i] <= '9')
		{
			toLeftData = toLeftData * 0.1f + int(pData[i] - '0') * 0.1f;
		}
		//printf("--2--i=[%d]---char=[%d]--toDouble=[%0.10llf]-----\n", i, int(pData[i] - '0'), toLeftData);
		//std::cout << std::setprecision(15) << toLeftData << std::endl;
	}
	toDouble += toLeftData;
	//printf("-------fu_str_to_double=[%0.10llf]-------\n", toDouble);
	return toDouble;
}
std::string fu_int_to_str(int toData)
{
	char pStrData[100] = {};
#ifdef WIN32
	sprintf_s(pStrData, "%d", toData);
#else
	sprintf(pStrData, "%d", toData);
#endif
	return std::string(pStrData);
}

std::string fu_ushort_to_str(unsigned short toData)
{
	char pStrData[100] = {};
#ifdef WIN32
	sprintf_s(pStrData, "%u", toData);
#else
	sprintf(pStrData, "%u", toData);
#endif
	return std::string(pStrData);
}

std::string fu_uint_to_str(unsigned int toData)
{
	char pStrData[100] = {};
#ifdef WIN32
	sprintf_s(pStrData, "%u", toData);
#else
	sprintf(pStrData, "%u", toData);
#endif
	return std::string(pStrData);
}

std::string fu_long_to_str(long toData)
{
	char pStrData[100] = {};
#ifdef WIN32
	sprintf_s(pStrData, "%ld", toData);
#else
	sprintf(pStrData, "%ld", toData);
#endif
	return std::string(pStrData);
}

std::string fu_longlong_to_str(long long toData)
{
	char pStrData[100] = {};
#ifdef WIN32
	sprintf_s(pStrData, "%lld", toData);
#else
	sprintf(pStrData, "%lld", toData);
#endif
	return std::string(pStrData);
}

std::string fu_float_to_str(float toData)
{
	char pStrData[100] = {};
#ifdef WIN32
	sprintf_s(pStrData, "%f", toData);
#else
	sprintf(pStrData, "%f", toData);
#endif
	return std::string(pStrData);
}

std::string fu_double_to_str(double toData)
{
	char pStrData[100] = {};
#ifdef WIN32
	sprintf_s(pStrData, "%lf", toData);
#else
	sprintf(pStrData, "%lf", toData);
#endif
	return std::string(pStrData);
}

std::string fu_ldouble_to_str(double toData)
{
	char pStrData[100] = {};
#ifdef WIN32
	sprintf_s(pStrData, "%llf", toData);
#else
	sprintf(pStrData, "%llf", toData);
#endif
	return std::string(pStrData);
}
//////////////////////////////////////////////////////////////////////////
std::string fu_str_increment(int addData, const char* pData, int len)
{
	std::string addKey = fu_int_to_str(addData);
	std::string toKey;
	toKey.assign(pData, len);
	int endIndex = toKey.size() - 1;
	if (pData[endIndex]<'0' || pData[endIndex]>'9' || len<=0)
	{
		toKey.append(addKey);
	}
	else
	{
		int addEndIndex = addKey.size()-1;
		int toKeyIndex = endIndex;
		int carry = 0; //进位
		for (int j = addEndIndex; j >= -1; j--, toKeyIndex--)
		{
			if (j == -1 && carry<=0)
			{
				break;
			}
			int toAddData = 0;
			if (toKeyIndex >= 0)
			{
				if (toKey[toKeyIndex] >= '0' && toKey[toKeyIndex] <= '9')
				{
					toAddData = toKey[toKeyIndex] - '0';
				}
				else
				{
					toKeyIndex++;
					toKey.insert(toKeyIndex, 1, '0');
				}
			}
			else
			{
				toKey.insert(0, 1, '0');
				toKeyIndex = 0;
			}
			if (j>=0)
			{
				toAddData += int(addKey[j] - '0') + carry;
			}
			else //最后一次循环用来查看carry
			{
				toAddData += carry;
			}
			toKey[toKeyIndex] = char(toAddData % 10 + '0');
			carry = toAddData / 10;
		}
	}
	return toKey;
}