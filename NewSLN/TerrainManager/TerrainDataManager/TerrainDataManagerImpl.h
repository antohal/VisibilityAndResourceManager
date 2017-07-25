#pragma once

#include "TerrainDataManager.h"

class CTerrainDataManager::CTerrainDataManagerImplementation
{
public:

	CTerrainDataManagerImplementation();
	~CTerrainDataManagerImplementation();

	// ��������� �������� ������ ����������� ����� [out_ppRootDataBlock] �� ��������� ���������� [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, float in_fLongitudeScaleCoeff, unsigned int* out_uiMaximumDepth/* = nullptr*/);

	void	GenerateTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth);

	// ���������� ����������� �������� ������
	void	ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock);

	void	TerrainBlockCreated(CTerrainBlockDesc*);

private:

	void	GetDepthRecursive(const CTerrainBlockDesc* block, unsigned int& out_depth);

	void	GetMemoryUsageRecursive(const CTerrainBlockDesc* block, unsigned int& out_memory);

	unsigned int	_uiTerrainBlocksCount = 0;
};
