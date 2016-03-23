#ifndef __FU_UTILS_PROJECTPATH_H__
#define __FU_UTILS_PROJECTPATH_H__

#include "../Platform/Include.h"

namespace UtilsProjPath
{
	void  Init();
	const std::vector<std::string>& GetProjectPath();
	void  AddProjectPath(const std::string& path);
	void  AppendToCurPath(const std::string& path);
	void  Clear();
};



#endif