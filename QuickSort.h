#include "stdafx.h"
#include "FileAdapter.h"
#include "string.h"
#define PATH_SIZE 200
#define MIN_SYMBOLS_COUNT 32

typedef unsigned long long ull;
extern int compare(const void *i, const void *j);

#ifndef _QUICKSORT
#define _QUICKSORT

template<typename T>
class QuickSort
{
private:
    T* firstElement;
	FileAdapter* readingFile;
	ull fpOffset;
    T** arr;
	ull arrCount;
    T* outputFileName;
	int blockCount;
public:
    QuickSort(FileAdapter *fp, T *outputFileName, int blockCount);
	int copyToRam(ull bufSize);
    T** getArr();
	ull getArrCount();
	void sort();
	void save();
	~QuickSort(void);
};


#include "QuickSort.h"
#include "stdafx.h"
#include <stdio.h>
#include <string.h>

template<typename T>
QuickSort<T>::QuickSort(FileAdapter *fp, T *outputFileName, int blockCount)
{
    this->readingFile = fp;
    this->outputFileName = outputFileName;
    this->blockCount = blockCount;
}

template<typename T>
ull QuickSort<T>::getArrCount()
{
    return arrCount;
}

template<typename T>
T** QuickSort<T>::getArr()
{
    return arr;
}

template<typename T>
QuickSort<T>::~QuickSort(void)
{
    free(firstElement);
    delete []arr;
}

template<typename T>
int QuickSort<T>::copyToRam(ull bufSize)
{
    ull lenght = 0, i = 0;
    T *err = new T[100];

    arr = new T*[bufSize / (MIN_SYMBOLS_COUNT * sizeof(T))];
    firstElement = (T*)malloc(bufSize + 3000000);		//3MB for reserve

    arr[i] = firstElement;

    while (arr[i] < (firstElement + (bufSize / sizeof(T))))
    {
        err = readingFile->readString(arr[i]);
        lenght = strlen(arr[i]);
        if (err != NULL)
        {
            i++;
            arr[i] = arr[i - 1] + lenght;
        }
        else
        {
            arr[i][0] = L'\n';
            i++;
            break;
        }
    }
    i--;

    arrCount = i;

    if (arrCount <= 1)
    {
        this->~QuickSort();
        return -1;
    }
    else
    {
        return 0;
    }
}



template<typename T>
void QuickSort<T>::sort()
{
    qsort(arr, arrCount, sizeof(T*), compare);
}

template<typename T>
void QuickSort<T>::save()
{
    T fileName[PATH_SIZE];
    sprintf(fileName, "%s_tmp%d", this->outputFileName, blockCount);

    FileAdapter file((T*)fileName, "w, ccs=UTF-16LE");

    T ch;
    int k;

    for (ull i = 0; i < arrCount; i++)
    {
        ch = 0;
        k = 0;
        while (arr[i][k] != L'\n')
        {
            file.writeChar(arr[i][k]);
            k++;
        }
        file.writeChar(L'\n');
    }
}



#endif // !_QUICKSORT




