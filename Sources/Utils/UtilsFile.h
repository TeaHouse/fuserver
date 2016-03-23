#ifndef __FU_UTILS_HDDFILE_H__
#define __FU_UTILS_HDDFILE_H__

#include "../Platform/Include.h"

//�򿪲���Ӳ���ϵ��ļ�
class UtilsFile
{
public:		
	UtilsFile();
	UtilsFile(UtilsFile &file);
	virtual ~UtilsFile();
		
	UtilsFile &operator=(UtilsFile &file);
		
	// open/close
	bool open(const char *name,const char *mode);
	bool openEx(const char *name, const char *mode);
	int close();
		
	bool isOpen()	{ return m_pFile != NULL; }
	// parameters
	const char *getName();
	int fileSize(); //��offset��end�Ĵ�С
		
	// stdio
	int eof();
	int getc();
	int seekSet(int offset);
	int seekEnd(int offset);
	int seekCur(int offset);
	int tell();
	int flush();
				
	// stream read/write
	size_t read(void *ptr,size_t size,size_t nmemb);
	size_t write(const void *ptr,size_t size,size_t nmemb);
public:
	void puts(const char* pstr);
	void putc(const char& toc);

private:
	FILE*		m_pFile;
	std::string		m_sFileName;
};

#endif /* __FILE_H__ */
