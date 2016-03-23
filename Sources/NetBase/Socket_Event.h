#ifndef __FU_NETBASE_SOCKET_LINUX_H__
#define __FU_NETBASE_SOCKET_LINUX_H__

#ifndef WIN32
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif

static bool socket_event_invalid(int efd)
{
	return efd == -1;
}

static int socket_event_create()
{
#ifndef WIN32
	return epoll_create(1024);
#endif
	return 0;
}

static void socket_event_release(int efd)
{
#ifndef WIN32
	close(efd);
#endif
}

static int socket_event_add(int efd, int sock, void *ud)
{
#ifndef WIN32
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = ud;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, sock, &ev) == -1) {
		return 1;
	}
#endif
	return 0;
}

static void socket_event_del(int efd, int sock)
{
#ifndef WIN32
	epoll_ctl(efd, EPOLL_CTL_DEL, sock, NULL);
#endif
}

static void socket_event_write(int efd, int sock, void *ud, bool enable)
{
#ifndef WIN32
	struct epoll_event ev;
	ev.events = EPOLLIN | (enable ? EPOLLOUT : 0);
	ev.data.ptr = ud;
	epoll_ctl(efd, EPOLL_CTL_MOD, sock, &ev);
#endif
}

static int socket_event_wait(int efd, struct event *e, int max)
{
#ifndef WIN32
	struct epoll_event ev[max];
	int n = epoll_wait(efd, ev, max, -1);
	int i;
	for (i = 0; i < n; i++) {
		e[i].s = ev[i].data.ptr;
		unsigned flag = ev[i].events;
		e[i].write = (flag & EPOLLOUT) != 0;
		e[i].read = (flag & EPOLLIN) != 0;
	}
	return n;
#endif
	return 0;
}

static void socket_event_nonblocking(int fd)
{
#ifndef WIN32
	int flag = fcntl(fd, F_GETFL, 0);
	if (-1 == flag) {
		return;
	}

	fcntl(fd, F_SETFL, flag | O_NONBLOCK);
#endif
}


#endif
