#pragma once

#include <string>
#include <functional>
#include <string>

void LogInit(const char* fileName);
void LogEnable(bool enable = true);
void LogMessage (const char*, ...);
void AddLogHandler(const std::function<void(const std::string&)>& handler);
void RemoveLogHandler();


