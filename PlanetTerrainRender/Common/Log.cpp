#include "Log.h"

#include <stdio.h>
#include <stdarg.h>

static bool g_enabled = false;
static bool g_inited = false;
static FILE* g_fp = NULL;

class CFileCloser
{
public:
	~CFileCloser()
	{
		if (g_fp)
		{
			fclose(g_fp);
			g_fp = NULL;
		}
	}

} g_FileCloser;

void LogInit(const char* fileName)
{
	FILE* fp = nullptr;
	fopen_s(&fp, fileName, "w");

	if (fp)
		fclose(fp);

	fopen_s(&g_fp, fileName, "a+");

	g_inited = true;
}

void LogEnable(bool enable/* = true*/)
{
	g_enabled = enable;
}

void LogMessage (const char* strFmt, ...)
{
	if (!g_inited)
	{
		LogInit("visresman.log");
	}

	if (/*!g_enabled ||*/ !g_fp)
		return;

    va_list ap;

    va_start(ap, strFmt);
	vfprintf(g_fp, strFmt, ap);
	fprintf(g_fp, "\n");
	vprintf(strFmt, ap);
	printf("\n");
    va_end(ap);
}
