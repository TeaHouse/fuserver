#include "NetServer_Platform.h"

NetServer_Platform::NetServer_Platform()
	: m_ListenSocket(-1)
	, m_bShutDown(false)
	, m_bStart(false)
	, m_bHeartBeat(true)
{

}

NetServer_Platform::~NetServer_Platform()
{

}

