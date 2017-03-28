#pragma once

#include <atlbase.h>
#include <d3d11.h>

#include "HeightfieldConverter.h"

struct HeightfieldConverter::HeightfieldConverterPrivate
{

	// �������������
	void	Init(ID3D11Device* in_pD3DDevice11);

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

	CComPtr<ID3D11Device>           m_ptrD3DDevice;
	CComPtr<ID3D11ComputeShader>	m_ptrComputeShader;
};