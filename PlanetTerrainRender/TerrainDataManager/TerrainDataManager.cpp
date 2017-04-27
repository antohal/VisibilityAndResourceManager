#include "TerrainDataManagerImpl.h"
#include "TerrainDataBlock.h"
#include "Log.h"

#define _USE_MATH_DEFINES

#include <math.h>

CTerrainDataManager::CTerrainDataManager()
{
	LogInit("TerrainDataManager.log");
	LogEnable(true);

	LogMessage("CTerrainDataManager::Init OK.");

	_implementation = new CTerrainDataManager::CTerrainDataManagerImplementation();
}

CTerrainDataManager::~CTerrainDataManager()
{
	delete _implementation;
}

// «агрузить описание данных поверхности «емли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
bool CTerrainDataManager::LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockData** out_ppRootDataBlock)
{
	return _implementation->LoadTerrainDataInfo(in_pcwszDirectoryName, out_ppRootDataBlock);
}

// ќсвободить загруженное описание данных
void CTerrainDataManager::ReleaseTerrainDataInfo(CTerrainBlockData* in_pTerrainDataBlock)
{
	_implementation->ReleaseTerrainDataInfo(in_pTerrainDataBlock);
}

//
// CTerrainDataManagerImplementation
//

CTerrainDataManager::CTerrainDataManagerImplementation::CTerrainDataManagerImplementation()
{

}

CTerrainDataManager::CTerrainDataManagerImplementation::~CTerrainDataManagerImplementation()
{

}


// «агрузить описание данных поверхности «емли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
bool CTerrainDataManager::CTerrainDataManagerImplementation::LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockData** out_ppRootDataBlock)
{
	CTerrainBlockData* pRootBlock = CTerrainBlockData::CTerrainBlockDataImplementation::CreateTerrainBlockDataInstance(this,
		-static_cast<float>(M_PI*0.5), static_cast<float>(M_PI*0.5), 0.f, static_cast<float>(2 * M_PI), std::wstring(), std::wstring(), nullptr);

	pRootBlock->_implementation->LoadChildsFromDirectory(in_pcwszDirectoryName);
	*out_ppRootDataBlock = pRootBlock;

	return true;
}

// ќсвободить загруженное описание данных
void CTerrainDataManager::CTerrainDataManagerImplementation::ReleaseTerrainDataInfo(CTerrainBlockData* in_pTerrainDataBlock)
{
	delete in_pTerrainDataBlock;
}
