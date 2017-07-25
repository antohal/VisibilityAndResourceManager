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

	// ������� ������������ ���������� � ��������� ����������
	virtual void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation) override;

	// ��������/������� listener
	virtual void	RegisterListener(HeightfieldConverterListener*) override;
	virtual void 	UnregisterListener(HeightfieldConverterListener*) override;

	// �������� ������ �� ������������, ������� ����� ����������� ����������
	virtual void	AppendTriangulationTask(const SHeightfield* in_pHeightfield) override;

	// ���������� ������������ ������
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
		STriangulationTask(DirectComputeHeightfieldConverter* owner, const SHeightfield& heightfield) : _owner(owner), _heightfield(heightfield)
		{
		}

		SHeightfield	_heightfield;
		STriangulation	_triangulation;

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

	std::set<HeightfieldConverterListener*>		_setListeners;

	std::mutex									_tasksMutex;
	std::queue<STriangulationTask*>				_qTriangulationTasks;

	CComPtr<ID3D11Device>						_ptrD3DDevice;
	CComPtr<ID3D11DeviceContext>				_ptrDeviceContext;
	CComPtr<ID3D11ComputeShader>				_ptrComputeShader;

	HeightfieldConverter::HeightfieldConverterPrivate*	_owner = nullptr;
};
