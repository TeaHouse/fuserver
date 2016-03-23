#include "UtilsSerialize.h"
#include "UtilsFile.h"
#include "UtilsProjPath.h"


std::string fu_string_format(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	std::string toStr = fu_string_format(format, ap);
	va_end(ap);
	return toStr;
}

std::string fu_string_format(const char * format, va_list args)
{
	int toCharLen = strlen(format);
	if (toCharLen < 1024)
	{
		toCharLen = 1024;
	}
	else
	{
		toCharLen *= 2;
	}

	char* pBuf = NULL;

	do
	{
		pBuf = new char[toCharLen];
		int toSize = vsnprintf(pBuf, toCharLen, format, args);
		if (toSize < 0)
		{
			toCharLen *= 2;
			delete[] pBuf;
		}
		else
		{
			break;
		}
	} while (1);
	std::string toString;
	toString.assign(pBuf, strlen(pBuf));
	delete[] pBuf;
	return toString;
}

char* fu_encrypt_stream(const char* pData, int& iolen, int seed)
{
	int totalLen = iolen + 2 * sizeof(int);
	char* pEnData = new char[totalLen];
	int toEnLen = 0;
	memcpy(pEnData + toEnLen, &seed, sizeof(int));
	toEnLen += sizeof(int);
	memcpy(pEnData + toEnLen, &iolen, sizeof(int));
	toEnLen += sizeof(int);


	int toDoData = (((~seed >> 2) + 1) >> seed / 3) & 0x0007;
	//printf("----fu_new_encryption----1-=[%d]-\n", toDoData);

	if (toDoData > iolen)
	{
		toDoData = 2;
	}
	int leftPoint = iolen / toDoData;
	int encryKey = iolen;
	encryKey <<= leftPoint;

	memcpy(pEnData + toEnLen, pData, iolen);
	int size = 0;
	while (size <= int(iolen - sizeof(int)))
	{
		*(int*)(pEnData + toEnLen + size) += encryKey;
		size += sizeof(int);
	}
	iolen = totalLen;

	return pEnData;
}

char* fu_unencrypt_stream(const char* pData, int& outLen)
{
	int toSetPos = 0;
	int seed = *(int*)(pData + toSetPos);
	toSetPos += sizeof(int);
	int datalen = *(int*)(pData + toSetPos);
	toSetPos += sizeof(int);

	char* pEnData = new char[datalen + 1];

	memcpy(pEnData, pData + toSetPos, datalen);
	pEnData[datalen] = '\0';

	int toDoData = (((~seed >> 2) + 1) >> seed / 3) & 0x0007;

	if (toDoData > datalen)
	{
		toDoData = 2;
	}
	int leftPoint = datalen / toDoData;
	int encryKey = datalen;
	encryKey <<= leftPoint;

	int size = 0;
	while (size <= int(datalen - sizeof(int)))
	{
		*(int*)(pEnData + size) -= encryKey;
		size += sizeof(int);
	}
	outLen = datalen;
	return pEnData;
}

size_t fu_unencrypt_stream_size(const char* pData, int datalen)
{
	const int toSeedPos = sizeof(int);
	return *(int*)(pData + toSeedPos);
}

char* fu_getfiledata(const char* name, int& datalen)
{
	UtilsFile hFile;
	if (!hFile.open(name, "r")) return NULL;

	size_t fileSize = hFile.fileSize();
	char* pFileData = new char[fileSize + 1];
	hFile.read(pFileData, 1, fileSize);
	datalen = fileSize;
	return pFileData;
}
//GBK编码转换到UTF8编码
int fu_gbk_to_utf8(const char * lpGBKStr, char * lpUTF8Str, int nUTF8StrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if (!lpGBKStr)  //如果GBK字符串为NULL则出错退出
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_ACP, 0, lpGBKStr, strlen(lpGBKStr), NULL, NULL);  //获取转换到Unicode编码后所需要的字符空间长度
	lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
	nRetLen = ::MultiByteToWideChar(CP_ACP, 0, lpGBKStr, strlen(lpGBKStr), lpUnicodeStr, nRetLen);  //转换到Unicode编码
	if (!nRetLen)  //转换失败则出错退出
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, nRetLen, NULL, 0, NULL, NULL);  //获取转换到UTF8编码后所需要的字符空间长度

	if (!lpUTF8Str)  //输出缓冲区为空则返回转换后需要的空间大小
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return nRetLen;
	}

	if (nUTF8StrLen < nRetLen)  //如果输出缓冲区长度不够则退出
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_UTF8, 0, lpUnicodeStr, nRetLen, lpUTF8Str, nUTF8StrLen, NULL, NULL);  //转换到UTF8编码

	if (lpUnicodeStr)
		delete[]lpUnicodeStr;

	return nRetLen;
}

// UTF8编码转换到GBK编码
int fu_utf8_to_gbk(const char * lpUTF8Str, char * lpGBKStr, int nGBKStrLen)
{
	wchar_t * lpUnicodeStr = NULL;
	int nRetLen = 0;

	if (!lpUTF8Str)  //如果UTF8字符串为NULL则出错退出
		return 0;

	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, lpUTF8Str, -1, NULL, NULL);  //获取转换到Unicode编码后所需要的字符空间长度
	lpUnicodeStr = new WCHAR[nRetLen + 1];  //为Unicode字符串空间
	nRetLen = ::MultiByteToWideChar(CP_UTF8, 0, lpUTF8Str, -1, lpUnicodeStr, nRetLen);  //转换到Unicode编码
	if (!nRetLen)  //转换失败则出错退出
		return 0;

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, NULL, NULL, NULL, NULL);  //获取转换到GBK编码后所需要的字符空间长度

	if (!lpGBKStr)  //输出缓冲区为空则返回转换后需要的空间大小
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return nRetLen;
	}

	if (nGBKStrLen < nRetLen)  //如果输出缓冲区长度不够则退出
	{
		if (lpUnicodeStr)
			delete[]lpUnicodeStr;
		return 0;
	}

	nRetLen = ::WideCharToMultiByte(CP_ACP, 0, lpUnicodeStr, -1, lpGBKStr, nRetLen, NULL, NULL);  //转换到GBK编码

	if (lpUnicodeStr)
		delete[]lpUnicodeStr;

	return nRetLen;
}
//////////////////////////////////////////////////////////////////////////
UtilsSerialize::UtilsSerialize()
: m_DataString("")
, m_iIterator(0)
{

}

UtilsSerialize::UtilsSerialize(const char* pData, int datalen)
: m_DataString("")
, m_iIterator(0)
{
	m_DataString.assign(pData, datalen);
}

UtilsSerialize::UtilsSerialize(const UtilsSerialize& data)
{
	m_DataString.assign(data.m_DataString.c_str(), data.m_DataString.length());
	m_iIterator = data.m_iIterator;
}

UtilsSerialize::~UtilsSerialize()
{
	m_DataString = "";
}

UtilsSerialize& UtilsSerialize::operator=(const UtilsSerialize& data)
{
	m_DataString.assign(data.m_DataString.c_str(), data.m_DataString.length());
	m_iIterator = data.m_iIterator;

	return *this;
}

//////////////////////////////////////////////////////////////////////////

void UtilsSerialize::Input(const int& toData)
{
	m_DataString.append((char*)&toData, sizeof(int));
}

void UtilsSerialize::Input(const std::string& toData)
{
	m_DataString.append(toData.c_str(), toData.length());
	m_DataString.append(1, '\0');
}

void UtilsSerialize::Input(const unsigned int& toData)
{
	m_DataString.append((char*)&toData, sizeof(unsigned int));
}

void UtilsSerialize::Input(const unsigned short& toData)
{
	m_DataString.append((char*)&toData, sizeof(unsigned short));
}

void UtilsSerialize::Input(const unsigned char& toData)
{
	m_DataString.append((char*)&toData, sizeof(unsigned char));
}

void UtilsSerialize::Input(const long& toData)
{
	m_DataString.append((char*)&toData, sizeof(long));
}

void UtilsSerialize::Input(const long long& toData)
{
	m_DataString.append((char*)&toData, sizeof(long long));
}

void UtilsSerialize::Input(const float& toData)
{
	m_DataString.append((char*)&toData, sizeof(float));
}

void UtilsSerialize::Input(const double& toData)
{
	m_DataString.append((char*)&toData, sizeof(double));
}

void UtilsSerialize::Input(const char& toData)
{
	m_DataString.append((char*)&toData, sizeof(char));
}

void UtilsSerialize::Input(const bool& toData)
{
	m_DataString.append((char*)&toData, sizeof(bool));
}

//////////////////////////////////////////////////////////////////////////
void UtilsSerialize::Output(int& toData)
{
	toData = *(int*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(int);
}

void UtilsSerialize::Output(std::string& toData)
{
	toData = (m_DataString.c_str() + m_iIterator);
	m_iIterator += toData.length() + 1;
}

void UtilsSerialize::Output(unsigned int& toData)
{
	toData = *(unsigned int*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(unsigned int);
}

void UtilsSerialize::Output(unsigned short& toData)
{
	toData = *(unsigned short*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(unsigned short);
}

void UtilsSerialize::Output(unsigned char& toData)
{
	toData = *(unsigned char*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(unsigned char);
}

void UtilsSerialize::Output(long& toData)
{
	toData = *(long*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(long);
}

void UtilsSerialize::Output(long long& toData)
{
	toData = *(long long*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(long long);
}

void UtilsSerialize::Output(float& toData)
{
	toData = *(float*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(float);
}

void UtilsSerialize::Output(double& toData)
{
	toData = *(double*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(double);
}

void UtilsSerialize::Output(char& toData)
{
	toData = *(char*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(char);
}

void UtilsSerialize::Output(bool& toData)
{
	toData = *(bool*)(m_DataString.c_str() + m_iIterator);
	m_iIterator += sizeof(bool);
}

void UtilsSerialize::ResetString()
{
	m_iIterator = 0;
	m_DataString = "";
}

void UtilsSerialize::ResetIterator()
{
	m_iIterator = 0;
}
