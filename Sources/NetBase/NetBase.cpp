#include "NetBase.h"
#include "../Utils/UtilsSerialize.h"

int fu_get_ipv4(std::vector<std::string>& ipList)
{
	PHOSTENT hostinfo;
	char hostname[255] = { 0 };      //主机名   
	char *port = "100";			//端口号 
	gethostname(hostname, sizeof(hostname));
	if ((hostinfo = gethostbyname(hostname)) == NULL)    //获得本地ipv4地址
	{
		//errno = GetLastError();
		//fprintf(stderr, "gethostbyname Error:%d\n", errno);
		return 0;
	}
	LPCSTR ip;
	while (*(hostinfo->h_addr_list) != NULL)        //输出ipv4地址
	{
		ip = inet_ntoa(*(struct in_addr *) *hostinfo->h_addr_list);
		//printf("ipv4 addr = %s\n\n", ip);
		ipList.push_back(std::string(ip));
		hostinfo->h_addr_list++;
	}
	return ipList.size();
}
int fu_get_ipv6(std::vector<std::string>& ipList)
{
	struct addrinfo *ailist, *aip;
	struct addrinfo myhint;
	struct sockaddr_in6 *sinp6;
	myhint.ai_family = AF_INET6;                   /*  myhint 的限定设置  */
	myhint.ai_socktype = SOCK_STREAM;     /*   这里可是设置 socket type    比如  SOCK——DGRAM */
	myhint.ai_flags = AI_PASSIVE;                    // flags 的标志很多  。常用的有AI_CANONNAME;
	myhint.ai_protocol = 0;                               /*  设置协议  一般为0，默认 */
	myhint.ai_addrlen = 0;                                /*  下面不可以设置，为0，或者为NULL  */
	myhint.ai_canonname = NULL;
	myhint.ai_addr = NULL;
	myhint.ai_next = NULL;
	char hostname[255] = { 0 };      //主机名  
	char *port = "100";			//端口号 
	int ilRc = getaddrinfo(hostname, port, &myhint, &ailist);    /*通过主机名获得地址信息*/
	if (ilRc < 0)
	{
		char str_error[100];
		strcpy_s(str_error, (char *)gai_strerror(errno));
		printf("str_error = %s", str_error);
		return 0;
	}
	if (ailist == NULL)
	{
		printf("sorry not find the IP address,please try again \n");
	}
	for (aip = ailist; aip != NULL; aip = aip->ai_next)                         /* 显示获取的信息  */
	{
		if (aip->ai_family == AF_INET6)
		{
			sinp6 = (struct sockaddr_in6 *)aip->ai_addr;                                  /* 为什么是for 循环 ，先向下看 */
			int i;
			//printf("ipv6 addr = ");
			for (i = 0; i < 16; i++)
			{
				if (((i - 1) % 2) && (i>0))
				{
					//printf(":");
				}
				//printf("%02x", sinp6->sin6_addr.u.Byte[i]);
			}
			//printf(" \n");
			//printf(" \n");
		}
	}
	return 0;
}

int fu_socket_send(int sockfd, const char* buf, int buflen)
{
	int size = 0;
	int totalLen = 0;
	while (0 != buflen){
		if ((size = ::send(sockfd, buf, buflen, 0)) <= 0){
			break;
		}
		buf += size;
		buflen -= size;
		totalLen += size;
	}
	if (size < 0)
	{
		return size;
	}
	return totalLen;
}

int fu_socket_recv(int sockfd, std::string& recvStr)
{
	if (recvStr.length() > 0 && recvStr.length()<NET_BUFFER_MAXSIZE)
	{
		return recvStr.length();
	}
	int size = 0;
	char buf[NET_BUFFER_MAXSIZE] = {};
	size = ::recv(sockfd, buf, NET_BUFFER_MAXSIZE, 0);
	if (size <= 0)
	{
		return size;
	}
	else if (size <NET_BUFFER_MAXSIZE)
	{
		recvStr.append(buf, size);
		return recvStr.length();
	}	
	while (1)
	{
		size = ::recv(sockfd, buf, NET_BUFFER_MAXSIZE, 0);
		if (size <= 0) break;
#ifdef ENCRYPT_PROT_BUFFER
		size_t buflen = fu_unencrypt_stream_size(buf, size);
		if (buflen <= 0)
		{
			break;
		}
#endif
		recvStr.append(buf, size);
	}
#ifdef ENCRYPT_PROT_BUFFER
	int tolen = 0;
	char* pData = fu_unencrypt_stream(recvStr.c_str(), tolen);
	recvStr.clear();
	recvStr.assign(pData, tolen);
	delete pData;
#endif
	return recvStr.length();
}
