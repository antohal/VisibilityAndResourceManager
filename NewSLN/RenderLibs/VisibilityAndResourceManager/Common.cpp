#include "stdafx.h"
#include "common.h"

#include <sstream>

std::wstring GetStartDirectory()
{
	wchar_t lpstrStartDir[MAX_PATH];
	GetModuleFileNameW(NULL, lpstrStartDir, MAX_PATH);

	std::wstring strOut = std::wstring(lpstrStartDir);
	
	size_t pos = strOut.find_last_of(L'/');
	if (pos != std::wstring::npos)
		return std::wstring(strOut, 0, pos) + L"\\";

	pos = strOut.find_last_of(L'\\');
	if (pos != std::wstring::npos)
		return std::wstring(strOut, 0, pos) + L"\\";

	return strOut + L"\\";
}


std::string IPAddressToString (DWORD in_dwIP)
{
	WORD wLo = LOWORD(in_dwIP);
	WORD wHi = HIWORD(in_dwIP);

	BYTE b1 = LOBYTE(wLo);
	BYTE b2 = HIBYTE(wLo);
	BYTE b3 = LOBYTE(wHi);
	BYTE b4 = HIBYTE(wHi);

	std::ostringstream oss;
	oss << static_cast<int>(b1) << "." << static_cast<int>(b2) << "."
		<< static_cast<int>(b3) << "." << static_cast<int>(b4);

	return oss.str();
}

void ConvertIPToBytes (DWORD in_dwIP, BYTE out_pbBytes[4])
{
	WORD wLo = LOWORD(in_dwIP);
	WORD wHi = HIWORD(in_dwIP);

	out_pbBytes[0] = LOBYTE(wLo);
	out_pbBytes[1] = HIBYTE(wLo);
	out_pbBytes[2] = LOBYTE(wHi);
	out_pbBytes[3] = HIBYTE(wHi);
}

DWORD ConvertBytesToIP (BYTE in_pbBytes[4])
{
	return MAKELONG(MAKEWORD(in_pbBytes[0], in_pbBytes[1]), MAKEWORD(in_pbBytes[2], in_pbBytes[3]));
}

void replace_in_str (std::string& str, const std::string& oldStr, const std::string& newStr)
{
	size_t pos = 0;
	while((pos = str.find(oldStr, pos)) != std::string::npos)
	{
		str.replace(pos, oldStr.length(), newStr);
		pos += newStr.length();
	}
}

void replace_char_in_str (std::string& str, const char ch, const std::string& newStr)
{
	size_t pos = 0;
	while((pos = str.find(ch, pos)) != std::string::npos)
	{
		str.replace(pos, 1, newStr);
		pos += newStr.length();
	}
}

void erase_chars_in_str (std::string& str, const char ch)
{
	for (auto it = str.begin(); it != str.end();)
	{
		if (*it == ch)
			it = str.erase(it);
		else
			it++;
	}
}

std::string GetFormattedString (const std::string& strInput)
{
	std::string strOut = strInput;

	replace_char_in_str(strOut, '\'', "\"");
	replace_char_in_str(strOut, '\n', "\\n");
	replace_char_in_str(strOut, '\r', "\\r");
	replace_char_in_str(strOut, '\t', "\\t");

	return strOut;
}

std::string GetOnlyHexDigits(const std::string& str)
{
	std::string strOutStr = str;

	erase_chars_in_str(strOutStr, '{');
	erase_chars_in_str(strOutStr, '}');
	erase_chars_in_str(strOutStr, '-');

	return strOutStr;
}

std::string GetSafeSqlString (const std::string& strInput)
{
	std::string strOut = strInput;

	replace_char_in_str(strOut, '\'', "''");

	return strOut;

}
