#ifndef __FU_DATABASE_PROTOCOL_H__
#define __FU_DATABASE_PROTOCOL_H__

#include "../Utils/UtilsSerialize.h"

//������
#define ERROR_CODE_LOADDB		100	//���ݿ����ʧ��,���ӱ�ʧ��
#define ERROR_CODE_QUERY		101	//��ѯʧ�ܣ�û�и�����
#define ERROR_CODE_MODIFY		102	//�޸ļ�¼ʧ��
#define ERROR_CODE_INSERT		103	//�����¼ʧ��
#define ERROR_CODE_PROT			104	//Э�����
#define ERROR_CODE_NOTABLE		105	//�����ڸñ�

//Э����
#define PROTOCOL_DB_S_TO_C_ERROR    100	//�������

#define PROTOCOL_DB_S_TO_C_QUERY	101 //��ѯ, �ӷ��������ͻ���
#define PROTOCOL_DB_S_TO_C_MODIFY	102 //�޸�
#define PROTOCOL_DB_S_TO_C_INSERT	103 //����
#define PROTOCOL_DB_S_TO_C_LOADTABLE	104 //���ر��


#define PROTOCOL_DB_C_TO_S_QUERY	201 //��ѯ, �ӿͻ��˵�������
#define PROTOCOL_DB_C_TO_S_MODIFY	202 //�޸�
#define PROTOCOL_DB_C_TO_S_INSERT	203 //����
#define PROTOCOL_DB_C_TO_S_LOADTABLE		104	//���ر��

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
//���ر�
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
struct db_sc_query_t //��ѯ�ṹ��
{
	int	protId;
	std::vector<std::string>	fieldNameList;
	std::vector<std::vector<std::string> >		recordList;
};
struct db_cs_query_t
{
	int	protId;
	std::string		tname;	//������
	std::string		fieldName;	//��������
	std::string		dataEquKey;	//��������������Ϊ����ȡ���ڵļ�¼��������ƥ��
	std::string		dataMinKey;	//��С����������Ϊ����ȡ���ڵ��ڵļ�¼�����ƥ��
	std::string		dataMaxKey;	//�������������Ϊ����ȡС����ڵļ�¼�����ƥ��
};
void unserialize_prot_sc_query(db_sc_query_t& dStruct, UtilsSerialize& dSerialize); //���л�
void unserialize_prot_cs_query(db_cs_query_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_sc_query(db_sc_query_t& dStruct, UtilsSerialize& dSerialize); //�����л�
void serialize_prot_cs_query(db_cs_query_t& dStruct, UtilsSerialize& dSerialize);
//------------------------------------------
struct db_sc_modify_t //�޸�Э��ṹ��
{
	int	protId;
};
struct db_cs_modify_t  
{
	int	protId;
	std::string		tname;	//������
	std::string		dataKey;	//����ֵ,������ͨ���������޸������ֶ�
	std::unordered_map<std::string, std::string>	dataList;
};
void unserialize_prot_sc_modify(db_sc_modify_t& dStruct, UtilsSerialize& dSerialize); //���л�
void unserialize_prot_cs_modify(db_cs_modify_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_sc_modify(db_sc_modify_t& dStruct, UtilsSerialize& dSerialize); //�����л�
void serialize_prot_cs_modify(db_cs_modify_t& dStruct, UtilsSerialize& dSerialize);
//------------------------------------------
struct db_sc_insert_t	//����Э��ṹ��
{
	int	protId;
	std::string		dataKey;	//��������ֵ
};
struct db_cs_insert_t
{
	int	protId;
	std::string		tname;
	std::unordered_map<std::string, std::string>	dataList;
};
void unserialize_prot_sc_insert(db_sc_insert_t& dStruct, UtilsSerialize& dSerialize); //���л�
void unserialize_prot_cs_insert(db_cs_insert_t& dStruct, UtilsSerialize& dSerialize);
void serialize_prot_sc_insert(db_sc_insert_t& dStruct, UtilsSerialize& dSerialize); //�����л�
void serialize_prot_cs_insert(db_cs_insert_t& dStruct, UtilsSerialize& dSerialize);
//------------------------------------------


#endif