#pragma once

#include <atlbase.h>
#include <d3d11.h>

#include <mutex>
#include <thread>
#include <queue>
#include <set>

#include "AbstractConverter.h"
#include "HeightfieldConverterPrivate.h"

class DirectComputeHeightfieldConverter : public IAbstractHeightfieldConverter
{
public:

	DirectComputeHeightfieldConverter(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcszComputeShaderFile, HeightfieldConverter::HeightfieldConverterPrivate* owner);
	~DirectComputeHeightfieldConverter();

	//@{ IAbstractHeightfieldConverter

	virtual void	ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords) override;

	// Создать триангуляцию немедленно и дождаться готовности
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, STriangulation* out_pTriangulation) override;

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, void* param, TriangulationTaskCompleteCallback in_Callback) override;

	// обработать поставленные задачи
	virtual void	UpdateTasks();

	//@}

private:

	struct ConstantBufferData
	{
		SHeightfield::SConfig Config;

		float	fLongitudeCoeff = 1;
		float	fLattitudeCoeff = 1;

		float fWorldScale = 1;
		float fHeightScale = 1;

		float	fTemp2;
		float	fTemp3;
	};

	struct STriangulationTask
	{
		STriangulationTask(DirectComputeHeightfieldConverter* owner, const SHeightfield& heightfield, float in_fLongitudeCutCoeff, 
			float in_fLattitudeCutCoeff, void* param, TriangulationTaskCompleteCallback callback)
				: _owner(owner), _heightfield(heightfield), _param(param), _callback(callback), _fLattitudeCoeff(in_fLattitudeCutCoeff), _fLongitudeCoeff(in_fLongitudeCutCoeff)
		{
		}

		SHeightfield	_heightfield;
		STriangulation	_triangulation;

		float			_fLattitudeCoeff = 1.f;
		float			_fLongitudeCoeff = 1.f;

		void*			_param = nullptr;
		TriangulationTaskCompleteCallback _callback = nullptr;


		void	createTriangulation();
		void	computeBasis();
		
		void	createInputBuffers();
		void	createOutputBuffers();

		DirectComputeHeightfieldConverter*	_owner = nullptr;

		CComPtr<ID3D11UnorderedAccessView>	_ptrIndexBufferUAV;
		CComPtr<ID3D11UnorderedAccessView>	_ptrVertexBufferUAV;

		CComPtr<ID3D11Buffer>				_ptrInputBuffer;

		CComPtr<ID3D11Buffer>				_ptrConstantBuffer;
	};

	std::mutex									_tasksMutex;
	std::queue<STriangulationTask*>				_qTriangulationTasks;

	CComPtr<ID3D11Device>						_ptrD3DDevice;
	CComPtr<ID3D11DeviceContext>				_ptrDeviceContext;
	CComPtr<ID3D11ComputeShader>				_ptrComputeShader;

	HeightfieldConverter::HeightfieldConverterPrivate*	_owner = nullptr;
};
