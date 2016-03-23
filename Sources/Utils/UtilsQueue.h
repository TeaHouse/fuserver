#ifndef __FU_UTILS_QUEUE_H__
#define __FU_UTILS_QUEUE_H__

#include "../Platform/Include.h"
#include "UtilsLock.h"

class UtilsQueue
{
public:
	UtilsQueue(int size = 50);
	~UtilsQueue();

	void  Reset(int size);
	bool  PushBack(char* pData, bool bExtern = true); //压入，队列满了就返回失败，压入失败, bExtern=true表示队列满了就扩展
	char* PopFront();
	char* Front();
	void  Clear(bool bDelIn = true);
	bool  Empty();
private:
	int		m_ibegin;
	int		m_iMaxSize;
	int		m_iDataSize;
	int*	m_QueueArray;
};





#endif