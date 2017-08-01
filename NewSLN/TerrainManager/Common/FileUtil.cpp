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

	WIN32_FIND_DATAW ffd;

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

std::wstring GetStartDir()
{
	wchar_t wsStartDir[MAX_PATH + 1];
	int i;

	GetModuleFileNameW(NULL, wsStartDir, MAX_PATH + 1);
	i = lstrlenW(wsStartDir) - 1;
	while (i >= 0)
	{
		if (wsStartDir[i] == L'\\') break;
		else i--;
	}
	i++;
	wsStartDir[i] = L'\0';

	return std::wstring(wsStartDir);
}

std::wstring	ExtractFileDirectory(const std::wstring& in_Str)
{
	size_t pos = in_Str.find_last_of('/');
	if (pos == size_t(-1))
	{
		pos = in_Str.find_last_of('\\');
		if (pos == size_t(-1))
			return std::wstring();
	}
	return std::wstring(in_Str, 0, pos) + std::wstring(L"/");
}

bool DirectoryExists(const std::wstring& szPath)
{
	DWORD dwAttrib = GetFileAttributesW(szPath.c_str());

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
