#include "TerrainDataManagerImpl.h"
#include "TerrainDataBlock.h"
#include "Log.h"

#define _USE_MATH_DEFINES

#include <math.h>

CTerrainDataManager::CTerrainDataManager()
{
	LogMessage("CTerrainDataManager::Init OK.");

	_implementation = new CTerrainDataManager::CTerrainDataManagerImplementation();
}

CTerrainDataManager::~CTerrainDataManager()
{
	delete _implementation;
}

// «агрузить описание данных поверхности «емли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
bool CTerrainDataManager::LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int* out_uiMaximumDepth)
{
	return _implementation->LoadTerrainDataInfo(in_pcwszDirectoryName, out_ppRootDataBlock, out_uiMaximumDepth);
}

// ќсвободить загруженное описание данных
void CTerrainDataManager::ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock)
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
bool CTerrainDataManager::CTerrainDataManagerImplementation::LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int* out_uiMaximumDepth)
{
	CTerrainBlockDesc* pRootBlock = CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(this,
		-static_cast<float>(M_PI*0.5), static_cast<float>(M_PI*0.5), 0.f, static_cast<float>(2 * M_PI), std::wstring(), std::wstring(), nullptr);

	pRootBlock->_implementation->LoadChildsFromDirectory(in_pcwszDirectoryName);
	*out_ppRootDataBlock = pRootBlock;

	if (out_uiMaximumDepth)
	{
		*out_uiMaximumDepth = 0;

		GetDepthRecursive(pRootBlock, *out_uiMaximumDepth);
	}

	return true;
}

// ќсвободить загруженное описание данных
void CTerrainDataManager::CTerrainDataManagerImplementation::ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock)
{
	delete in_pTerrainDataBlock;
}

void CTerrainDataManager::CTerrainDataManagerImplementation::GetDepthRecursive(const CTerrainBlockDesc* block, unsigned int& out_depth)
{
	if (!block)
		return;

	if (block->_implementation->_uiDepth > out_depth)
		out_depth = block->_implementation->_uiDepth;

	for (unsigned int i = 0; i < block->GetChildBlockDescCount(); i++)
	{
		GetDepthRecursive(block->GetChildBlockDesc(i), out_depth);
	}
}

