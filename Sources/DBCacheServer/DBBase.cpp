#include "DBBase.h"
#include "../Utils/UtilsConvert.h"


int DB_CompareDataKey(const std::string& data1, const std::string& data2, unsigned short iType)
{

	switch (iType)
	{
		case db_int_t:				//int
		case db_byte_t: 				//char
		case db_usint_t:				//unsigned int
		case db_long_t:				//long 
		case db_longlong_t:			//long long
		{
			long long thisData = fu_str_to_integer(data1.c_str(), data1.length());
			long long otherData = fu_str_to_integer(data2.c_str(), data2.length());
			if (thisData == otherData) return 0;
			if (thisData > otherData) return 1;
			return -1;
		}
		case db_float_t:				//float
		case db_double_t:				//double
		{
			double thisData = fu_str_to_double(data1.c_str(), data1.length());
			double otherData = fu_str_to_double(data2.c_str(), data2.length());
			if (fabs(thisData - otherData) < 0.00001f) return 0;
			if (thisData > otherData) return 1;
			return -1;
		}
		case db_string_t: 			//char*
		case db_bool_t:				//bool
			return data1.compare(data2);
		default:
			break;
	}
	return data1.compare(data2);
}

