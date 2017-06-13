#pragma once

#include "HeightfieldConverter.h"
#include "AbstractConverter.h"

#include <atlbase.h>

struct HeightfieldConverter::HeightfieldConverterPrivate
{

	~HeightfieldConverterPrivate();

	// инициализация
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcszComputeShaderFile);

	// Задать глобальный коэффициент масштаба.
	// По умолчанию все расчеты ведуться в привязке к эллипсоиду Земли в системе координат WGS-84 в метрах
	void	SetWorldScale(float in_fScale);

	float	GetWorldScale() const;

	void	SetHeightScale(float in_fHeightScale);

	void	ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords);

	// Считать данные карты высот из текстуры
	void	ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield);

	// Создать триангуляцию немедленно и дождаться готовности
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// добавить/удалить listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно с помощью DirectCompute
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);

	// обработать поставленные задачи
	void	UpdateTasks();

	// Освободить карту высот
	void	ReleaseHeightfield(SHeightfield*);

	// Освободить буферы триангуляции
	void	ReleaseTriangulation(STriangulation*);

	// Получить буферы вершин и индексов в памяти
	void	UnmapTriangulation(STriangulation*, SVertex* out_pVertexes, unsigned int* out_pIndices);

	float	GetHeightScale() const {
		return _fHeightScale;
	}

private:

	IAbstractHeightfieldConverter*	_pAbstractConverter = nullptr;

	CComPtr<ID3D11Device>			_ptrD3DDevice;
	CComPtr<ID3D11DeviceContext>	_ptrDeviceContext;

	float							_fScale = 1;
	float							_fHeightScale = 1;
};
