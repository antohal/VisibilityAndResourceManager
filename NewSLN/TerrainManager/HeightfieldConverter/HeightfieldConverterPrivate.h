#pragma once

#include "HeightfieldConverter.h"
#include "AbstractConverter.h"

#include <atlbase.h>

struct HeightfieldConverter::HeightfieldConverterPrivate
{

	~HeightfieldConverterPrivate();

	// �������������
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcszComputeShaderFile);

	// ������ ���������� ����������� ��������.
	// �� ��������� ��� ������� �������� � �������� � ���������� ����� � ������� ��������� WGS-84 � ������
	void	SetWorldScale(float in_fScale);

	float	GetWorldScale() const;

	void	SetHeightScale(float in_fHeightScale);

	void	ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords);

	// ������� ������ ����� ����� �� ��������
	void	ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield);

	// ������� ������������ ���������� � ��������� ����������
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation);

	// ��������/������� listener
	void	RegisterListener(HeightfieldConverterListener*);
	void 	UnregisterListener(HeightfieldConverterListener*);

	// �������� ������ �� ������������, ������� ����� ����������� ���������� � ������� DirectCompute
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield);

	// ���������� ������������ ������
	void	UpdateTasks();

	// ���������� ����� �����
	void	ReleaseHeightfield(SHeightfield*);

	// ���������� ������ ������������
	void	ReleaseTriangulation(STriangulation*);

	// �������� ������ ������ � �������� � ������
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
