#include "UtilsQueue.h"
#include "UtilsFunction.h"

UtilsQueue::UtilsQueue(int size /*= 50*/)
	: m_iMaxSize(size)
	, m_ibegin(0)
	, m_iDataSize(0)
	, m_QueueArray(NULL)
{
	Reset(size);
}

UtilsQueue::~UtilsQueue()
{
	Clear();
	FU_SAFE_DELETE(m_QueueArray);
}

void UtilsQueue::Reset(int size)
{
	if (size < m_iMaxSize) return;
	int* pArray = new int[size];
	int endindex = (m_ibegin + m_iDataSize) % m_iMaxSize;
	if (m_ibegin > endindex)
	{
		int toPos = (m_iMaxSize - m_ibegin)* sizeof(int);
		memcpy(pArray, m_QueueArray + m_ibegin, toPos);
		memcpy(pArray + toPos, m_QueueArray, endindex*sizeof(int));
	}
	else if (m_ibegin<endindex)
	{
		memcpy(pArray, m_QueueArray + m_ibegin, m_iDataSize* sizeof(int));
	}

	FU_SAFE_DELETE(m_QueueArray);
	m_ibegin = 0;
	m_QueueArray = pArray;
	m_iMaxSize = size;
}

bool UtilsQueue::PushBack(char* pData, bool bExtern /*= true*/)
{
	bool temp = false;
	do 
	{
		if (m_iDataSize >= m_iMaxSize)
		{
			if (!bExtern) break;
			Reset(int(m_iMaxSize*1.5f));
		}

		int toIndex = (m_iDataSize + m_ibegin) % m_iMaxSize;
		m_QueueArray[toIndex] = (int)pData;
		m_iDataSize++;

		temp = true;
	} while (0);

	return temp;
}

char* UtilsQueue::PopFront()
{
	char* pData = NULL;
	do 
	{
		if (m_iDataSize <= 0) break;
		pData = (char*)m_QueueArray[m_ibegin];

		m_ibegin++;
		m_ibegin %= m_iMaxSize;
	} while (0);
	return pData;
}

char* UtilsQueue::Front()
{
	if (m_iDataSize <= 0) return NULL;
	char* pData = (char*)m_QueueArray[m_ibegin];

	return pData;
}

void UtilsQueue::Clear(bool bDelIn)
{
	while (m_iDataSize > 0)
	{
		char* pData = PopFront();
		FU_SAFE_DELETE(pData);
	}
	m_ibegin = 0;
	m_iDataSize = 0;
}

bool UtilsQueue::Empty()
{
	return m_iDataSize <= 0;
}

