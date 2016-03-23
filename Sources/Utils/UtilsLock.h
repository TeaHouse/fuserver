#ifndef __FU_UTILS_LOCK_H__
#define __FU_UTILS_LOCK_H__

#include "../Platform/Include.h"


class UtilsLock
{
public:
	UtilsLock();
	~UtilsLock();

	void Lock();
	void unLock();

private:
#ifdef WIN32
	CRITICAL_SECTION	m_Lock;
#else
	pthread_mutex_t		m_Lock;
#endif
};







#endif