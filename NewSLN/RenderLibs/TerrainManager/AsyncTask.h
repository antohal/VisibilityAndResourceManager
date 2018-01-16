#pragma once

#include <thread>
#include <functional>
#include <mutex>
#include <list>

class AsyncTaskManager
{
public:

	AsyncTaskManager();
	~AsyncTaskManager();

	void appendTask(unsigned long long ID, const std::function<void()>& taskFunc);
	void removeTask(unsigned long long ID, bool in_bWaitFinish = false);
	size_t tasksCount() const;

private:

	void threadFunc();

	struct AsyncTask
	{
		unsigned long long		ID = -1;
		std::function<void()>	func;

		AsyncTask() {}
		AsyncTask(unsigned long long in_ID, const std::function<void()>& in_taskFunc) : ID(in_ID), func(in_taskFunc) {}
	};

	std::list<AsyncTask>		_taskList;
	mutable std::mutex			_tasksMutex;
	unsigned long long			_currentTaskRunning = -1;

	std::thread*				_taskManagementThread = nullptr;
	bool						_finished = false;
};
