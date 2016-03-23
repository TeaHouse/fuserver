
#include "NetServer_Libevent.h"
#include "../Utils/UtilsThread.h"

#include "../three-party/libevent/event2/bufferevent.h"
#include "../three-party/libevent/event2/buffer.h"
#include "../three-party/libevent/event2/listener.h"
#include "../three-party/libevent/event2/util.h"
#include "../three-party/libevent/event2/event.h"

UtilsThread		g_ThreadHandler;
static void* st_WaitEeventThreadProc(void* lpParam)
{
	NetServer_Libevent* pThis = (NetServer_Libevent*)lpParam;
	struct event_base* pEventBase = (struct event_base*)pThis->GetEventBase();

	pThis->SetIsStart(true);
	FULOG("Create Server Success ip=[%s], port=[%d]",
		  pThis->GetIp().c_str(), pThis->GetPort());
	event_base_dispatch(pEventBase);
	
	//pThis->DestroyServer();
	return 0; 
}

static void st_ReadFinishFunc(struct bufferevent *bev, void *user_data) //写完成回调
{
	NetServer_Libevent* pThis = (NetServer_Libevent*)user_data;
	char pReadBuf[NET_BUFFER_MAXSIZE] = {};
	std::string recvStr;
	int readSize = 0;
	do
	{
		readSize = bufferevent_read(bev, pReadBuf, NET_BUFFER_MAXSIZE);
		if (readSize > 0)
		{
			recvStr.append(pReadBuf, readSize);
		}
		else
		{
			break;
		}
	} while (1);
	recvStr.append(1, '\0');
	pThis->NotifyRecvFinish((int)bev, recvStr.c_str(), recvStr.length());
}
static void st_WriteFinishFunc(struct bufferevent *bev, void *user_data) //有读数据时候回调
{
	//bufferevent_get_output(bev);
	struct evbuffer *output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0)
	{
		//printf("flushed answer\n");
		//bufferevent_free(bev);
		NetServer_Libevent* pThis = (NetServer_Libevent*)user_data;
		pThis->NotifySendFinish((int)bev);
	}
}
static void st_DisconnectFunc(struct bufferevent *bev, short events, void *user_data)
{
	NetServer_Libevent* pThis = (NetServer_Libevent*)user_data;
	if (events & BEV_EVENT_EOF || events & BEV_EVENT_ERROR)
	{
		FULOG("Connection closed.\n");
		pThis->NotifyCloseLink((int)bev, 0);
		bufferevent_free(bev);
	}
	else if (events & BEV_EVENT_TIMEOUT)
	{
		FULOG("Timed out\n"); //if bufferevent_set_timeouts() called
	}
}
static void st_AcceptFunc(struct evconnlistener *listener, evutil_socket_t fd,
struct sockaddr *sa, int socklen, void *user_data)
{
	NetServer_Libevent* pThis = (NetServer_Libevent*)user_data;
	struct event_base* pEventBase = (struct event_base*)pThis->GetEventBase();

	struct bufferevent *bev = bufferevent_socket_new(pEventBase, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		FUError("Error constructing bufferevent!");
		event_base_loopbreak(pEventBase);
		return;
	}
	bufferevent_setcb(bev, st_ReadFinishFunc, st_WriteFinishFunc, st_DisconnectFunc, user_data);
	bufferevent_enable(bev, EV_WRITE | EV_READ | EV_PERSIST);

	//printf("------listener_cb------[%s]------\n", inet_ntoa(((sockaddr_in*)sa)->sin_addr));
	pThis->NotifyEstablishLink((int)bev, inet_ntoa(((sockaddr_in*)sa)->sin_addr));
}

//////////////////////////////////////////////////////////////////////////
NetServer_Libevent::NetServer_Libevent()
{
#ifdef WIN32
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif
}

NetServer_Libevent::~NetServer_Libevent()
{
	DestroyServer();
}

bool NetServer_Libevent::CreateServer(const char* serverIp, int serverPort)
{
	if (GetIsStart()) return false;
	struct event_base* pEventBase = NULL;
	struct evconnlistener* pListener = NULL;

	struct sockaddr_in sin;
	pEventBase = event_base_new();
	if (!pEventBase)
	{
		FUError("Could not initialize libevent!\n");
		return false;
	}
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(serverPort);
	sin.sin_addr.S_un.S_addr = inet_addr(serverIp);

	pListener = evconnlistener_new_bind(pEventBase, st_AcceptFunc, (void*)this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_EXEC, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));
	if (!pListener)
	{
		FUError("Could not create a listener!");
		return false;
	}
	SetListenSocket((int)pListener);
	SetEventBase((int)pEventBase);
	SetIp(serverIp);
	SetPort(serverPort);

	g_ThreadHandler.Create(st_WaitEeventThreadProc, (char*)this);

	return true;
}

void NetServer_Libevent::DestroyServer()
{
	if (GetIsStart())
	{
		struct evconnlistener* plistener = (struct evconnlistener*)GetListenSocket();
		struct event_base* pbase = (struct event_base*)GetEventBase();
		if (pbase) 


		if (plistener)evconnlistener_free(plistener);
		if (pbase)
		{
			event_base_loopexit(pbase, 0);
			event_base_free(pbase);
		}
		g_ThreadHandler.Destroy(false);
		SetIsStart(false);
	}
}

bool NetServer_Libevent::SendText(int sock, const char* buf, int buflen)
{
	if (buflen <= 0 || buf == NULL) return false;
	struct bufferevent* bev = (struct bufferevent*)sock;

	bufferevent_write(bev, buf, buflen);
	return true;
}

bool NetServer_Libevent::SendText(int sock, const std::string& sendBuf)
{
	return this->SendText(sock, sendBuf.c_str(), sendBuf.length());
}

void NetServer_Libevent::CloseSocket(int sock_id)
{
	bufferevent_free((struct bufferevent*)sock_id);
}

