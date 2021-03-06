#include "Log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <mutex>

static bool g_enabled = false;
static bool g_inited = false;
static FILE* g_fp = NULL;
std::string g_sFileName;
static std::function<void(const std::string&)> g_Handler;

std::mutex g_logMutex;

//
//class CFileCloser
//{
//public:
//	~CFileCloser()
//	{
//		if (g_fp)
//		{
//			fclose(g_fp);
//			g_fp = NULL;
//		}
//	}
//
//} g_FileCloser;

void LogInit(const char* fileName)
{
	std::lock_guard<std::mutex> lock(g_logMutex);

	FILE* fp = nullptr;
	fopen_s(&fp, fileName, "w");

	if (fp)
		fclose(fp);

	g_sFileName = fileName;

	//fopen_s(&g_fp, fileName, "a+");

	g_inited = true;
}

void LogEnable(bool enable/* = true*/)
{
	std::lock_guard<std::mutex> lock(g_logMutex);

	g_enabled = enable;
}

void LogMessage (const char* strFmt, ...)
{
	std::lock_guard<std::mutex> lock(g_logMutex);

	if (!g_inited)
	{
		LogInit("visresman.log");
	}

	if (!g_enabled)
		return;

	fopen_s(&g_fp, g_sFileName.c_str(), "a+");

	if (!g_fp)
		return;

    va_list ap;

    va_start(ap, strFmt);
	vfprintf(g_fp, strFmt, ap);
	fprintf(g_fp, "\n");
	vprintf(strFmt, ap);
	printf("\n");
    va_end(ap);

	if (g_Handler)
	{
		va_start(ap, strFmt);

		static char buff[512];

		vsnprintf_s<512>(buff, sizeof(buff), strFmt, ap);

		g_Handler(std::string(buff));

		va_end(ap);
	}

	fclose(g_fp);
	g_fp = NULL;
}

void AddLogHandler(const std::function<void(const std::string&)>& handler)
{
	std::lock_guard<std::mutex> lock(g_logMutex);

	g_Handler = handler;
}

void RemoveLogHandler()
{
	std::lock_guard<std::mutex> lock(g_logMutex);

	g_Handler = std::function<void(const std::string&)>();
}
