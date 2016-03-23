#include "DBProtocol.h"
#include "../Utils/UtilsSerialize.h"


int unserialize_db_protid(UtilsSerialize& dSerialize)
{
	int protId = 0;
	dSerialize.Output(protId);
	return protId;
}

void serialize_db_protid(int protId, UtilsSerialize& dSerialize)
{
	dSerialize.Input(protId);
}

//////////////////////////////////////////////////////////////////////////
void unserialize_prot_error(db_prot_error_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
	dSerialize.Output(dStruct.code);
}

void serialize_prot_error(db_prot_error_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
	dSerialize.Input(dStruct.code);
}
//////////////////////////////////////////////////////////////////////////
void unserialize_prot_sc_loadtable(db_sc_loadtable_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
}

void unserialize_prot_cs_loadtable(db_cs_loadtable_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
	dSerialize.Output(dStruct.dbname);
	dSerialize.Output(dStruct.tname);
}

void serialize_prot_sc_loadtable(db_sc_loadtable_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
}

void serialize_prot_cs_loadtable(db_cs_loadtable_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
	dSerialize.Input(dStruct.dbname);
	dSerialize.Input(dStruct.tname);
}
//////////////////////////////////////////////////////////////////////////
void unserialize_prot_sc_query(db_sc_query_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
	int nameSize = 0;
	dSerialize.Output(nameSize);
	for (int i = 0; i < nameSize; i++)
	{
		std::string toName;
		dSerialize.Output(toName);
		dStruct.fieldNameList.push_back(toName);
	}
	int recordSize = 0;
	dSerialize.Output(recordSize);
	for (int i = 0; i < recordSize; i++)
	{
		int fieldSize = 0;
		dSerialize.Output(fieldSize);
		std::vector<std::string>  fieldList;
		for (int j = 0; j < fieldSize; j++)
		{
			std::string toName;
			dSerialize.Output(toName);
			fieldList.push_back(toName);
		}
		dStruct.recordList.push_back(fieldList);
	}
}

void unserialize_prot_cs_query(db_cs_query_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
	dSerialize.Output(dStruct.tname);
	dSerialize.Output(dStruct.fieldName);
	dSerialize.Output(dStruct.dataEquKey);
	dSerialize.Output(dStruct.dataMinKey);
	dSerialize.Output(dStruct.dataMaxKey);
}

void serialize_prot_sc_query(db_sc_query_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
	int nameSize = dStruct.fieldNameList.size();
	dSerialize.Input(nameSize);
	for (auto& name : dStruct.fieldNameList)
	{
		dSerialize.Input(name);
	}
	int recordSize = dStruct.recordList.size();
	dSerialize.Input(recordSize);
	for (auto& fieldList : dStruct.recordList)
	{
		int fieldSize = fieldList.size();
		dSerialize.Input(fieldSize);
		for (auto& field : fieldList)
		{
			dSerialize.Input(field);
		}
	}
}

void serialize_prot_cs_query(db_cs_query_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
	dSerialize.Input(dStruct.tname);
	dSerialize.Input(dStruct.fieldName);
	dSerialize.Input(dStruct.dataEquKey);
	dSerialize.Input(dStruct.dataMinKey);
	dSerialize.Input(dStruct.dataMaxKey);
}

//////////////////////////////////////////////////////////////////////////
void unserialize_prot_sc_modify(db_sc_modify_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
}

void unserialize_prot_cs_modify(db_cs_modify_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
	dSerialize.Output(dStruct.tname);
	dSerialize.Output(dStruct.dataKey);
	int fieldSize = 0;
	dSerialize.Output(fieldSize);
	for (int i = 0; i < fieldSize; i++)
	{
		std::string firstStr;
		dSerialize.Output(firstStr);
		std::string secondStr;
		dSerialize.Output(secondStr);
		dStruct.dataList.insert(std::make_pair(firstStr, secondStr));
	}
}

void serialize_prot_sc_modify(db_sc_modify_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
}

void serialize_prot_cs_modify(db_cs_modify_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
	dSerialize.Input(dStruct.tname);
	dSerialize.Input(dStruct.dataKey);
	int dataSize = dStruct.dataList.size();
	dSerialize.Input(dataSize);
	for (auto& iter : dStruct.dataList)
	{
		dSerialize.Input(iter.first);
		dSerialize.Input(iter.second);
	}
}
//////////////////////////////////////////////////////////////////////////
void unserialize_prot_sc_insert(db_sc_insert_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
	dSerialize.Output(dStruct.dataKey);
}

void unserialize_prot_cs_insert(db_cs_insert_t& dStruct, UtilsSerialize& dSerialize)
{
	dSerialize.Output(dStruct.protId);
	dSerialize.Output(dStruct.tname);
	int fieldSize = 0;
	dSerialize.Output(fieldSize);
	for (int i = 0; i < fieldSize; i++)
	{
		std::string firstStr;
		dSerialize.Output(firstStr);
		std::string secondStr;
		dSerialize.Output(secondStr);
		dStruct.dataList.insert(std::make_pair(firstStr, secondStr));
	}
}

void serialize_prot_sc_insert(db_sc_insert_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
	dSerialize.Input(dStruct.dataKey);
}

void serialize_prot_cs_insert(db_cs_insert_t& dStruct, UtilsSerialize& dSerialize)
{
	serialize_db_protid(dStruct.protId, dSerialize);
	dSerialize.Input(dStruct.protId);
	dSerialize.Input(dStruct.tname);
	int fieldSize = dStruct.dataList.size();
	dSerialize.Input(fieldSize);
	for (auto& iter : dStruct.dataList)
	{
		dSerialize.Input(iter.first);
		dSerialize.Input(iter.second);
	}
}

//////////////////////////////////////////////////////////////////////////
