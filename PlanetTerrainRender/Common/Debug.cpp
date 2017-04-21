#include "debug.h"
#include <strsafe.h>

#define FLOAT_EXCEPTIONS

void Trace(const wchar_t* in_sFormat, ...)
{
	va_list args;
	va_start(args, in_sFormat);
	static wchar_t pwstrTmp[10000];
	pwstrTmp[0] = 0;
	size_t dwBuf = sizeof(pwstrTmp) / sizeof(pwstrTmp[0]);
	unsigned Res = _vsnwprintf_s(pwstrTmp, 10000, dwBuf, in_sFormat, args);
	//assert_andrey_ex(Res < dwBuf, L"неправильный format string или получилась слишком длинная строка");
	pwstrTmp[dwBuf - 1] = 0;
	va_end(args);

	OutputDebugStringW( pwstrTmp );
}

std::string GetLastErrorString (LPCTSTR lpszFunction)
{ 
	std::string out_str;
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf);

    out_str = (LPCTSTR)lpDisplayBuf;

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);

	return out_str;
}

void EnableFloatExceptions()
{	
	/*ResetCoProcessor();
#ifdef FLOAT_EXCEPTIONS
	_control87(_EM_INEXACT | _EM_UNDERFLOW | _EM_DENORMAL, _MCW_EM);
	_MM_SET_EXCEPTION_MASK(_MM_MASK_INEXACT | _MM_MASK_UNDERFLOW | _MM_MASK_DENORM);		
#endif*/
}



void DisableFloatExceptions()
{	
	/*ResetCoProcessor();

#ifdef FLOAT_EXCEPTIONS
	_control87(_EM_INEXACT | _EM_UNDERFLOW | _EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID | _EM_DENORMAL, _MCW_EM);
	_MM_SET_EXCEPTION_MASK(_MM_MASK_INVALID | _MM_MASK_DIV_ZERO | _MM_MASK_DENORM | _MM_MASK_OVERFLOW | _MM_MASK_UNDERFLOW | _MM_MASK_INEXACT);
#endif*/
}



bool IsFloatExceptionsEnabled()
{
	/*const unsigned exceptionMask = _EM_INEXACT | _EM_UNDERFLOW | _EM_OVERFLOW | _EM_ZERODIVIDE | _EM_INVALID | _EM_DENORMAL;
	return (_statusfp() & _MCW_EM) != exceptionMask;*/
	return true;
}


void ResetCoProcessor()
{
//	_clear87();
//
//#ifndef WIN64
//	// set floating point precision
//	_control87(_PC_53, MCW_PC);
//#endif
//
//	// set rounding to near
//	_control87(_RC_NEAR, _MCW_RC);
//
//	_MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
//
//	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
}
