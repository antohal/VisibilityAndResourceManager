#include "StlUtil.h"

#include <algorithm>

#include <Windows.h>

struct SEqualCharSeparatorResult
{
	SEqualCharSeparatorResult(): m_dwEnd(-1), m_dwSepLength(-1)
	{ }

	size_t	m_dwEnd;
	size_t	m_dwSepLength;
};

struct SEqualCharSeparator
{
	SEqualCharSeparator(const std::wstring& in_sList, size_t in_dwStartFind, SEqualCharSeparatorResult& in_Result): 
		m_sList(in_sList), m_dwStartFind(in_dwStartFind), m_Result(in_Result)
	{
	}

	bool operator()(const std::wstring& in_strSep)
	{
		m_Result.m_dwEnd = m_sList.find( in_strSep, m_dwStartFind );
		if( m_Result.m_dwEnd != size_t(-1) )
		{
			m_Result.m_dwSepLength = in_strSep.length();
			return true;
		}
		return false;
	}

	const std::wstring&	m_sList;
	size_t			m_dwStartFind;

	SEqualCharSeparatorResult&	m_Result;
};


void TrimSpaces(std::wstring& inout_Str)
{
	size_t pos_first = inout_Str.find_first_not_of(L" \t");
	size_t pos_last = inout_Str.find_last_not_of(L" \t");
	if (pos_first == -1) //полностью из пробелов
		inout_Str = std::wstring();
	else
		inout_Str.assign(inout_Str, pos_first, pos_last - pos_first + 1);
}

void SplitByStringSeparatedList(const std::wstring& in_sList, const std::vector<std::wstring>& in_vecSeparaters, std::vector<std::wstring>& out_vecResult)
{
	if( in_sList.empty() )
		return;

	size_t dwListLen = in_sList.length();
	size_t dwBegin = 0;

	while(dwBegin < dwListLen)
	{
		size_t dwPieceLen = 0;

		SEqualCharSeparatorResult SepRes;
		SEqualCharSeparator EqualCharSeparator(in_sList, dwBegin, SepRes);
		find_if(in_vecSeparaters.begin(), in_vecSeparaters.end(), EqualCharSeparator);

		size_t dwEnd = SepRes.m_dwEnd;

		if(dwEnd > dwBegin)
		{
			if( dwEnd == size_t(-1) )
			{
				dwPieceLen = dwListLen - dwBegin;
				SepRes.m_dwSepLength = 0;
			}
			else
				dwPieceLen = dwEnd - dwBegin;

			std::wstring sTmp = in_sList.substr(dwBegin, dwPieceLen);
			TrimSpaces(sTmp);
			out_vecResult.push_back(sTmp);

			dwBegin += dwPieceLen + SepRes.m_dwSepLength;
		}
		else
			dwBegin++;
	}
}

void SplitByStringSeparatedList(const std::wstring& in_sList, const std::wstring& in_sSeparator, std::vector<std::wstring>& out_vecResult)
{
	if( in_sList.empty() )
		return;

	size_t dwSeparatorLen = in_sSeparator.length();
	size_t dwListLen = in_sList.length();
	size_t dwBegin = 0;

	while(dwBegin < dwListLen)
	{
		size_t dwPieceLen = 0;

		size_t dwEnd = in_sList.find( in_sSeparator, dwBegin );

		if(dwEnd > dwBegin)
		{
			if( dwEnd == size_t(-1) )
				dwPieceLen = dwListLen - dwBegin;
			else
				dwPieceLen = dwEnd - dwBegin;

			std::wstring sTmp = in_sList.substr(dwBegin, dwPieceLen);
			TrimSpaces(sTmp);
			out_vecResult.push_back(sTmp);

			dwBegin += dwPieceLen + dwSeparatorLen;
		}
		else
			dwBegin++;
	}
}

std::string WideToAnsi(const std::wstring& in_wsWide)
{
	if (in_wsWide.empty())
		return std::string();


	int iLen = WideCharToMultiByte(CP_ACP, 0, &in_wsWide[0], (int)in_wsWide.length(), 0, 0, NULL, NULL);
	if (iLen >= 0)
	{
		std::string sAnsi;
		sAnsi.resize(static_cast<size_t>(iLen), 0);

		WideCharToMultiByte(CP_ACP, 0, &in_wsWide[0], (int)in_wsWide.length(), &sAnsi[0], iLen, NULL, NULL);

		return sAnsi;
	}

	return std::string();
}

std::wstring AnsiToWide(const std::string& in_sAnsi)
{
	if(in_sAnsi.empty())
		return std::wstring();

	int iLen = MultiByteToWideChar(CP_ACP, 0, &in_sAnsi[0], (int)in_sAnsi.length(), 0, 0);

	if (iLen >= 0)
	{
		std::wstring wsWide;
		wsWide.resize(static_cast<size_t>(iLen), 0);

		MultiByteToWideChar(CP_ACP, 0, &in_sAnsi[0], (int)in_sAnsi.length(), &wsWide[0], iLen);

		return wsWide;
	}

	return std::wstring();
}

std::wstring FormatString(const wchar_t* in_pwstrFormat, ...)
{
	va_list args;
	va_start(args, in_pwstrFormat);
	wchar_t pwstrTmp[10000];
	pwstrTmp[0] = 0;
	size_t dwBuf = sizeof(pwstrTmp) / sizeof(pwstrTmp[0]);
	unsigned Res = _vsnwprintf_s(pwstrTmp, 10000, dwBuf, in_pwstrFormat, args);
	
	pwstrTmp[dwBuf - 1] = 0;
	va_end(args);
	return std::wstring(pwstrTmp);
}


template <typename stringT> void TrimSpaces(stringT& inout_Str);

template <> void TrimSpaces<std::string> (std::string& inout_Str)
{
	size_t pos_first = inout_Str.find_first_not_of(" \t");
	size_t pos_last = inout_Str.find_last_not_of(" \t");
	if (pos_first == -1) //полностью из пробелов
		inout_Str = std::string();
	else
		inout_Str.assign(inout_Str, pos_first, pos_last - pos_first + 1);
}

template <> void TrimSpaces<std::wstring> (std::wstring& inout_Str)
{
	size_t pos_first = inout_Str.find_first_not_of(L" \t");
	size_t pos_last = inout_Str.find_last_not_of(L" \t");
	if (pos_first == -1) //полностью из пробелов
		inout_Str = std::wstring();
	else
		inout_Str.assign(inout_Str, pos_first, pos_last - pos_first + 1);
}
