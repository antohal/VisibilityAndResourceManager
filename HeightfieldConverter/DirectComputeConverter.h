#pragma once

#include <atlbase.h>
#include <d3d11.h>

#include <mutex>
#include <thread>
#include <queue>
#include <set>

#include "AbstractConverter.h"

class DirectComputeHeightfieldConverter : public IAbstractHeightfieldConverter
{
public:

	DirectComputeHeightfieldConverter(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext);
	~DirectComputeHeightfieldConverter();

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

	struct STriangulationTask
	{
		STriangulationTask(DirectComputeHeightfieldConverter* owner, const SHeightfield& heightfield) : _owner(owner), _heightfield(heightfield)
		{
		}

		SHeightfield	_heightfield;
		STriangulation	_triangulation;

		void	createTriangulation();
		
		void	createInputBuffers();
		void	createOutputBuffers();

		struct ConstantBuffer
		{
			float fMinHeight;
			float fMaxHeight;
			float fSizeX;
			float fSizeY;

			unsigned int nCountX;
			unsigned int nCountY;

			float temp1;
			float temp2;
		};

		DirectComputeHeightfieldConverter*	_owner = nullptr;

		CComPtr<ID3D11UnorderedAccessView>	_ptrIndexBufferUAV;
		CComPtr<ID3D11UnorderedAccessView>	_ptrVertexBufferUAV;

		CComPtr<ID3D11Buffer>				_ptrInputBuffer;
		CComPtr<ID3D11ShaderResourceView>	_ptrInputSRV;

		CComPtr<ID3D11Buffer>				_ptrConstantBuffer;
	};

	std::mutex									_listenersMutex;
	std::set<HeightfieldConverterListener*>		_setListeners;

	std::thread									_triangulationThread;
	bool										_threadFinished = false;

	std::queue<STriangulationTask>				_qTriangulationTasks;
	std::mutex									_triangulationsMutex;

	CComPtr<ID3D11Device>						_ptrD3DDevice;
	CComPtr<ID3D11DeviceContext>				_ptrDeviceContext;
	CComPtr<ID3D11ComputeShader>				_ptrComputeShader;

	bool threadFinished() const {
		return _threadFinished;
	}

	void setThreadFinished() {
		_threadFinished = true;
	}

	bool processTriangulations();

	static void triangulationThreadFunc(DirectComputeHeightfieldConverter*);
};
