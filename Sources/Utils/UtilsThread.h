#ifndef __FU_UTILS_THREAD_H__
#define __FU_UTILS_THREAD_H__


#include "../Platform/Include.h"

typedef void* (*THREAD_FUNCTION_TYPE)(void*);

class UtilsThread
{
public:
	UtilsThread();
	~UtilsThread();

	bool Create(THREAD_FUNCTION_TYPE func, char* paramData);
	void Destroy(bool waitEnd);

private:
#ifdef WIN32
	std::unordered_map<HANDLE, char*>  m_HandlerList;
#else
	std::unordered_map<pthread_t, char*>  m_HandlerList;
#endif
};





#endif