#pragma once

#include "AbstractConverter.h"

#include <queue>
#include <thread>
#include <set>
#include <thread>
#include <mutex>

class SoftwareHeightfieldConverter : public IAbstractHeightfieldConverter
{
public:

	SoftwareHeightfieldConverter();
	~SoftwareHeightfieldConverter();

	//@{ IAbstractHeightfieldConverter

	// Создать триангуляцию немедленно и дождаться готовности
	virtual void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation) override;

	// добавить/удалить listener
	virtual void	RegisterListener(HeightfieldConverterListener*) override;
	virtual void 	UnregisterListener(HeightfieldConverterListener*) override;

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно
	virtual void	AppendTriangulationTask(const SHeightfield* in_pHeightfield) override;

	//@}

private:

	bool threadFinished() const {
		return _threadFinished;
	}

	void setThreadFinished() {
		_threadFinished = true;
	}

	static void triangulationThreadFunc(SoftwareHeightfieldConverter*);
	
	bool processTriangulations();

	struct STriangulationTask
	{
		STriangulationTask(const SHeightfield& heightfield) : _heightfield(heightfield)
		{
		}

		SHeightfield	_heightfield;
		STriangulation	_triangulation;

		void	createTriangulation();
	};

	std::mutex									_listenersMutex;
	std::set<HeightfieldConverterListener*>		_setListeners;

	std::thread									_triangulationThread;
	bool										_threadFinished = false;

	std::queue<STriangulationTask>				_qTriangulationTasks;
	std::mutex									_triangulationsMutex;
};
