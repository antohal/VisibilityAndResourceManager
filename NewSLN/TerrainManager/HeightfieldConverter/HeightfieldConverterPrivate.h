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

	void	SetNormalDivisionAngles(float in_fAngle1InDeg, float in_fAngle2InDeg);

	void	ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords);

	// ������� ������ ����� ����� �� ��������
	void	ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield, unsigned short in_usCompressionRatio);

	// ������� ������������ ���������� � ��������� ����������
	void	CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, STriangulation* out_pTriangulation, const SHeightfield** in_ppNeighbours);

	// �������� ������ �� ������������, ������� ����� ����������� ����������
	void	AppendTriangulationTask(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, void* param, TriangulationTaskCompleteCallback in_Callback);

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

	float	GetNormalDivisionAngle1Deg() const {
		return _fNormalDivisionAngle1;
	}

	float	GetNormalDivisionAngle2Deg() const {
		return _fNormalDivisionAngle2;
	}

private:

	IAbstractHeightfieldConverter*	_pAbstractConverter = nullptr;

	CComPtr<ID3D11Device>			_ptrD3DDevice;
	CComPtr<ID3D11DeviceContext>	_ptrDeviceContext;

	float							_fScale = 1;
	float							_fHeightScale = 1;

	float							_fNormalDivisionAngle1 = 30;
	float							_fNormalDivisionAngle2 = 90;
};
