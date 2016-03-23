#ifndef __FU_UTILS_SERIALIZE_H__
#define __FU_UTILS_SERIALIZE_H__

#include "../Platform/Include.h"

//字符串序列,传入的%s必须是以'\0'结尾
std::string fu_string_format(const char* format, ...);
std::string fu_string_format(const char * format, va_list args);
char* fu_getfiledata(const char* name, int& datalen);

//加密,都会新建一个buffer来加密，返回新创建的
char* fu_encrypt_stream(const char* pData, int& iolen, int seed);
char* fu_unencrypt_stream(const char* pData, int& outLen);
size_t fu_unencrypt_stream_size(const char* pData, int datalen);

//字符串转换
int fu_gbk_to_utf8(const char * lpGBKStr, char * lpUTF8Str, int nUTF8StrLen);
int fu_utf8_to_gbk(const char * lpUTF8Str, char * lpGBKStr, int nGBKStrLen);
//string
//int
//unsigned int
//unsigned short
//long
//long long
//float
//double
//char
//unsigned char
//bool
class UtilsSerialize
{
public:
	UtilsSerialize();
	UtilsSerialize(const char* pData, int datalen);
	~UtilsSerialize();
	UtilsSerialize(const UtilsSerialize& data);
	UtilsSerialize& operator=(const UtilsSerialize& data);

	//导入
	void Input(const int& toData);
	void Input(const std::string& toData);
	void Input(const unsigned int& toData);
	void Input(const unsigned short& toData);
	void Input(const unsigned char& toData);
	void Input(const long& toData);
	void Input(const long long& toData);
	void Input(const float& toData);
	void Input(const double& toData);
	void Input(const char& toData);
	void Input(const bool& toData);

	//导出
	void Output(int& toData);
	void Output(std::string& toData);
	void Output(unsigned int& toData);
	void Output(unsigned short& toData);
	void Output(unsigned char& toData);
	void Output(long& toData);
	void Output(long long& toData);
	void Output(float& toData);
	void Output(double& toData);
	void Output(char& toData);
	void Output(bool& toData);

public:
	void  ResetString();
	void  ResetIterator();
	const char* getData()	{ return m_DataString.c_str(); }
	size_t	getLength()		{ return m_DataString.length(); }
private:
	std::string		m_DataString;
	size_t			m_iIterator;
};



#endif