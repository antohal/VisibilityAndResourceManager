#pragma once

#include <string>

void LogInit(const char* fileName);
void LogEnable(bool enable = true);
void LogMessage (const char*, ...);
