
#include "UtilsProjPath.h"
#include "../Utils/UtilsDir.h"

static std::vector<std::string> st_ProjectDirList;
static std::string	st_CurrentProjPath = "";

void UtilsProjPath::Init()
{
	Clear();

	UtilsDir toDir;
	std::string curPath = toDir.CurPath();
	if (curPath[curPath.length() - 1] != '/' && curPath[curPath.length() - 1] != '\\')
	{
		curPath += "/";
	}
	st_ProjectDirList.push_back(curPath);
	st_ProjectDirList.push_back(curPath + "Sources/");
	st_ProjectDirList.push_back(curPath + "config/");
	st_ProjectDirList.push_back(curPath + "Sources/config/");
	st_ProjectDirList.push_back(curPath + "../Sources/");
	st_ProjectDirList.push_back(curPath + "../Sources/config/");

	st_CurrentProjPath = curPath;
}

const std::vector<std::string>& UtilsProjPath::GetProjectPath()
{
	if (st_ProjectDirList.empty())
	{
		Init();
	}
	return st_ProjectDirList;
}

void UtilsProjPath::AddProjectPath(const std::string& path)
{
	if (st_ProjectDirList.empty())
	{
		Init();
	}
	st_ProjectDirList.push_back(path);
}

void UtilsProjPath::AppendToCurPath(const std::string& path)
{
	if (st_ProjectDirList.empty())
	{
		Init();
	}
	st_ProjectDirList.push_back(st_CurrentProjPath + path);
}

void UtilsProjPath::Clear()
{
	st_ProjectDirList.clear();
	st_CurrentProjPath = "";
}

