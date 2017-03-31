#pragma once

#include "HeightfieldConverter.h"

class IAbstractHeightfieldConverter
{
public:

	virtual ~IAbstractHeightfieldConverter() {}

	// ������� ������������ ���������� � ��������� ����������
	virtual void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation) = 0;

	// ��������/������� listener
	virtual void	RegisterListener(HeightfieldConverterListener*) = 0;
	virtual void 	UnregisterListener(HeightfieldConverterListener*) = 0;

	// �������� ������ �� ������������, ������� ����� ����������� ����������
	virtual void	AppendTriangulationTask(const SHeightfield* in_pHeightfield) = 0;
};
