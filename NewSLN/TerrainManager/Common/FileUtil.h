#pragma once

#include <string>
#include <vector>

void GetFileListFromDirectory(const std::wstring& in_wsDirectory, std::vector<std::wstring>& out_vecFileNames);
std::wstring		ExtractFileDirectory(const std::wstring& in_Str);

std::wstring GetStartDir();
