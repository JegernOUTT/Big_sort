#include "stdafx.h"

typedef unsigned long long ull;
const ull MAX_SIZE = 1000000000000;

using namespace std;

void sortInputFile(FileAdapter* inputFile, char* outFileName);
void arrayMerge(char* outFileName);
int renamePaths(char* outFileName);
void renameLastFile(char* outFileName);

int threadsCount = 0, blockCount = 0, iteration = 0, 
	strategy = 1;
ull fileSize = 0, blockSize = 0;
vector<QuickSort<char>*> qsArr;

ull getBlockSize(ull ram)
{
	return (ram / threadsCount);
}

int getBlockCount()
{
	return int(fileSize / blockSize) + 1;
}

ull recountRam(ull ram)
{
    ram -= ram / (MIN_SYMBOLS_COUNT * sizeof(char)) * sizeof(char*);
	return ram;
}

ull fsize(FILE *fp)
{
    ull prev = ftello64(fp);
    fseeko64(fp, 0ull, SEEK_END);
    ull sz = ftello64(fp);
    fseeko64(fp, prev, SEEK_SET);
    return sz;
}

void* threadSort(void *val)
{
    QuickSort<char>* qs = (QuickSort<char>*)val;
    qs->sort();
    return val;
}

int compare(const void *i, const void *j)
{
    char* ch1 = *(char**)i;
    char* ch2 = *(char**)j;
    char str1[500];
    char str2[500];

    int k = 0;
    while(ch1[k] != L'\n')
    {
        str1[k] = ch1[k];
        str2[k] = ch2[k];
        k++;
    }
    str1[k] = L'\n';
    str2[k] = L'\n';

    return strcmp(str1, str2);
}

int main(int argc, char *argv[])
{
    char *inpFileName = new char[500];
    char *outFileName = new char[500];
    ull ram = 0;

    threadsCount = 4;

    if (argc == 4)
    {
        sprintf(inpFileName, "%s", argv[1]);
        sprintf(outFileName, "%s", argv[2]);
        ram = atoll(argv[3]);
    }
    else
    {
        printf("Parameters is not valid\n");
        printf("1st parameter: input file path. Current: %s \n", argv[1]);
        printf("2nd parameter: output file path. Current: %s \n", argv[2]);
        printf("3rd parameter: RAM in bytes. Current: %s \n", argv[3]);
        exit(0);
    }
    printf("Selected parameters:\n");
    printf("1st parameter: input file path. Current: %s \n", inpFileName);
    printf("2nd parameter: output file path. Current: %s \n", outFileName);
    printf("3rd parameter: RAM in bytes. Current: %lld \n", ram);

    if (ram < 128000000l)
    {
        printf("RAM is not enough\n");
        exit(-1);
    }

    FileAdapter inputFile(inpFileName, "r");

	if (!inputFile.isOpened())
	{
        printf("File not found\n");
        exit(-1);
	}
	fileSize = inputFile.getSize();

	if (fileSize == 0) 
    {
        printf("File size = 0\n");
        exit(-2);
    }
    else if (fileSize <= ram)
		strategy = 2;
	else if (fileSize >= MAX_SIZE)
    {
        printf("File is too big\n");
        exit(-3);
    }

	ram = recountRam(ram);
	
	blockSize = getBlockSize(ram);
	blockCount = getBlockCount();

	printf("Thread count: %d\n", threadsCount);
    printf("Block size: %lld\n", blockSize);
	printf("Block count: %d\n\n\n", blockCount);
	
	switch (strategy)
	{
	case 1:
		{
            sortInputFile(&inputFile, outFileName);
			inputFile.~FileAdapter();
            FileAdapter::fileDelete(inpFileName);		//!!!deleting original file
			arrayMerge(outFileName);
		}
		break;

	case 2:
		{
            QuickSort<char> qs(&inputFile, outFileName, 1);
			printf("Reading... \n");
			qs.copyToRam(fileSize);
			printf("Sorting... \n");
			threadSort(&qs);
			qs.save();
			printf("Saving... \n");
			renameLastFile(outFileName);
		}
		break;
	}
	
	printf("Operation is finished\n");
	getchar();

    exit(0);
}

void sortInputFile(FileAdapter* inputFile, char* outFileName)
{
	int blockCount_tmp = blockCount;

	while(blockCount_tmp > 0)
	{
		printf("\n\nBlock number: %d\n\n", blockCount_tmp);
		for (int i = 0; i < threadsCount; i++)
		{
			printf("Thread %d reading data\n", i);
            QuickSort<char> *qs = new QuickSort<char>(inputFile, outFileName, blockCount_tmp);
			if (qs->copyToRam(blockSize) == 0)
			{
                qsArr.push_back(qs);
				blockCount_tmp--;
			}
			else
			{
				blockCount_tmp--;
			}
		}

		printf("Sorting\n");
        vector<pthread_t> threads;
        auto iter = qsArr.begin();
		for (int i  = 0; i < qsArr.size(); i++)
        {
			if (&iter != nullptr)
			{
                pthread_t tid;

                int err = pthread_create(&tid, NULL, threadSort, (void*)(*iter));

                if (err == 0)
                    threads.push_back(tid);
				iter++;
			} else
			{
				iter++;
			}
		}
				
		for (int i = 0; i < qsArr.size(); i++)
		{
                pthread_join(threads[i], NULL);
		}

		for (int i = 0; i < qsArr.size(); i++)
		{
			if (&qsArr[i] != nullptr)
			{
				printf("Writing data\n");
				qsArr[i]->save();
				qsArr[i]->~QuickSort();
			}
		}

		for (int i = 0; i < qsArr.size(); i++)
		{
			qsArr[i] = nullptr;
		}

		qsArr.clear();
		threads.clear();
	}
}

void arrayMerge(char* outFileName)
{
	//start merge
	printf("\nMerging... \n");
	int p = renamePaths(outFileName);
    char **tmpArrPaths = new char*[blockCount];
			
	for (int i = 1; i <= p; i++)
	{
        tmpArrPaths[i] = new char[PATH_SIZE];
        sprintf(tmpArrPaths[i], "%s_tmp%d", outFileName, i);
	}

	int tmp_p = p;
			
	while (p > 1)
	{
		int k = 1;

		if (p % 2 == 1 && p != 1)
		{
            MergeSort<char> *mergeSort = new MergeSort<char>(tmpArrPaths[1], tmpArrPaths[2], outFileName, 1);
            printf("Merge %s and %s blocks\n", tmpArrPaths[1], tmpArrPaths[2]);
            mergeSort->merge();
            delete mergeSort;
			p--;
			for (int i = 2; i <= p; i++)
			{
                char oldName[PATH_SIZE], newName[PATH_SIZE];
                sprintf(oldName, "%s_tmp%d", outFileName, i + 1);
                sprintf(newName, "%s_tmp%d", outFileName, i);
				FileAdapter::fileRename(oldName, newName);
			}
		}

		for (int i = 1; i <= p - 1; i++)
		{
            MergeSort<char> *mergeSort = new MergeSort<char>(tmpArrPaths[i], tmpArrPaths[i + 1], outFileName, k);
            printf("Merge %s and %s blocks\n", tmpArrPaths[i], tmpArrPaths[i + 1]);
            mergeSort->merge();
            delete mergeSort;
			i++;
			k++;
		}
		p >>= 1;
	}
			
	for (int i = 1; i <= tmp_p; i++)
	{
		delete []tmpArrPaths[i];
	}
	delete []tmpArrPaths;

	renameLastFile(outFileName);
}

int renamePaths(char* outFileName)
{
	int pCount = 1, tmp = 0;
	for (int i = 1; i <= blockCount; i++)
	{
        char oldName[PATH_SIZE], newName[PATH_SIZE];
        sprintf(oldName, "%s_tmp%d", outFileName, i);
        sprintf(newName, "%s_tmp%d", outFileName, pCount);
		tmp = FileAdapter::fileRename(oldName, newName);
		if (tmp == 0)
		{
			pCount++;
		}
	}

	return --pCount;
}

void renameLastFile(char* outFileName)
{
    char oldName[PATH_SIZE];
    sprintf(oldName, "%s_tmp%d", outFileName, 1);
	FileAdapter::fileRename(oldName, outFileName);
}
