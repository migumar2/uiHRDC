// Test7Zip.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../Lib7Zip/lib7zip.h"
#include <iostream>

#ifdef _WIN32
int _tmain(int argc, _TCHAR* argv[])
#else
int main(int argc, char * argv[])
#endif
{
	C7ZipLibrary lib;

	if (!lib.Initialize())
	{
		printf("initialize fail!\n");
		return 1;
	}

	WStringArray exts;

	if (!lib.GetSupportedExts(exts))
	{
		printf("get supported exts fail\n");
		return 1;
	}

	size_t size = exts.size();

	for(size_t i = 0; i < size; i++)
	{
		wstring ext = exts[i];

		for(size_t j = 0; j < ext.size(); j++)
		{
			printf("%c", (char)(ext[j] &0xFF));
		}

		printf("\n");
	}

	//lib.Extract(L"D:\\eBooks\\С˵\\The-Bourne-Identity.rar");
	C7ZipArchive * pArchive = NULL;
	if (false)
	{
		unsigned int numItems = 0;

		pArchive->GetItemCount(&numItems);

		wprintf(L"%d\n", numItems);

		for(unsigned int i = 0;i < numItems;i++)
		{
			C7ZipArchiveItem * pArchiveItem = NULL;

			if (pArchive->GetItemInfo(i, &pArchiveItem))
			{
				wprintf(L"%d,%s,%d\n", pArchiveItem->GetArchiveIndex(),
					pArchiveItem->GetFullPath().c_str(),
					pArchiveItem->IsDir());
			}
		}
	}

	if (pArchive != NULL)
		delete pArchive;

	return 0;
}

