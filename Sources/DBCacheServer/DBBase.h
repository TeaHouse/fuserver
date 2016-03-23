#ifndef __FUDATABASE_DBBASE_H__
#define __FUDATABASE_DBBASE_H__
#include "../Platform/Include.h"

#define		db_int_t			1	//int
#define		db_byte_t 			2	//char
#define		db_string_t 		3	//char*
#define		db_bool_t			4	//bool
#define		db_float_t			5	//float
#define		db_double_t			6	//double
#define		db_usint_t			7	//unsigned int
#define		db_long_t			8	//long 
#define		db_longlong_t		9	//long long

#define		DB_BACKUP_LIMITE_TIME	10//(600)	//数据库备份时间，秒

//比较两个字段之间的大小,依据以上类型
int DB_CompareDataKey(const std::string& data1, const std::string& data2, unsigned short iType);

#endif