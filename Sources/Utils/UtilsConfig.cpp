
#include "UtilsConfig.h"
#include "UtilsFunction.h"
#include "UtilsLog.h"
#include "UtilsSerialize.h"

UtilsConfig::UtilsConfig(std::string fileName)
	: _filePath(fileName)
	, _isCheckFile(false)
{
	if(fileName != "")	
	{
		_fileHandler.open(_filePath.c_str(), "r");
	}	
}
UtilsConfig::~UtilsConfig()
{
	flush();
	_fileDataList.clear();
}

void UtilsConfig::openFile(std::string fileName)
{
	_filePath = fileName;
	if (!_fileHandler.isOpen())
	{
		_fileHandler.open(fileName.c_str(), "r");
	}
	int len = _fileHandler.fileSize();
	char *fileData = new char[len];
	size_t rCount = _fileHandler.read(fileData, 1, len);
	if (rCount == len)
	{
		char* pDataIndex = (char*)fileData;
		char* ptr = pDataIndex;
		bool tempKey = false;
		bool tempValue = false;
		std::string keyName = "";
		std::string keyValue = "";
		bool tempLeft = false;
		bool tempRight = false;
		for (int i=0; i<(int)len; i++, pDataIndex++)
		{
			if(*pDataIndex == '[') 
			{
				ptr = pDataIndex;
				ptr++;
				tempLeft = true;
			}
			else if(!tempLeft)
			{
				continue;
			}
			if (tempLeft)
			{
				if(*pDataIndex == ']') 
				{
					tempRight = true;
				}
				else if(!tempRight)
				{
					continue;
				}
			}
			*pDataIndex = '\0';
			if (tempLeft && !_isCheckFile && !strncmp(ptr, _filePath.c_str(), _filePath.size()))
			{
				ptr = pDataIndex;
				ptr++;
				tempLeft = false;
				tempRight = false;
				_isCheckFile = true;
				continue;
			}
			if(!_isCheckFile) break;
			if (!tempKey) 
			{
				keyName = ptr;
				tempKey = true;
			}
			else if (!tempValue) 
			{
				keyValue = ptr;
				tempValue = true;
			}
			if (tempKey && tempValue)
			{
				_fileDataList[keyName] = keyValue;
				tempKey = false;
				tempValue = false;
			}
			ptr = pDataIndex;
			ptr++;
			tempLeft = false;
			tempRight = false;
		}
		
	}
	FU_SAFE_DELETE(fileData);
}
bool UtilsConfig::isFileExist()
{
	if(_filePath=="") return false;
	FILE *fp = fopen(_filePath.c_str(), "r");
	bool bRet = false;

	if (fp)
	{
		bRet = true;
		fclose(fp);
	}

	return bRet;	
}

void UtilsConfig::setStringKey(std::string keyName, std::string keyValue)
{
	if (keyName == "") return;
	_fileDataList[keyName] = keyValue;
}
std::string UtilsConfig::getStringKey(std::string keyName, std::string defaultValue)
{
	auto iter =  _fileDataList.find(keyName);
	if(iter != _fileDataList.end())
	{
		return iter->second;
	}
	setStringKey(keyName, defaultValue);
	return defaultValue;
}
void UtilsConfig::flush()
{
	FILE *fp = fopen(_filePath.c_str(), "w+");
	if (!fp)
	{
		std::string logStr = fu_string_format("UtilsConfig::flush failed! _filePath=[%s] not in!", _filePath.c_str());
		FUAssert(false, logStr);
	}
	fprintf(fp, "[%s]\n", _filePath.c_str());
	auto iter = _fileDataList.begin();
	for(; iter != _fileDataList.end(); iter++)
	{
		fprintf(fp, "[%s]=[%s]\n", iter->first.c_str(), iter->second.c_str());
	}
	fclose(fp);

}
