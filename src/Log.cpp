#include "stdafx.h"
#include "log.h"

#include <stdio.h>
#include <stdarg.h>


void LogMessage (const string& strFmt, ...)
{
    va_list ap;

	FILE* fp = fopen("visman.log", "a+");

	const char* szFmt = strFmt.c_str();
    va_start(ap, szFmt);
	vfprintf(fp, szFmt, ap);
	printf(szFmt, ap);
    va_end(ap);

	fclose(fp);
}
