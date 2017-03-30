#include "SoftwareConverter.h"

#include <chrono>

using namespace std::chrono_literals;

SoftwareHeightfieldConverter::SoftwareHeightfieldConverter()
{
	_triangulationThread = std::thread(triangulationThreadFunc, this);
}

SoftwareHeightfieldConverter::SoftwareHeightfieldConverter()
{
	setThreadFinished();
	_triangulationThread.join();
}

// Создать триангуляцию немедленно и дождаться готовности
void SoftwareHeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	STriangulationTask task(*in_pHeightfield);
	task.createTriangulation();
	*out_pTriangulation = task._triangulation;
}

// добавить/удалить listener
void SoftwareHeightfieldConverter::RegisterListener(HeightfieldConverterListener* in_pListener)
{
	std::lock_guard<std::mutex> lock(_listenersMutex);
	_setListeners.insert(in_pListener);
}

void SoftwareHeightfieldConverter::UnregisterListener(HeightfieldConverterListener* in_pListener)
{
	std::lock_guard<std::mutex> lock(_listenersMutex);
	_setListeners.erase(in_pListener);
}

void SoftwareHeightfieldConverter::triangulationThreadFunc(SoftwareHeightfieldConverter* self)
{
	while (!self->threadFinished())
	{
		if (!self->processTriangulations())
			std::this_thread::sleep_for(1ns);
	}
}

bool SoftwareHeightfieldConverter::processTriangulations()
{
	_triangulationsMutex.lock();
	if (_qTriangulationTasks.empty())
	{
		_triangulationsMutex.unlock();
		return false;
	}

	STriangulationTask task = _qTriangulationTasks.front();
	_qTriangulationTasks.pop();
	_triangulationsMutex.unlock();

	task.createTriangulation();

	std::lock_guard<std::mutex> lock(_listenersMutex);
	for (HeightfieldConverterListener* listener : _setListeners)
		listener->TriangulationCreated(&task._triangulation);
}

// добавить задачу на триангуляцию, которая будет выполняться асинхронно
void SoftwareHeightfieldConverter::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
	std::lock_guard<std::mutex> lock(_triangulationsMutex);
	_qTriangulationTasks.push(STriangulationTask(*in_pHeightfield));
}

void SoftwareHeightfieldConverter::STriangulationTask::createTriangulation()
{
	_triangulation.ID = _heightfield.ID;

	_finished = true;
}
