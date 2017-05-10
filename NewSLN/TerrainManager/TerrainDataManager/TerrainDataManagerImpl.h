#pragma once

#include "TerrainDataManager.h"

class CTerrainDataManager::CTerrainDataManagerImplementation
{
public:

	CTerrainDataManagerImplementation();
	~CTerrainDataManagerImplementation();

	// «агрузить описание данных поверхности «емли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock);

	// ќсвободить загруженное описание данных
	void	ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock);

};
