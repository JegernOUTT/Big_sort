#include "FileAdapter.h"

FileAdapter::FileAdapter(char* path, char* format) 
	: strSize(1000)
{
	this->_path = path;
	this->_format = format;
    this->file = fopen(this->_path, this->_format);
	this->isWide = false;
}

FileAdapter::~FileAdapter()
{
	fclose(this->file);
}

bool FileAdapter::isOpened()
{
	if (file == NULL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

char FileAdapter::readChar()
{
	return (char)fgetc(this->file);
}

char* FileAdapter::readString(char* buf)
{
	return fgets(buf, this->strSize, this->file);
}

char* FileAdapter::getFilePath()
{
	return this->_path;
}

ull FileAdapter::getSize()
{
    ull prev = ftello64(this->file);
    fseeko64(this->file, 0ull, SEEK_END);
    ull sz = ftello64(this->file);
    fseeko64(this->file, prev, SEEK_SET);
    return sz;
}

int FileAdapter::writeChar(const char symbol)
{
	return fputc(symbol, this->file);
}

int FileAdapter::writeString(const char* _string)
{
	return fputs(_string, this->file);
}
