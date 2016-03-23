#ifndef __FU_UTILS_CONFIG_H__
#define __FU_UTILS_CONFIG_H__

#include "../Platform/Include.h"
#include "UtilsFile.h"

class UtilsConfig
{
public:
	UtilsConfig(std::string fileName= "");
	~UtilsConfig();

	void openFile(std::string fileName);
	bool isFileExist();

	void setStringKey(std::string keyName, std::string keyValue);
	std::string getStringKey(std::string keyName, std::string defaultValue);

	void flush();
private:
	std::string readLine(unsigned char*& fileData);

private:
	UtilsFile		_fileHandler;
	std::string		_filePath;
	bool			_isCheckFile;
	std::unordered_map<std::string, std::string>		_fileDataList;
	
};




#endif