#include "UtilsThread.h"

struct utils_thread_t
{
	THREAD_FUNCTION_TYPE	func;
	char*	param;
};

#ifdef WIN32
DWORD WINAPI  st_WaitEeventThreadProc(LPVOID lpParam)
#else
void* st_WaitEeventThreadProc(void* lpParam)
#endif
{
	utils_thread_t* toThread = (utils_thread_t*)lpParam;
	toThread->func(toThread->param);

	return 0;
}


UtilsThread::UtilsThread()
{
}

UtilsThread::~UtilsThread()
{
	Destroy(false);
}

bool UtilsThread::Create(THREAD_FUNCTION_TYPE func, char* paramData)
{
	utils_thread_t* pData = new utils_thread_t;
	pData->func = func;
	pData->param = paramData;
#ifdef WIN32
	HANDLE pHandler = ::CreateThread(NULL, 0, st_WaitEeventThreadProc, (LPVOID)pData, 0, NULL);
	if (pHandler == NULL)
	{
		return false;
	}
#else
	pthread_t pHandler;
	if (0 != pthread_create(&pHandler, NULL, st_WaitEeventThreadProc, (void*)pThread))
	{
		FUError("pthread_create Error!");
		return false;
	}
#endif
	m_HandlerList.insert(std::make_pair(pHandler, (char*)pData));
	return true;
}

void UtilsThread::Destroy(bool waitEnd)
{
	if (!m_HandlerList.empty())
	{
		for (auto& iter : m_HandlerList)
		{
#ifdef WIN32
			if (waitEnd)::WaitForSingleObject(iter.first, INFINITE);
			::CloseHandle(iter.first);
#else
			if (waitEnd)pthread_join(pHandler, NULL);
			if (0 != pthread_cancel(pHandler))
			{
				FUError("pthread_cancel Error!");
			}
#endif
			FU_SAFE_DELETE(iter.second);
		}
		m_HandlerList.clear();
	}
}

