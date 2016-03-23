#ifndef __FU_NET_NETBASE_H__
#define __FU_NET_NETBASE_H__


#include "../Platform/Include.h"

//服务器客户端共有配置
#define		NET_BUFFER_MAXSIZE			64	//默认首次次recv收buffer的大小,字节
#define		NET_LISTEN_MAXSIZE			2048
//#define		ENCRYPT_PROT_BUFFER			//是否加密


//公共函数
int fu_get_ipv4(std::vector<std::string>& ipList);
int fu_get_ipv6(std::vector<std::string>& ipList);

int fu_socket_send(int sockfd, const char* buf, int buflen);
int fu_socket_recv(int sockfd, std::string& recvStr);

#endif