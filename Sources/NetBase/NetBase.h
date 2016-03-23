#ifndef __FU_NET_NETBASE_H__
#define __FU_NET_NETBASE_H__


#include "../Platform/Include.h"

//�������ͻ��˹�������
#define		NET_BUFFER_MAXSIZE			64	//Ĭ���״δ�recv��buffer�Ĵ�С,�ֽ�
#define		NET_LISTEN_MAXSIZE			2048
//#define		ENCRYPT_PROT_BUFFER			//�Ƿ����


//��������
int fu_get_ipv4(std::vector<std::string>& ipList);
int fu_get_ipv6(std::vector<std::string>& ipList);

int fu_socket_send(int sockfd, const char* buf, int buflen);
int fu_socket_recv(int sockfd, std::string& recvStr);

#endif