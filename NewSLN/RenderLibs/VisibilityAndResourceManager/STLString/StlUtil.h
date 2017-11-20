#pragma once

void TrimSpaces(std::wstring& inout_Str);
void SplitByStringSeparatedList(const std::wstring& in_sList, const std::vector<std::wstring>& in_vecSeparaters, std::vector<std::wstring>& out_vecResult);
void SplitByStringSeparatedList(const std::wstring& in_sList, const std::wstring& in_sSeparator, std::vector<std::wstring>& out_vecResult);
wstring FormatString(const wchar_t* in_strFormat, ...);
std::string WideToAnsi(const std::wstring& in_wsWide);
std::wstring AnsiToWide(const std::string& in_sAnsi);

std::string GetFormattedString (const std::string& strInput);
std::string GetSafeSqlString (const std::string& strInput);
