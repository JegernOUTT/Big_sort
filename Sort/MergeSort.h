#include "stdafx.h"
#include <string.h>

#ifndef _MERGESORT
#define _MERGESORT

template<typename T>class MergeSort
{
private:
    FileAdapter *firstFile;
    FileAdapter *secondFile;
    FileAdapter *outputFile;
	int fileCount;
    T* firstFilePath;
    T* secondFilePath;
    T* outputFilePath;
    int comp(const T *i, const T *j, const T *tmp1, const T *tmp2);
	bool isMerged;
public:
    MergeSort(T* firstPath, T* secondPath, T* outputPath, int fileNum);
	void merge();
	~MergeSort(void);
};


template<typename T>
MergeSort<T>::MergeSort(T *firstPath, T *secondPath, T *outputPath, int fileNum)
    : firstFile(new FileAdapter(firstPath, "r, ccs=UTF-16LE")), secondFile(new FileAdapter(secondPath, "r, ccs=UTF-16LE")),
    outputFile(new FileAdapter(outputPath, "w, ccs=UTF-16LE")), fileCount(fileNum),
    firstFilePath(firstPath), secondFilePath(secondPath), outputFilePath(outputPath),
    isMerged(false)
{

}

template<typename T>
MergeSort<T>::~MergeSort(void)
{
    firstFile->~FileAdapter();
    secondFile->~FileAdapter();
    outputFile->~FileAdapter();
    firstFile = nullptr;
    secondFile = nullptr;
    outputFile = nullptr;

    FileAdapter::fileDelete(firstFilePath);
    FileAdapter::fileDelete(secondFilePath);

    T output[200];
    sprintf(output, "%s_tmp%d", outputFilePath, fileCount);
    FileAdapter::fileRename(outputFilePath, output);

    isMerged = true;
}

template<typename T>
void MergeSort<T>::merge()
{
    int compareResult = 0;
    T *firstStr = new T[1000];
    T *secondStr = new T[1000];
    T *errCh_1 = new T[1000];
    T *errCh_2 = new T[1000];

    while((compareResult != -999) && (errCh_1 != NULL || errCh_2 != NULL))
    {
        if (compareResult == 0)
        {
            errCh_1 = firstFile->readString(firstStr);
            errCh_2 = secondFile->readString(secondStr);
        }
        else if (compareResult > 0)
        {
            errCh_2 = secondFile->readString(secondStr);
        }
        else if (compareResult < 0)
        {
            errCh_1 = firstFile->readString(firstStr);
        }

        compareResult = comp(firstStr, secondStr, errCh_1, errCh_2);

        if (compareResult == 0)
        {
            outputFile->writeString(firstStr);
            outputFile->writeString(secondStr);
        }
        else if (compareResult > 0)
        {
            outputFile->writeString(secondStr);
        }
        else if (compareResult < 0)
        {
            outputFile->writeString(firstStr);
        }
        else if (compareResult == -999)
        {
            break;
        }
    }

    delete []firstStr;
    delete []secondStr;
    delete []errCh_1;
    delete []errCh_2;


}

template<typename T>
int MergeSort<T>::comp(const T *i, const T *j, const T *tmp1, const T *tmp2)
{
    if (tmp1 == NULL)
    {
        return 1;
    }
    else if (tmp2 == NULL)
    {
        return -1;
    }
    else if (tmp2 == NULL && tmp1 == NULL)
    {
        return -999;
    }
    else
    {
        return strcmp(i, j);
    }
}


#endif // !_MERGESORT



