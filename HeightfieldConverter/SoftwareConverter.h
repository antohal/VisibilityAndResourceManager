#pragma once

#include "AbstractConverter.h"

#include <queue>
#include <thread>
#include <set>
#include <thread>
#include <mutex>

#include <atlbase.h>
#include <d3d11.h>

class SoftwareHeightfieldConverter : public IAbstractHeightfieldConverter
{
public:

	SoftwareHeightfieldConverter(ID3D11Device* in_pD3DDevice11);
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
		STriangulationTask(SoftwareHeightfieldConverter* owner, const SHeightfield& heightfield) : _owner(owner), _heightfield(heightfield)
		{
		}

		SHeightfield	_heightfield;
		STriangulation	_triangulation;

		std::vector<SVertex>		_vecVertexData;
		std::vector<unsigned int>	_vecIndexData;

		void	createTriangulation();
		void	createBuffers();

		SoftwareHeightfieldConverter*	_owner = nullptr;
	};

	std::mutex									_listenersMutex;
	std::set<HeightfieldConverterListener*>		_setListeners;

	std::thread									_triangulationThread;
	bool										_threadFinished = false;

	std::queue<STriangulationTask>				_qTriangulationTasks;
	std::mutex									_triangulationsMutex;

	CComPtr<ID3D11Device>						_ptrD3DDevice11;
};
