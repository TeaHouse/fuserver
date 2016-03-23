
#include "UtilsLog.h"
#include "UtilsLock.h"
#include "UtilsTime.h"
#include "UtilsFile.h"
#include "UtilsSerialize.h"
#include "UtilsThread.h"

//////////////////////////////////////////////////////////////////////////
struct CLogInfo
{
	CLogInfo(const std::string& log, int level)
	: m_slog(log)
	, m_ilevel(level)
	, m_sFile("")
	{}
	CLogInfo(const std::string& log, const std::string& file, int level)
		: m_slog(log)
		, m_sFile(file)
		, m_ilevel(level)
	{}

	std::string		m_slog;
	std::string		m_sFile;
	int		m_ilevel;
};

UtilsLock	g_LogLock;
bool	g_LogExit = false;
bool	g_LogWriteToFile = false;
bool	g_bInit = false;
int		g_LogFileNum = 0;
std::list<CLogInfo*>	g_LogInfoList;
UtilsThread		g_hThread;

void st_WriteFile(const std::string& logStr)
{
	std::string fileName;
	fileName = fu_string_format("UtilsLog%d.log", g_LogFileNum);
	UtilsFile	hFileHandler;
	hFileHandler.open(fileName.c_str(), "a");
	
	int leng = hFileHandler.fileSize();
	if (leng >= UtilsLog::ONEFILE_MAXSIZE)
	{
		g_LogFileNum++;
		fileName = fu_string_format("UtilsLog%d.log", g_LogFileNum);
		hFileHandler.open(fileName.c_str(), "a");
	}

	hFileHandler.puts(logStr.c_str());
	hFileHandler.close();
}
void st_WriteFile(const std::string& logStr, const std::string& file)
{
	UtilsFile	hFileHandler;
	hFileHandler.open(file.c_str(), "a");
	hFileHandler.puts(logStr.c_str());
	hFileHandler.close();
}

void CheckLogInfoFunc(CLogInfo* pLogInfo)
{
	std::string myLogStr = "";
	
	if (pLogInfo->m_ilevel<=1)
	{
		myLogStr = "[FULOG]";
	}
	else if (pLogInfo->m_ilevel== 2)
	{
		myLogStr = "[FUWarning]";
	}
	else if (pLogInfo->m_ilevel >= 3)
	{
		myLogStr = "[FUError]";
	}
	myLogStr += pLogInfo->m_slog;
	if (myLogStr.at(myLogStr.size() - 1) != '\n')
	{
		myLogStr += '\n';
	}
	if (pLogInfo->m_sFile != "")
	{
		st_WriteFile(myLogStr, pLogInfo->m_sFile);
	}
	else if (g_LogWriteToFile)
	{
		st_WriteFile(myLogStr);
	}
	else
	{
		printf("%s", myLogStr.c_str());
	}
}

void* st_WorkerThreadProc(void* lpParam)
{
	unsigned long  outPutLine = 0;
	CLogInfo* pLogInfo = NULL;
	g_LogExit = false;
	while (1)
	{
		if (!g_LogInfoList.empty())
		{
			g_LogLock.Lock();
			pLogInfo = g_LogInfoList.front();
			g_LogInfoList.pop_front();
			g_LogLock.unLock();
			CheckLogInfoFunc(pLogInfo);
			delete pLogInfo;
			outPutLine++;

		}
		else if (g_LogExit)
		{
			break;
		}
		UtilsTime::SleepUsec(1);
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
bool UtilsLog::Init()
{
	g_bInit = true;
	g_LogExit = false;
	return g_hThread.Create(st_WorkerThreadProc, NULL);
}

void UtilsLog::LogFile(const std::string& pstr, const std::string& fileName)
{
	if (!g_bInit) UtilsLog::Init();
#ifdef Define_PublishTest
	std::cout << pstr << std::endl;
#else
	g_LogLock.Lock();
	g_LogInfoList.push_back(new CLogInfo(pstr, fileName, 0));
	g_LogLock.unLock();
#endif
}

void UtilsLog::Exit()
{
	g_LogExit = true;
	g_hThread.Destroy(true);
}

void UtilsLog::Log(int level, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	std::string toStr = fu_string_format(format, args);
	va_end(args);

	if (!g_bInit) UtilsLog::Init();
#ifdef Define_PublishTest
	std::cout << toStr << std::endl;
#else
	g_LogLock.Lock();
	g_LogInfoList.push_back(new CLogInfo(toStr, level));
	g_LogLock.unLock();
#endif
}

void UtilsLog::SetWriteFile(bool flag)
{
	g_LogWriteToFile = !g_LogWriteToFile;
}
