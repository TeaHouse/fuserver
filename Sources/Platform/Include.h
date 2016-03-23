#ifndef __FU_PLATFORM_INCLUDE_H__
#define __FU_PLATFORM_INCLUDE_H__

//#define Define_PublishTest //发布测试版本
#define Define_Publish //发布版本

#ifdef Define_PublishTest
#include "../../libs/vld/include/vld.h"
#pragma comment(lib, "vld.lib")
#endif


#ifdef WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <Mswsock.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <time.h>
#pragma comment(lib, "WS2_32.lib")
#else
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <assert.h>

//一些跨平台后的类型
#ifndef WIN32
typedef void* HANDLE;
typedef unsigned long DWORD;
typedef int		SOCKET;
typedef void*	LPVOID;
#define WINAPI
#define INVALID_SOCKET  (int)(~0)
#define SOCKET_ERROR            (-1)
typedef true	TRUE;
typedef false	FALSE;
#endif

#include "../three-party/jsoncpp/json.h"

#define Json_FindKeyValue(root, keyname)  ((root).find((keyname), (keyname) + strlen(keyname)))


//////////////////////////////////////////////////////////////////////////
//以上是系统头
//以下是全局头文件
//
#include "../Utils/UtilsLog.h"
#include "../Utils/UtilsSerialize.h"
#include "../Utils/UtilsFunction.h"
#include "../Utils/UtilsTime.h"


#endif