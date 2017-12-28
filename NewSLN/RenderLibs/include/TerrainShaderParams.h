#pragma once

struct SGlobalTerrainShaderParams
{
	//������������ ���������
	// N - ���������� LOD-��; 
	// ��� ������ �������, ��� ���� �����������; 
	// ������ ������ �������� �� ���, �� ������� ������������ ���������;
	// partitionCoefficients[k+1] �������� ������������ �� ���� ����, �� ������� ��� ������ ������ k+1 ������, ��� ������ ������ k
	// 	(�.�., �� ������� ��� ����� ������� ������ k+1 ����, ��� ����� ������� ������ k)
	// partitionCoefficients[0] �������� �������������� ���������� � ��������� ���� ����� �� ������� 
	// ��������� �� X ������ ���� ���������� ����� nCountX-1
	// ��������� �� Y ������ ���� ���������� ����� nCountY-1
	unsigned int	aPartitionCoefficients[20][2];

	// ���������� ������ �� X � Y  ��� ������� ������
	unsigned int	aVertexCounts[20][2];

	//20 - ��� ������������ ���������� ������� �����������. �������� ���������� ������� ���������� � ���������� levelsCount
	unsigned int	uiLevelsCount = 0;
};

struct STerrainBlockShaderParams
{
	float			fMinLattitude = 0;				// ����������� ������
	float			fMaxLattitude = 0;				// ������������ ������

	float			fMinLongitude = 0;				// ����������� �������
	float			fMaxLongitude = 0;				// ������������ �������

	unsigned int	nCountX = 0;					// ���������� ����� �� ������� (����� �� ��������� � ����������� ��������)
	unsigned int	nCountY = 0;					// ���������� ����� �� ������ (����� �� ��������� � ����������� ��������)

	float			fLongitudeCoeff = 0;			// ������������ ���������� ���������� �� �������
	float			fLattitudeCoeff = 0;			// ������������ ���������� ���������� �� ������

	float			fWorldScale = 1;				// ������� ����
	float			fHeightScale = 1;				// ������� ������

	// ������������ ������� ���������� ��������� ��� �������� ������
	float			fNorthBlockLongCoeff = 1;
	float			fNorthBlockLatCoeff = 1;

	float			fNorthEastBlockLongCoeff = 1;
	float			fNorthEastBlockLatCoeff = 1;

	float			fEastBlockLongCoeff = 1;
	float			fEastBlockLatCoeff = 1;

	float			fSouthEastBlockLongCoeff = 1;
	float			fSouthEastBlockLatCoeff = 1;

	float			fSouthBlockLongCoeff = 1;
	float			fSouthBlockLatCoeff = 1;

	float			fSouthWestBlockLongCoeff = 1;
	float			fSouthWestBlockLatCoeff = 1;

	float			fWestBlockLongCoeff = 1;
	float			fWestBlockLatCoeff = 1;

	float			fNorthWestBlockLongCoeff = 1;
	float			fNorthWestBlockLatCoeff = 1;

	// 0	-	���/���� �������/������ ��������� ��������� �����
	float			fNorthMinLat = 0;
	float			fNorthMaxLat = 0;

	float			fNorthMinLong = 0;
	float			fNorthMaxLong = 0;

	// 1	-	���/���� �������/������ ������-���������� ��������� �����
	float			fNorthEastMinLat = 0;
	float			fNorthEastMaxLat = 0;

	float			fNorthEastMinLong = 0;
	float			fNorthEastMaxLong = 0;

	//2		-	���/���� �������/������ ���������� ��������� �����

	float			fEastMinLat = 0;
	float			fEastMaxLat = 0;

	float			fEastMinLong = 0;
	float			fEastMaxLong = 0;

	//3		-	���/���� �������/������ ���-���������� ��������� �����
	float			fSouthEastMinLat = 0;
	float			fSouthEastMaxLat = 0;

	float			fSouthEastMinLong = 0;
	float			fSouthEastMaxLong = 0;

	//4		-	���/���� �������/������ ������ ��������� �����
	float			fSouthMinLat = 0;
	float			fSouthMaxLat = 0;

	float			fSouthMinLong = 0;
	float			fSouthMaxLong = 0;

	//5		-	���/���� �������/������ ���-��������� ��������� �����
	float			fSouthWestMinLat = 0;
	float			fSouthWestMaxLat = 0;

	float			fSouthWestMinLong = 0;
	float			fSouthWestMaxLong = 0;

	//6		-	���/���� �������/������ ��������� ��������� �����
	float			fWestMinLat = 0;
	float			fWestMaxLat = 0;

	float			fWestMinLong = 0;
	float			fWestMaxLong = 0;

	//7		-	���/���� �������/������ ������-��������� ��������� �����
	float			fNorthWestMinLat = 0;
	float			fNorthWestMaxLat = 0;

	float			fNorthWestMinLong = 0;
	float			fNorthWestMaxLong = 0;


	//������ ����������� ��� �������� � �������� �������� terrain-�
	//���������� ������:
	// 0 - �������� �������
	// 1 - �������� �������
	// 2 - ����� �������
	// 3 - ��������� �������
	unsigned int	uiCurrentLOD = 0;
	unsigned int	uiAdjacentLOD[4];

	// �������� ����� ����� ������� �����. ������ - R32F
	ID3D11ShaderResourceView*	pHeightfield = nullptr;

	// �������� ����� ����� ��� �������� ������
	ID3D11ShaderResourceView*	pNorthNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pNorthEastNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pEastNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pSouthEastNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pSouthNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pSouthWestNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pWestNeighbourHeightfield = nullptr;
	ID3D11ShaderResourceView*	pNorthWestNeighbourHeightfield = nullptr;

	// ����� ������ ��� ������� �����
	ID3D11Buffer*				pVertexBuffer = nullptr;
};
