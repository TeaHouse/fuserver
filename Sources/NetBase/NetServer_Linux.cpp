#include "NetServer_Linux.h"
#include "Socket_Event.h"

#ifndef WIN32

UtilsThread		g_ThreadHandler;
struct epoll_event		g_EventHandler; //全局event
int		g_iEventSocket = -1;	//全局socket


static void* st_WaitEeventThreadProc(void* lpParam)
{
	NetServer_Linux* pNetThis = (NetServer_Linux*)lpParam;
	struct epoll_event toKeepEvent[NET_EPOLL_ONLINE_MAX];
	int waitCount, i;
	struct sockaddr_in accept_addr;
	int accept_addr_len = sizeof(struct sockaddr_in);
	int recvfd, recvlen;
	char*  pRecvBuffer[NET_BUFFER_MAXSIZE] = {};
	int listenSockfd = pNetThis->GetListenSocket();
	while(true){
		waitCount = epoll_wait(g_iEventSocket, toKeepEvent, NET_EPOLL_ONLINE_MAX, -1);
		for(i=0; i<waitCount; i++){
			if(toKeepEvent[i].data.fd==listenSockfd){
				recvfd = accept(g_iEventSocket, (struct sockaddr*)&accept_addr, accept_addr_len)
				if(recvfd==-1){
					FULOG("accept: error!n");
					continue;
				}
				event_nonblocking(recvfd);
				event_add(recvfd);
				pNetThis->NotifyEstablishLink(recvfd, NULL);
			}
			else if(toKeepEvent[i].events & EPOLLERR){
				int fd = toKeepEvent[i].data.fd;
				pNetThis->EpollCtl(fd, EPOLL_CTL_DEL, EPOLLERR);
				::close(fd);
			}
			else if(toKeepEvent[i].events & EPOLLHUP || toKeepEvent[i].events & EPOLLRDHUP){
				pNetThis->NotifyCloseLink(toKeepEvent[i].data.fd, 0);
				::close(toKeepEvent[i].data.fd);
				FULOG("客户端断开链接！");
			}
			else if(toKeepEvent[i].events & EPOLLIN){
				int fd = toKeepEvent[i].data.fd;
				printf("-----1-------------EPOLLIN--fd=[%d]--\n", fd);
				std::string recvStr = "";
				if(fu_socket_recv(fd, recvStr)<0)
				{
					pNetThis->EpollCtl(fd, EPOLL_CTL_DEL, EPOLLERR);
					::close(fd);
				}
				if (recvStr.empty()) continue;
				pNetThis->NotifyRecvFinish(fd, recvStr.c_str(), recvStr.length());
				printf("-----2-------------EPOLLIN----\n");
			}else {
				::close(toKeepEvent[i].data.fd);
			}
		}
	}
	return 0;
}


NetServer_Linux::NetServer_Linux()
	: g_pGlobalEvent(NULL)
{

}

NetServer_Linux::~NetServer_Linux()
{
	g_ThreadHandler.Destroy(true);
}

bool NetServer_Linux::CreateServer(const char* serverIp, int serverPort)
{
	if(GetIsStart()) return false;
	m_ListenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListenSocket==-1 || !SetNotBlockSocket(m_ListenSocket)) return false;

	struct sockaddr_in toAddr;
	toAddr.sin_family = AF_INET;
	toAddr.sin_port = htons((unsigned short)serverPort);
	
	toAddr.sin_addr.s_addr = inet_addr(serverIp);


	if (-1 == ::bind(m_ListenSocket, (struct sockaddr*)&toAddr, sizeof(toAddr)));
	{
		::close(m_ListenSocket);
		return false;
	}

	if (-1 == ::listen(m_ListenSocket, NET_LISTEN_MAXSIZE))
	{
		::close(m_ListenSocket);
		return false;
	}


	g_iEventSocket = epoll_create(1024);
	if (g_iEventSocket==-1) return false;

	g_EventHandler.data.fd = m_ListenSocket;
	g_EventHandler.events = EPOLLIN | EPOLLET;

	if (!EpollCtl(g_iEventSocket, m_ListenSocket, EPOLL_CTL_ADD, EPOLLIN|EPOLLET, &g_EventHandler))
	{
		return false;
	}
	g_ThreadHandler.Create(st_WaitEeventThreadProc, this);
	SetIsStart(true);
	SetIp(serverIp);
	SetPort(serverPort);
}

void NetServer_Linux::DestroyServer()
{
	SetIsStart(false);
	g_ThreadHandler.Destroy(true);
	::close(g_iEventSocket);
	::close(m_ListenSocket);
}

bool NetServer_Linux::SendText(int sock, const char* buf, int buflen)
{
	if(fu_socket_send(sock, buf, buflen) == buflen)
	{
		NotifySendFinish(sock);
	}
}

void NetServer_Linux::CloseSocket(int sock_id)
{
	::close(sock_id);
}

bool NetServer_Linux::EpollCtl(int connfd, int option, unsigned int toEvent)
{
	g_EventHandler.data.fd = connfd;
	g_EventHandler.events = toEvent;
	if (-1==epoll_ctl(g_iEventSocket, option, connfd, g_EventHandler))
	{
		return false;
	}
	return true;
}

#endif