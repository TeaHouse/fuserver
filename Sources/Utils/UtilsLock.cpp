
#include "UtilsLock.h"

UtilsLock::UtilsLock()
{
#ifdef WIN32
	::InitializeCriticalSection(&m_Lock);
#else
	pthread_mutex_init(&m_Lock, NULL);
#endif
}

UtilsLock::~UtilsLock()
{
#ifdef WIN32
	::DeleteCriticalSection(&m_Lock);
#else
	pthread_mutex_destroy(&m_Lock);
#endif
}

void UtilsLock::Lock()
{
#ifdef WIN32
	::EnterCriticalSection(&m_Lock);
#else
	pthread_mutex_lock(&m_Lock);
#endif
}

void UtilsLock::unLock()
{
#ifdef WIN32
	::LeaveCriticalSection(&m_Lock);
#else
	pthread_mutex_unlock(&m_Lock);
#endif
}

