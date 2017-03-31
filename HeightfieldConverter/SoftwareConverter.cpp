#include "SoftwareConverter.h"
#include "vecmath.h"
#include <chrono>

using namespace std::chrono_literals;

SoftwareHeightfieldConverter::SoftwareHeightfieldConverter()
{
	_triangulationThread = std::thread(triangulationThreadFunc, this);
}

SoftwareHeightfieldConverter::~SoftwareHeightfieldConverter()
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

	return true;
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

	if (_heightfield.nCountX <= 1 || _heightfield.nCountY <= 1)
	{
		// TODO: log

		return;
	}

	for (unsigned int ly = 0; ly < _heightfield.nCountY - 1; ly++)
	{
		for (unsigned int lx = 0; lx < _heightfield.nCountX - 1; lx++)
		{
			unsigned int i0 = ly*_heightfield.nCountX + lx;
			unsigned int i1 = ly*_heightfield.nCountX + lx + 1;
			unsigned int i2 = (ly + 1)*_heightfield.nCountX + lx;
			unsigned int i3 = (ly + 1)*_heightfield.nCountX + lx + 1;

			_triangulation.vecIndexData.push_back(i3);
			_triangulation.vecIndexData.push_back(i1);
			_triangulation.vecIndexData.push_back(i0);

			_triangulation.vecIndexData.push_back(i2);
			_triangulation.vecIndexData.push_back(i3);
			_triangulation.vecIndexData.push_back(i0);
		}
	}

	float dx = _heightfield.fSizeX / (_heightfield.nCountX - 1);
	float dy = _heightfield.fSizeY / (_heightfield.nCountY - 1);

	_triangulation.vecVertexData.reserve(_heightfield.nCountX * _heightfield.nCountY);

	for (unsigned int i = 0; i < _heightfield.vecData.size(); i++)
	{
		unsigned int ly = i / _heightfield.nCountX;
		unsigned int lx = i - ly * _heightfield.nCountX;
		
		SVertex vtx;
		vtx.position.y = _heightfield.fMinHeight + (_heightfield.fMaxHeight - _heightfield.fMinHeight) * ((float)_heightfield.vecData[i] / 255.f);
		vtx.position.x = lx * dx - _heightfield.fSizeX * 0.5;
		vtx.position.z = ly * dy - _heightfield.fSizeX * 0.5;

		vtx.texture.x = (float)lx / (_heightfield.nCountX - 1);
		vtx.texture.y = (float)ly / (_heightfield.nCountY - 1);

		vtx.normal.x = 0;
		vtx.normal.y = 1;
		vtx.normal.z = 0;

		_triangulation.vecVertexData.push_back(vtx);
	}
}
