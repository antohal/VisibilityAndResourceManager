#include "AsyncTask.h"
#include <chrono>

#include <Windows.h>

AsyncTaskManager::AsyncTaskManager()
{
	_taskManagementThread = new std::thread([this]() {threadFunc(); });
	SetThreadPriority(_taskManagementThread->native_handle(), THREAD_PRIORITY_LOWEST);
}

AsyncTaskManager::~AsyncTaskManager()
{
	_finished = true;

	if (_taskManagementThread->joinable())
		_taskManagementThread->join();	

	delete _taskManagementThread;
}

void AsyncTaskManager::appendTask(unsigned long long ID, const std::function<void()>& taskFunc)
{
	std::lock_guard<std::mutex> lg(_tasksMutex);
	_taskList.push_front(AsyncTask(ID, taskFunc));
}

size_t AsyncTaskManager::tasksCount() const
{
	std::lock_guard<std::mutex> lg(_tasksMutex);
	size_t count = _taskList.size();

	return count;
}

void AsyncTaskManager::removeTask(unsigned long long ID, bool in_bWaitFinish)
{
	std::lock_guard<std::mutex> lg(_tasksMutex);

	for (auto it = _taskList.begin(); it != _taskList.end();)
	{
		if (it->ID == ID)
			it = _taskList.erase(it);
		else
			it++;
	}

	if (in_bWaitFinish)
	{
		while (_currentTaskRunning == ID)
		{
			std::chrono::milliseconds dur(1);
			std::this_thread::sleep_for(dur);
		}
	}
}

void AsyncTaskManager::threadFunc()
{
	while (!_finished)
	{
		AsyncTask currentTask;
		bool tasksEmpty = false;

		{
			std::lock_guard<std::mutex> lg(_tasksMutex);
			if (_taskList.empty())
			{
				tasksEmpty = true;
			}
			else
			{
				currentTask = _taskList.back();
				_taskList.pop_back();
			}
		}

		if (tasksEmpty)
		{
			std::chrono::milliseconds dur(1);
			std::this_thread::sleep_for(dur);
		}
		else
		{
			_currentTaskRunning = currentTask.ID;
			currentTask.func();
			_currentTaskRunning = -1;
		}
	}
}
