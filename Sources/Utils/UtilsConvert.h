#ifndef __FU_UTILS_CONVERT_TYPE_H__
#define __FU_UTILS_CONVERT_TYPE_H__

#include "../Platform/Include.h"

//字符串转换成其他类型
long long fu_str_to_integer(const char* pData, int len);
double fu_str_to_double(const char* pData, int len);

//其他类型转换成字符串
std::string fu_ushort_to_str(unsigned short toData);
std::string fu_int_to_str(int toData);
std::string fu_uint_to_str(unsigned int toData);
std::string fu_long_to_str(long toData);
std::string fu_longlong_to_str(long long toData);
std::string fu_float_to_str(float toData);
std::string fu_double_to_str(double toData);
std::string fu_ldouble_to_str(double toData);

//字符串加整数
std::string fu_str_increment(int addData, const char* pData, int len);

#endif