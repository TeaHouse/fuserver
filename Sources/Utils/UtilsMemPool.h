#ifndef __FU_UTILS_MEMORY_POOL_H_
#define __FU_UTILS_MEMORY_POOL_H_

#include "../Utils/UtilsLock.h"


template<class Type, int tSize>
class UtilsMemPool
{
	typedef void(*ExternCallFunc_Type)(Type*);
public:
	UtilsMemPool();	//在创建指针后，使用前要调用的初始化函数
	~UtilsMemPool();

	Type* Create(ExternCallFunc_Type affterCreateFunc = NULL);
	void  Free(Type* pData, ExternCallFunc_Type beforeFreeFunc = NULL);
	void  Extern(int t = 2);
private:
	void  ExternBuffer(int t = 2);

private:
	int*	m_pPoolArray;
	int		m_iPoolSize;
	int		m_iBufferSize;
	UtilsLock	m_Lock;
};

template<class Type, int tSize>
UtilsMemPool<Type, tSize>::UtilsMemPool()
: m_iPoolSize(30)
, m_iBufferSize(100)
, m_pPoolArray(NULL)
{
	m_Lock.Lock();
	m_pPoolArray = (int*)malloc(sizeof(int)* m_iBufferSize);

	for (int i = 0; i < m_iPoolSize; i++)
	{
		m_pPoolArray[i] = (int)malloc(tSize);
	}
	m_Lock.unLock();
}

template<class Type, int tSize>
UtilsMemPool<Type, tSize>::~UtilsMemPool()
{
	m_Lock.Lock();
	for (int i = 0; i < m_iPoolSize; i++)
	{
		free((void*)(m_pPoolArray[i]));
	}
	free((void*)m_pPoolArray);

	m_iPoolSize = 0;
	m_Lock.unLock();
}

template<class Type, int tSize>
Type* UtilsMemPool<Type, tSize>::Create(ExternCallFunc_Type affterCreateFunc /*= NULL*/)
{
	m_Lock.Lock();
	if (m_iPoolSize <= 0)
	{
		int externDataNum = m_iBufferSize / 2;
		for (int i = 0; i < externDataNum; i++)
		{
			m_pPoolArray[i] = (int)malloc(tSize);
		}
		m_iPoolSize = externDataNum;
	}
	//printf("--m_iPoolSize=[%d]----\n", m_iPoolSize);
	Type* pData = (Type*)(m_pPoolArray[m_iPoolSize - 1]);
	m_pPoolArray[m_iPoolSize - 1] = NULL;
	m_iPoolSize--;
	if (affterCreateFunc) affterCreateFunc(pData);

	m_Lock.unLock();
	return pData;
}
template<class Type, int tSize>
void UtilsMemPool<Type, tSize>::Free(Type* pData, ExternCallFunc_Type beforeFreeFunc /*= NULL*/)
{
	m_Lock.Lock();
	if (m_iPoolSize >= m_iBufferSize)
	{
		ExternBuffer();
	}
	//printf("--m_iPoolSize=[%d]----\n", m_iPoolSize);
	if (beforeFreeFunc)beforeFreeFunc(pData);
	m_pPoolArray[m_iPoolSize] = (int)pData;
	m_iPoolSize++;
	m_Lock.unLock();
}

template<class Type, int tSize>
void UtilsMemPool<Type, tSize>::ExternBuffer(int t /*= 2*/)
{
	m_iBufferSize = m_iBufferSize * t;
	m_pPoolArray = (int*)realloc(m_pPoolArray, sizeof(int)* m_iBufferSize);
	//::CopyMemory(pArray, m_pPoolArray, m_iBufferSize*sizeof(int));
	//m_pPoolArray = pArray;
	//m_iBufferSize = externSize;
}

template<class Type, int tSize>
void UtilsMemPool<Type, tSize>::Extern(int t /*= 2*/)
{
	m_Lock.Lock();
	ExternBuffer(t);
	m_Lock.unLock();
}


#endif