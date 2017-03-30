#pragma once

#include "HeightfieldConverter.h"

struct HeightfieldConverter::HeightfieldConverterPrivate
{

	// �������������
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode);

	// ������� ������� - ����� ����������� ������� ����� �������� D3D11Device
	void	CreateResources();

	// ���������� ������� - ���������� � ������ ������������ D3D11Device (�������� ��� ��������� ��������� ����)
	void	ReleaseResources();

	// ������� ������������ ���������� � ��������� ����������
	bool	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// ��������/������� listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// �������� ������ �� ������������, ������� ����� ����������� ���������� � ������� DirectCompute
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);


private:


};