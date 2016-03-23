
#include "UtilsFile.h"
#include "UtilsProjPath.h"

UtilsFile::UtilsFile() 
: m_pFile(nullptr)
, m_sFileName("")
{
}

UtilsFile::UtilsFile(UtilsFile &file) {
	m_pFile = file.m_pFile;
	m_sFileName = file.m_sFileName;
	m_pFile = nullptr;
	m_sFileName = nullptr;
}

UtilsFile::~UtilsFile() 
{
	close();
}

UtilsFile &UtilsFile::operator=(UtilsFile &file) {
	if(this == &file) return *this;

	m_pFile = file.m_pFile;
	m_sFileName = file.m_sFileName;
	m_pFile = nullptr;
	m_sFileName = nullptr;
	return *this;
}

bool UtilsFile::open(const char *name,const char *mode) {
	close();
	if (strncmp(mode, "r", strlen(mode)) == 0)
	{
		const std::vector<std::string>& projPathList = UtilsProjPath::GetProjectPath();
		for (auto& iter : projPathList)
		{
			std::string toName = iter + name;
			if (openEx(toName.c_str(), mode))
			{
				return true;
			}
		}
		return false;
	}
	return openEx(name, mode);
}

bool UtilsFile::openEx(const char *name, const char *mode)
{
#ifdef _WIN32
	wchar_t name_wbuf[1024];
	wchar_t mode_wbuf[64];
	MultiByteToWideChar(CP_UTF8, 0, name, -1, name_wbuf, sizeof(name_wbuf) / sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, mode, -1, mode_wbuf, sizeof(mode_wbuf) / sizeof(wchar_t));
	name_wbuf[sizeof(name_wbuf) / sizeof(wchar_t) - 1] = '\0';
	mode_wbuf[sizeof(mode_wbuf) / sizeof(wchar_t) - 1] = '\0';
	m_pFile = _wfopen(name_wbuf, mode_wbuf);
#elif _LINUX
	m_pFile = fopen(name, mode);
#elif _CELLOS_LV2
	char buf[1024];
	snprintf(buf, sizeof(buf), PATH_PREFIX"%s", name);
	buf[sizeof(buf) - 1] = '\0';
	m_pFile = fopen(buf, mode);
#endif
	if (m_pFile) {
		m_sFileName = name;
		return true;
	}
	return false;
}

int UtilsFile::close() {
	//assert(m_pFile != NULL && "UtilsFile::close(): file is not opened");
	if (m_pFile == NULL)	return 0;
	flush();
	int ret = fclose(m_pFile);
	m_sFileName = "";
	m_pFile = nullptr;
	return ret;
}

const char *UtilsFile::getName() {
	return m_sFileName.c_str();
}

int UtilsFile::fileSize() {
	int offset = tell();
	seekEnd(0);
	int size = tell();
	seekSet(offset);
	return size;
}

int UtilsFile::eof() {
	return feof(m_pFile);
}

int UtilsFile::getc() {
	return fgetc(m_pFile);
}

int UtilsFile::seekSet(int offset) {
	return (fseek(m_pFile, (long)offset, SEEK_SET) == 0);
}

int UtilsFile::seekEnd(int offset) {
	return (fseek(m_pFile, (long)offset, SEEK_END) == 0);
}

int UtilsFile::seekCur(int offset) {
	return (fseek(m_pFile, (long)offset, SEEK_CUR) == 0);
}

int UtilsFile::tell() {
	return ftell(m_pFile);
}

int UtilsFile::flush() {
	return fflush(m_pFile);
}

size_t UtilsFile::read(void *ptr,size_t size,size_t nmemb) {
	return fread(ptr,size,nmemb,m_pFile);
}

size_t UtilsFile::write(const void *ptr,size_t size,size_t nmemb) {
	return fwrite(ptr,size,nmemb,m_pFile);
}

void UtilsFile::puts(const char* pstr)
{
	fputs(pstr, m_pFile);
}

void UtilsFile::putc(const char& toc)
{
	fputc(toc, m_pFile);
}
