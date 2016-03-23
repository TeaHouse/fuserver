#include "DBClient.h"
#include "../Utils/UtilsTime.h"
#include "../Utils/UtilsLog.h"

#define CHECK_WHILE_RETURN(ref, toProtId, code) \
while (1) { if (ref.protId == toProtId || code>0)return ref; else UtilsTime::SleepUsec(0); }
	

DBClient::DBClient()
	: m_iErrorCode(-1)
{

}

DBClient::~DBClient()
{
	m_iErrorCode = -1;
}

const db_sc_query_t& DBClient::SendQueryDB(db_cs_query_t& dStruct)
{
	UtilsSerialize dSerialize;
	serialize_prot_cs_query(dStruct, dSerialize);
	this->SendText(dSerialize.getData(), dSerialize.getLength());

	m_QueryRef.protId = -1;
	m_QueryRef.fieldNameList.clear();
	m_QueryRef.recordList.clear();
	m_iErrorCode = -1;
	CHECK_WHILE_RETURN(m_QueryRef, PROTOCOL_DB_S_TO_C_QUERY, m_iErrorCode);
}

const db_sc_modify_t& DBClient::SendModifyDB(db_cs_modify_t& dStruct)
{
	UtilsSerialize dSerialize;
	serialize_prot_cs_modify(dStruct, dSerialize);
	this->SendText(dSerialize.getData(), dSerialize.getLength());

	m_ModifyRef.protId = -1;
	CHECK_WHILE_RETURN(m_ModifyRef, PROTOCOL_DB_S_TO_C_MODIFY, m_iErrorCode);
}

const db_sc_insert_t& DBClient::SendInsertDB(db_cs_insert_t& dStruct)
{
	UtilsSerialize dSerialize;
	serialize_prot_cs_insert(dStruct, dSerialize);
	this->SendText(dSerialize.getData(), dSerialize.getLength());
	m_InsertRef.protId = -1;
	m_InsertRef.dataKey = "";
	CHECK_WHILE_RETURN(m_InsertRef, PROTOCOL_DB_S_TO_C_INSERT, m_iErrorCode);
}

const db_sc_loadtable_t& DBClient::SendLoadTableDB(db_cs_loadtable_t& dStruct)
{
	UtilsSerialize dSerialize;
	serialize_prot_cs_loadtable(dStruct, dSerialize);
	this->SendText(dSerialize.getData(), dSerialize.getLength());

	m_LoadTableRef.protId = -1;
	CHECK_WHILE_RETURN(m_LoadTableRef, PROTOCOL_DB_S_TO_C_LOADTABLE, m_iErrorCode);
}

void DBClient::NotifyRecvText(const char* pData, int datalen)
{
	UtilsSerialize dSerialize(pData, datalen);
	int protId = unserialize_db_protid(dSerialize);
	switch (protId)
	{
		case PROTOCOL_DB_S_TO_C_QUERY:
		{
			unserialize_prot_sc_query(m_QueryRef, dSerialize);
		}
		break;
		case PROTOCOL_DB_S_TO_C_MODIFY:
		{
			unserialize_prot_sc_modify(m_ModifyRef, dSerialize);
		}
		break;
		case PROTOCOL_DB_S_TO_C_INSERT:
		{
			unserialize_prot_sc_insert(m_InsertRef, dSerialize);
		}
		break;
		case PROTOCOL_DB_S_TO_C_LOADTABLE:
		{
			unserialize_prot_sc_loadtable(m_LoadTableRef, dSerialize);
		}
		break;
		case PROTOCOL_DB_S_TO_C_ERROR:
		{
			db_prot_error_t dStruct;
			unserialize_prot_error(dStruct, dSerialize);
			m_iErrorCode = dStruct.code;
		}
		default:
			FUAssert(false, "not the prot ID!");
			break;
	}
}

