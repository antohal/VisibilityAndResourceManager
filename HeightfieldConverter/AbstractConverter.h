#pragma once

#include "HeightfieldConverter.h"

class IAbstractHeightfieldConverter
{
public:

	virtual ~IAbstractHeightfieldConverter() {}

	// Создать триангуляцию немедленно и дождаться готовности
	virtual void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation) = 0;

	// добавить/удалить listener
	virtual void	RegisterListener(HeightfieldConverterListener*) = 0;
	virtual void 	UnregisterListener(HeightfieldConverterListener*) = 0;

	// добавить задачу на триангуляцию, которая будет выполняться асинхронно
	virtual void	AppendTriangulationTask(const SHeightfield* in_pHeightfield) = 0;
};
