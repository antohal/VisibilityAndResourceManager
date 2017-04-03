#pragma once

#include "HeightfieldConverter.h"
#include "AbstractConverter.h"

struct HeightfieldConverter::HeightfieldConverterPrivate
{

	~HeightfieldConverterPrivate();

	// инициализация
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode);

	// Создать триангуляцию немедленно и дождаться готовности
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// добавить/удалить listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно с помощью DirectCompute
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);

	// обработать поставленные задачи
	void	UpdateTasks();

private:

	IAbstractHeightfieldConverter*	_pAbstractConverter = nullptr;
};