#pragma once

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)

#define __WFILE__ WIDEN(__FILE__)
#define __WFUNCTION__ WIDEN(__FUNCTION__)

#include <string>
#include <windows.h>

void Trace(const wchar_t* in_sFormat, ...);

#define assert_release(expr, str) \
	if (!(expr)) { ::MessageBox(GetCOREInterface()->GetMAXHWnd(), str, "Assert!", MB_OK | MB_ICONEXCLAMATION); }

#ifdef ASSERTS
#define assert_debug(expr, message) \
	if(!(expr)) \
{ \
	static bool bDisabled = false;\
	std::wostringstream wossMessage;\
	wossMessage << L"assertion failed in " << __WFILE__ << L" at line " << __LINE__ << L": " << message << std::endl;\
	Trace(wossMessage.str().c_str()); \
	if (!bDisabled)\
	{\
		wossMessage << L"\r\n Retry - debug, Cancel - skip.";\
		if (IDCANCEL == ::MessageBoxW(0, wossMessage.str().c_str(), L"assertion failed", MB_RETRYCANCEL))\
		{\
			bDisabled = true;\
		}\
	if (!bDisabled)\
		__debugbreak();  \
	}\
}
#else
#define assert_debug(expr, message)
#endif

std::string GetLastErrorString (LPCTSTR lpszFunction);

void EnableFloatExceptions();
void DisableFloatExceptions();
bool IsFloatExceptionsEnabled();
void ResetCoProcessor();
