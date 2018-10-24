#pragma once

#include <utility>

#ifndef TERRAINMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

// ������������ ������� ������
#define MAX_TERRAIN_TREE_DEPTH	20

// ������� ������� ����� �� ������� ������
struct STerrainBlockIndex
{
	unsigned char	ucLongitudeIndex = 255;
	unsigned char	ucLattitudeIndex = 255;
};

// ��������� ����� �����
struct STerrainBlockParams
{
	// ����������� � ������������ �������
	float				fMinLongitude = 0;
	float				fMaxLongitude = 0;

	// ����������� � ������������ ������
	float				fMinLattitude = 0;
	float				fMaxLattitude = 0;

	// ������������ �������
	float				fLongitude�utCoeff = 1;
	float				fLattitudeCutCoeff = 1;

	// ������� ������� �����
	unsigned int		uiDepth = 0;

	// ��������� � ������ (������ �� ������� � ������ �� ������� ������ �������). ��� ������ ������� ������ �������� ����� �������� �� ��������� = 255
	STerrainBlockIndex	aTreePosition[MAX_TERRAIN_TREE_DEPTH];
};

#pragma pack(push, 4)

// ���������, ����������� �� �����, ����������� ���� �����
struct DataBaseVersionInfo
{
	unsigned char Major;
	unsigned char Minor;
};


struct DataBaseInfo : public DataBaseVersionInfo
{
	unsigned char Reserved;
	unsigned char LodCount; // ���-�� �����
	int DeltaY;				// ���-�� ��� �������� �� X ��� ����������� ��������
	int DeltaX;				//  ���-�� ��� �������� �� Y ��� ����������� ��������
};

struct DataBaseInfo_Ver_1_3 : public DataBaseInfo
{
	// � ������, ���� ������ ������ 1.3, �� ��� �������� ����������� �� DeltaY, DeltaX
	double MaxLon = 0;
	double MinLat = 0;
};

struct LodInfoStruct_Ver_1_1
{
	short Width;			// ������ ��������
	short Height;			// ������ ��������
	short CountY;			// ���-�� ������� �� X
	short CountX;			// ���-�� ������� �� Y

	// ver 1.1

	unsigned int	TextureFormat = 0;				// 4 �����

	unsigned int	TextureScaleAlgorithm = 0;		// 4 �����
	bool			TextureIsGauss = false;				// 1 �����

	short			AltWidth = 0;
	short			AltHeight = 0;

	unsigned int	AltFormat = 0;					// 4 �����
	unsigned int	AltScaleAlgorithm = 0;			// 4 �����
	bool			AltIsGauss = 0;					// 1 �����
	bool			HasBorder = 0;					// 1 �����
};

struct LodInfoStruct_Ver_1_2
{
	short Width;			// ������ ��������
	short Height;			// ������ ��������
	short CountY;			// ���-�� ������� �� X
	short CountX;			// ���-�� ������� �� Y

							// ver 1.1

	unsigned int	TextureFormat = 0;				// 4 �����

	unsigned int	TextureScaleAlgorithm = 0;		// 4 �����
	bool			TextureIsGauss = false;				// 1 �����

	short			AltWidth = 0;
	short			AltHeight = 0;

	unsigned int	AltFormat = 0;					// 4 �����
	unsigned int	AltScaleAlgorithm = 0;			// 4 �����
	bool			AltIsGauss = 0;					// 1 �����
	bool			HasBorder = 0;					// 1 �����

	// ver 1.2
	unsigned char	Border = 0;
	unsigned char	AltBorder = 0;
};

#pragma pack(pop)
