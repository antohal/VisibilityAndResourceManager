#pragma once

#include <string>
#include <vector>
#include <set>

void TrimSpaces(std::wstring& inout_Str);
void SplitByStringSeparatedList(const std::wstring& in_sList, const std::vector<std::wstring>& in_vecSeparaters, std::vector<std::wstring>& out_vecResult);
void SplitByStringSeparatedList(const std::wstring& in_sList, const std::wstring& in_sSeparator, std::vector<std::wstring>& out_vecResult);
std::wstring FormatString(const wchar_t* in_strFormat, ...);
std::string WideToAnsi(const std::wstring& in_wsWide);
std::wstring AnsiToWide(const std::string& in_sAnsi);

template <typename T> 
inline void ConvertSetToVector(const std::set<T>& setContainer, std::vector<T>& out_vec)
{
	out_vec.resize(setContainer.size());

	size_t nCount = 0;
	for (const T& element : setContainer)
	{
		out_vec[nCount] = element;
		nCount++;
	}
}
