#include "DBServer.h"
#include "../DBBase/DBProtocol.h"
#include "../DBBase/DBManager.h"

DBServer::DBServer()
{
	DBManager::getSingletonPtr()->Init();
}

DBServer::~DBServer()
{

}

void DBServer::NotifyEstablishLink(int sock, const SOCKADDR_IN* localAddr, const SOCKADDR_IN* remoteAddr)
{

}

void DBServer::NotifyRecvFinish(int sock, const char* buf, int buflen)
{
	int protSize = 0;
	int errorcode = -1;
	UtilsSerialize dSerialize(buf, buflen);
	int protId = unserialize_db_protid(dSerialize);
	switch (protId)
	{
	case PROTOCOL_DB_C_TO_S_QUERY:
		{
			db_cs_query_t dInStruct;
			unserialize_prot_cs_query(dInStruct, dSerialize);

			db_sc_query_t dOutStruct;
			dOutStruct.protId = PROTOCOL_DB_S_TO_C_QUERY;
			bool temp = DBManager::getSingletonPtr()->QueryDB(dOutStruct.recordList, dOutStruct.fieldNameList,
				dInStruct.tname, dInStruct.fieldName, dInStruct.dataEquKey, dInStruct.dataMinKey, dInStruct.dataMaxKey);
			if (temp)
			{
				dSerialize.ResetString();
				serialize_prot_sc_query(dOutStruct, dSerialize);
				this->SendText(sock, dSerialize.getData(), dSerialize.getLength());
			}
			else
			{
				errorcode = ERROR_CODE_QUERY;
			}
		}
		break;
	case PROTOCOL_DB_C_TO_S_MODIFY:
		{
			db_cs_modify_t dInStruct;
			unserialize_prot_cs_modify(dInStruct, dSerialize);
			bool temp = DBManager::getSingletonPtr()->ModifyDB(dInStruct.tname, dInStruct.dataKey, dInStruct.dataList);

			if (temp)
			{
				dSerialize.ResetString();
				db_sc_modify_t dOutStruct;
				dOutStruct.protId = PROTOCOL_DB_S_TO_C_MODIFY;
				serialize_prot_sc_modify(dOutStruct, dSerialize);
				this->SendText(sock, dSerialize.getData(), dSerialize.getLength());
			}
			else
			{
				errorcode = ERROR_CODE_MODIFY;
			}
		}
		break;
	case PROTOCOL_DB_C_TO_S_INSERT:
		{
			db_cs_insert_t dInStruct;
			unserialize_prot_cs_insert(dInStruct, dSerialize);
			std::string dataKey = DBManager::getSingletonPtr()->InsertDB(dInStruct.tname, dInStruct.dataList);

			if (dataKey!="")
			{
				dSerialize.ResetString();
				db_sc_insert_t dOutStruct;
				dOutStruct.protId = PROTOCOL_DB_S_TO_C_INSERT;
				dOutStruct.dataKey = dataKey;
				serialize_prot_sc_insert(dOutStruct, dSerialize);
				this->SendText(sock, dSerialize.getData(), dSerialize.getLength());
			}
			else
			{
				errorcode = ERROR_CODE_INSERT;
			}
		}
		break;
	case PROTOCOL_DB_C_TO_S_LOADTABLE:
		{
			db_cs_loadtable_t dInStruct;
			unserialize_prot_cs_loadtable(dInStruct, dSerialize);
			bool temp = DBManager::getSingletonPtr()->LoadTable(dInStruct.dbname, dInStruct.tname);

			if (temp)
			{
				dSerialize.ResetString();
				db_sc_insert_t dOutStruct;
				dOutStruct.protId = PROTOCOL_DB_S_TO_C_LOADTABLE;
				serialize_prot_sc_insert(dOutStruct, dSerialize);
				this->SendText(sock, dSerialize.getData(), dSerialize.getLength());
			}
			else
			{
				errorcode = ERROR_CODE_LOADDB;
			}
		}
		break;
	default:
		break;
	}
	if (errorcode>0)
	{
		dSerialize.ResetString();
		db_prot_error_t dOutStruct;
		dOutStruct.protId = PROTOCOL_DB_S_TO_C_ERROR;
		dOutStruct.code = errorcode;

		serialize_prot_error(dOutStruct, dSerialize);
		this->SendText(sock, dSerialize.getData(), dSerialize.getLength());
	}
}

void DBServer::NotifySendFinish(int sock, const char* buf, int buflen)
{

}

void DBServer::NotifyCloseLink(int sock, int code)
{

}
