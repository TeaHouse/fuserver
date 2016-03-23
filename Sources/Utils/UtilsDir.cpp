
#include "UtilsDir.h"
#include <direct.h>

UtilsDir::UtilsDir()
{

}

UtilsDir::~UtilsDir()
{

}

std::string UtilsDir::CurPath()
{
	char pBuf[255] = {};
	_getcwd(pBuf, sizeof(pBuf));
	return std::string(pBuf);
}

