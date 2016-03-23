#ifndef __FU_DATABASE_CLIENT_H__
#define __FU_DATABASE_CLIENT_H__

#include "../DBBase/DBProtocol.h"
#include "../NetBase/NetClient.h"

class DBClient : public NetClient
{
public:
	DBClient();
	~DBClient();

	const db_sc_query_t&		SendQueryDB(db_cs_query_t& dStruct);
	const db_sc_modify_t&		SendModifyDB(db_cs_modify_t& dStruct);
	const db_sc_insert_t&		SendInsertDB(db_cs_insert_t& dStruct);
	const db_sc_loadtable_t&	SendLoadTableDB(db_cs_loadtable_t& dStruct);

	virtual void NotifyRecvText(const char* pData, int datalen);

private:
	//用于中转的变量
	db_sc_query_t		m_QueryRef;
	db_sc_modify_t		m_ModifyRef;
	db_sc_insert_t		m_InsertRef;
	db_sc_loadtable_t	m_LoadTableRef;

	int		m_iErrorCode;
};








#endif