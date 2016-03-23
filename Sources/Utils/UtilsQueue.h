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
	bool  PushBack(char* pData, bool bExtern = true); //ѹ�룬�������˾ͷ���ʧ�ܣ�ѹ��ʧ��, bExtern=true��ʾ�������˾���չ
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