#include "FileUtil.h"
#include "Log.h"
#include "StlUtil.h"

#include <strsafe.h>
#include <Windows.h>

void GetFileListFromDirectory(const std::wstring& in_wsDirectory, std::vector<std::wstring>& out_vecFileNames)
{
	std::wstring wsDirectory = in_wsDirectory;

	if (wsDirectory.empty())
		return;

	wchar_t backChar = wsDirectory.back();

	if (backChar == L'\\' || backChar == L'/')
		wsDirectory = wsDirectory.substr(0, wsDirectory.size() - 1);

	std::wstring wsSearchRequest = wsDirectory + L"\\*";

	WIN32_FIND_DATA ffd;

	HANDLE hFind = INVALID_HANDLE_VALUE;

	hFind = FindFirstFileW(wsSearchRequest.c_str(), &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		LogMessage("CTerrainBlockDesc::LoadChildsFromDirectory('%ls'), ERROR: Directory is empty.", wsDirectory.c_str());
		return;
	}

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		std::wstring wsFileName = ffd.cFileName;
		
		out_vecFileNames.push_back(wsDirectory + L"\\" + wsFileName);
	} 
	while (FindNextFileW(hFind, &ffd) != 0);
}
