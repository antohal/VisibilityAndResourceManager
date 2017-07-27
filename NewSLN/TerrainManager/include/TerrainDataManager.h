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


// �������� ����� �����
class TERRAINDATAMANAGER_API CTerrainBlockDesc
{
public:

	CTerrainBlockDesc();

	const STerrainBlockParams*		GetParams() const;

	// �������� ��� ����� ��������
	const wchar_t*					GetTextureFileName() const;

	// �������� ��� ����� ����� �����
	const wchar_t*					GetHeightmapFileName() const;

	// �������� ��������� �� ������������ ���� (���������� null, ���� �������� ��������)
	const CTerrainBlockDesc*		GetParentBlockDesc() const;

	// �������� ���������� �������� ������
	unsigned int					GetChildBlockDescCount() const;

	// �������� ��������� �� �������� ����
	const CTerrainBlockDesc*		GetChildBlockDesc(unsigned int id) const;

protected:

	~CTerrainBlockDesc();

private:

	class CTerrainBlockDescImplementation;
	CTerrainBlockDescImplementation*	_implementation = nullptr;

	friend class CTerrainDataManager;
};

class TERRAINDATAMANAGER_API CTerrainDataManager
{
public:

	CTerrainDataManager();
	~CTerrainDataManager();

	// ��������� �������� ������ ����������� ����� [out_ppRootDataBlock] �� ��������� ���������� [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff, unsigned int* out_uiMaximumDepth = nullptr);

	// ������������� ������ �������. �������� � ����� �������� ����� ������� �� ���������� [in_pcwszDirectoryName] ��������� ������� 
	// ����� �������� �� HeightMaps, �������� �� Textures
	// in_uiM, in_uiN - ���������� ��������� ������� ������ �� ������ � �������
	// in_uiDepth - ������� ������
	void	GenerateTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth);

	// ���������� ����������� �������� ������
	void	ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;

	friend class CTerrainBlockDesc::CTerrainBlockDescImplementation;
};
