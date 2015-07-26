#include "stdafx.h"

#ifndef _FILEADAPTER
#define _FILEADAPTER

typedef unsigned long long ull;

class FileAdapter
{
private:
	const int strSize;
	FILE *file;
	wchar_t *_wPath;
	wchar_t *_wFormat;
	char *_path;
	char *_format;
	bool isWide;
	
public:
	FileAdapter(wchar_t* path, wchar_t* format);
	FileAdapter(char* path, char* format);

	bool isOpened();
	
	char readChar();
	wchar_t readWChar(void);
	char* readString(char* buf);

	int writeChar(const char symbol);
	int writeString(const char* _string);

	char* getFilePath();
	ull getSize();

	static int fileRename(const char* fileOld, const char* fileNew)
	{
		return rename(fileOld, fileNew);
	}

    static int fileDelete(const char* fileName)
    {
        return remove(fileName);
    }

	~FileAdapter(void);
};


#endif // !_FILEADAPTER





