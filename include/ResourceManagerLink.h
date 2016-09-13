#pragma once

#ifdef ResourceManager_EXPORTS
#define RESOURCEMANAGER_API __declspec(dllexport)
#else
#define RESOURCEMANAGER_API __declspec(dllimport)
#endif
