#pragma once

#include "TerrainDataManager.h"
#include <map>
#include <vector>

class CTerrainDataManager::CTerrainDataManagerImplementation
{
public:

	CTerrainDataManagerImplementation();
	~CTerrainDataManagerImplementation();

	// «агрузить описание данных поверхности «емли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff, unsigned int* out_uiMaximumDepth/* = nullptr*/);

	void	LoadTerrainDataInfo(const wchar_t * in_pcwszDirectoryName, const DataBaseInfo & dbInfo, const LodInfoStruct * in_pLodInfoArray, unsigned int in_uiMaxDepth, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int* out_uiMaximumDepth);

	void	GenerateTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth);

	// ќсвободить загруженное описание данных
	void	ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock);

	void	TerrainBlockCreated(CTerrainBlockDesc*);

private:

	void	GetDepthRecursive(const CTerrainBlockDesc* block, unsigned int& out_depth);
	void	GetMemoryUsageRecursive(const CTerrainBlockDesc* block, unsigned int& out_memory);
	void	GenerateAdjacency();

	unsigned int	_uiTerrainBlocksCount = 0;
	unsigned int	_uiMaxDepth = 0;

	std::map<int, std::vector<CTerrainBlockDesc*>>	_mapLodLevelBlocks;
};
