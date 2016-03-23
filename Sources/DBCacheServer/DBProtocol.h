#ifndef __FU_DATABASE_PROTOCOL_H__
#define __FU_DATABASE_PROTOCOL_H__

#include "../Utils/UtilsSerialize.h"

//错误码
#define ERROR_CODE_LOADDB		100	//数据库加载失败,增加表失败
#define ERROR_CODE_QUERY		101	//查询失败，没有该数据
#define ERROR_CODE_MODIFY		102	//修改记录失败
#define ERROR_CODE_INSERT		103	//插入记录失败
#define ERROR_CODE_PROT			104	//协议错误
#define ERROR_CODE_NOTABLE		105	//不存在该表

//协议码
#define PROTOCOL_DB_S_TO_C_ERROR    100	//错误代码

#define PROTOCOL_DB_S_TO_C_QUERY	101 //查询, 从服务器到客户端
#define PROTOCOL_DB_S_TO_C_MODIFY	102 //修改
#define PROTOCOL_DB_S_TO_C_INSERT	103 //新增
#define PROTOCOL_DB_S_TO_C_LOADTABLE	104 //加载表格


#define PROTOCOL_DB_C_TO_S_QUERY	201 //查询, 从客户端到服务器
#define PROTOCOL_DB_C_TO_S_MODIFY	202 //修改
#define PROTOCOL_DB_C_TO_S_INSERT	203 //新增
#define PROTOCOL_DB_C_TO_S_LOADTABLE		104	//加载表格

int  unserialize_db_protid(UtilsSerialize& dSerialize);
void serialize_db_protid(int protId, UtilsSerialize& dSerialize);

struct db_prot_error_t 
{
	int	protId;
	int code;
};
void unserialize_prot_error(db_prot_error_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_error(db_prot_error_t& dStruct, UtilsSerialize& dSerialize);
//------------------------------------------
//加载表
struct db_sc_loadtable_t
{
	int protId;
};
struct db_cs_loadtable_t
{
	int protId;
	std::string		dbname;
	std::string		tname;
};
void unserialize_prot_sc_loadtable(db_sc_loadtable_t& dStruct, UtilsSerialize& dSerialize);
void unserialize_prot_cs_loadtable(db_cs_loadtable_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_sc_loadtable(db_sc_loadtable_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_cs_loadtable(db_cs_loadtable_t& dStruct, UtilsSerialize& dSerialize);
//------------------------------------------
struct db_sc_query_t //查询结构体
{
	int	protId;
	std::vector<std::string>	fieldNameList;
	std::vector<std::vector<std::string> >		recordList;
};
struct db_cs_query_t
{
	int	protId;
	std::string		tname;	//表名称
	std::string		fieldName;	//主键名称
	std::string		dataEquKey;	//等于主键，若不为空则取等于的记录，最优先匹配
	std::string		dataMinKey;	//最小主键，若不为空则取大于等于的记录，其次匹配
	std::string		dataMaxKey;	//最大主键，若不为空则取小鱼等于的记录，最次匹配
};
void unserialize_prot_sc_query(db_sc_query_t& dStruct, UtilsSerialize& dSerialize); //序列化
void unserialize_prot_cs_query(db_cs_query_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_sc_query(db_sc_query_t& dStruct, UtilsSerialize& dSerialize); //反序列化
void serialize_prot_cs_query(db_cs_query_t& dStruct, UtilsSerialize& dSerialize);
//------------------------------------------
struct db_sc_modify_t //修改协议结构体
{
	int	protId;
};
struct db_cs_modify_t  
{
	int	protId;
	std::string		tname;	//表名称
	std::string		dataKey;	//主键值,必须是通过主键来修改其他字段
	std::unordered_map<std::string, std::string>	dataList;
};
void unserialize_prot_sc_modify(db_sc_modify_t& dStruct, UtilsSerialize& dSerialize); //序列化
void unserialize_prot_cs_modify(db_cs_modify_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_sc_modify(db_sc_modify_t& dStruct, UtilsSerialize& dSerialize); //反序列化
void serialize_prot_cs_modify(db_cs_modify_t& dStruct, UtilsSerialize& dSerialize);
//------------------------------------------
struct db_sc_insert_t	//新增协议结构体
{
	int	protId;
	std::string		dataKey;	//返回主键值
};
struct db_cs_insert_t
{
	int	protId;
	std::string		tname;
	std::unordered_map<std::string, std::string>	dataList;
};
void unserialize_prot_sc_insert(db_sc_insert_t& dStruct, UtilsSerialize& dSerialize); //序列化
void unserialize_prot_cs_insert(db_cs_insert_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_sc_insert(db_sc_insert_t& dStruct, UtilsSerialize& dSerialize); //反序列化
void serialize_prot_cs_insert(db_cs_insert_t& dStruct, UtilsSerialize& dSerialize);
//------------------------------------------


#endif