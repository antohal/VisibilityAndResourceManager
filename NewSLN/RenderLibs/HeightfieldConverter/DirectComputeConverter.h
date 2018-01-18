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
	DirectComputeHeightfieldConverter(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, ID3DX11Effect* in_pEffect, HeightfieldConverter::HeightfieldConverterPrivate* owner);
	~DirectComputeHeightfieldConverter();

	//@{ IAbstractHeightfieldConverter

	virtual void	ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords) override;

	// Создать триангуляцию немедленно и дождаться готовности
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, STriangulation* out_pTriangulation, const SHeightfield** in_ppNeighbours) override;

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, void* param, TriangulationTaskCompleteCallback in_Callback) override;

	// обработать поставленные задачи
	virtual void	UpdateTasks();

	//@}

private:

	struct ConstantBufferData
	{
		//SHeightfield::SConfig Config;

		float			fMinLattitude = 0;				// минимальная широта (рад)
		float			fMaxLattitude = 0;				// максимальная широта (рад)

		float			fMinLongitude = 0;				// минимальная долгота (рад)
		float			fMaxLongitude = 0;				// максимальная долгота (рад)

		unsigned int	nCountX = 256;					
		unsigned int	nCountY = 256;					

		float	fLongitudeCoeff = 1;
		float	fLattitudeCoeff = 1;

		float	fWorldScale = 1;
		float	fHeightScale = 1;

		float	fNormalDivisionAngleCos1 = 0.86f;
		float	fNormalDivisionAngleCos2 = 0.5f;

		float	fNorthBlockLongCoeff = 1.f;
		float	fNorthBlockLatCoeff = 1.f;

		float	fNorthEastBlockLongCoeff = 1.f;
		float	fNorthEastBlockLatCoeff = 1.f;

		float	fEastBlockLongCoeff = 1.f;
		float	fEastBlockLatCoeff = 1.f;

		float	fSouthEastBlockLongCoeff = 1.f;
		float	fSouthEastBlockLatCoeff = 1.f;

		float	fSouthBlockLongCoeff = 1.f;
		float	fSouthBlockLatCoeff = 1.f;

		float	fSouthWestBlockLongCoeff = 1.f;
		float	fSouthWestBlockLatCoeff = 1.f;

		float	fWestBlockLongCoeff = 1.f;
		float	fWestBlockLatCoeff = 1.f;

		float	fNorthWestBlockLongCoeff = 1.f;
		float	fNorthWestBlockLatCoeff = 1.f;

		float	fNeighbourCoords[32];

	};

	struct STriangulationTask
	{
		STriangulationTask(DirectComputeHeightfieldConverter* owner, const SHeightfield& heightfield, float in_fLongitudeCutCoeff, 
			float in_fLattitudeCutCoeff, void* param, TriangulationTaskCompleteCallback callback, const SHeightfield** ppNeighbours)
				: _fLattitudeCoeff(in_fLattitudeCutCoeff), _fLongitudeCoeff(in_fLongitudeCutCoeff)
		{
			_owner = owner;
			_heightfield = heightfield;
			_param = param;
			_callback = callback;
			_neighbours = ppNeighbours;
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

		const SHeightfield**				_neighbours = nullptr;
	};

	std::mutex									_tasksMutex;
	std::queue<STriangulationTask*>				_qTriangulationTasks;

	CComPtr<ID3D11Device>						_ptrD3DDevice;
	CComPtr<ID3D11DeviceContext>				_ptrDeviceContext;
	CComPtr<ID3D11ComputeShader>				_ptrComputeShader;

	HeightfieldConverter::HeightfieldConverterPrivate*	_owner = nullptr;
};
