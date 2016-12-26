#include "stdafx.h"
#include "log.h"

#include <stdio.h>
#include <stdarg.h>

static bool g_enabled = true;
static bool g_inited = false;

void LogInit()
{
	FILE* fp = fopen("resman.log", "w");
	fclose(fp);
}

void LogEnable(bool enable/* = true*/)
{
	g_enabled = enable;
}

void LogMessage (const char* strFmt, ...)
{
	if (!g_inited)
	{
		LogInit();
		g_inited = true;
	}

	if (!g_enabled)
		return;

    va_list ap;

	FILE* fp = fopen("resman.log", "a+");

    va_start(ap, strFmt);
	vfprintf(fp, strFmt, ap);
	fprintf(fp, "\n");
	vprintf(strFmt, ap);
	printf("\n");
    va_end(ap);

	fclose(fp);
}
