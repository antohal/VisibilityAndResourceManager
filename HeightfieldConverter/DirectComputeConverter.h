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

	DirectComputeHeightfieldConverter(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, HeightfieldConverter::HeightfieldConverterPrivate* owner);
	~DirectComputeHeightfieldConverter();

	//@{ IAbstractHeightfieldConverter

	// Создать триангуляцию немедленно и дождаться готовности
	virtual void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation) override;

	// добавить/удалить listener
	virtual void	RegisterListener(HeightfieldConverterListener*) override;
	virtual void 	UnregisterListener(HeightfieldConverterListener*) override;

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно
	virtual void	AppendTriangulationTask(const SHeightfield* in_pHeightfield) override;

	// обработать поставленные задачи
	virtual void	UpdateTasks();

	//@}

private:


	struct ConstantBufferData
	{
		SHeightfield::SHeightfieldConfig Config;
		
		float fWorldScale = 1;
		
		double vCenter[3];					// Точка начала координат триангуляции (лежит на поверхности эллипсоида)
		double vXAxis[3];					// Координаты оси X (направлена на север вдоль поверхности эллипсоида)
		double vYAxis[3];					// Координаты оси Y (направлена по нормали к поверхности эллипсоида)
		double vZAxis[3];					// Координаты оси Z (направлена на восток вдоль поверхности эллипсоида)

		float temp1;
		float temp2;
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
