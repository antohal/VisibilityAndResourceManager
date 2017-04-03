#pragma once

#include "HeightfieldConverter.h"
#include "AbstractConverter.h"

struct HeightfieldConverter::HeightfieldConverterPrivate
{

	~HeightfieldConverterPrivate();

	// �������������
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode);

	// ������� ������������ ���������� � ��������� ����������
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// ��������/������� listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// �������� ������ �� ������������, ������� ����� ����������� ���������� � ������� DirectCompute
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);

	// ���������� ������������ ������
	void	UpdateTasks();

private:

	IAbstractHeightfieldConverter*	_pAbstractConverter = nullptr;
};