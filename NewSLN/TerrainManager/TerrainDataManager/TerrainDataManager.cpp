#include "TerrainDataManagerImpl.h"
#include "TerrainDataBlock.h"
#include "Log.h"
#include "FileUtil.h"

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
bool CTerrainDataManager::LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff, unsigned int* out_uiMaximumDepth)
{
	return _implementation->LoadTerrainDataInfo(in_pcwszDirectoryName, out_ppRootDataBlock, in_fLongitudeScaleCoeff, in_fLattitudeScaleCoeff, out_uiMaximumDepth);
}

void CTerrainDataManager::GenerateTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth)
{
	_implementation->GenerateTerrainDataInfo(in_pcwszDirectoryName, out_ppRootDataBlock, in_uiM, in_uiN, in_uiDepth);
}

void CTerrainDataManager::LoadTerrainDataInfo(const wchar_t * in_pcwszDirectoryName, const DataBaseInfo & dbInfo, const LodInfoStruct * in_pLodInfoArray, unsigned int in_uiMaxDepth)
{
	_implementation->LoadTerrainDataInfo(in_pcwszDirectoryName, dbInfo, in_pLodInfoArray, in_uiMaxDepth);
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
bool CTerrainDataManager::CTerrainDataManagerImplementation::LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff, unsigned int* out_uiMaximumDepth)
{
	float fLattitudeRange = static_cast<float>(M_PI) * in_fLattitudeScaleCoeff;


	CTerrainBlockDesc* pRootBlock = CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(this,
		static_cast<float>(M_PI*0.5) - fLattitudeRange, static_cast<float>(M_PI*0.5), 0.f, static_cast<float>(2 * M_PI) * in_fLongitudeScaleCoeff, std::wstring(), std::wstring(), nullptr);

	_uiTerrainBlocksCount = 0;

	pRootBlock->_implementation->_params.aTreePosition[0].ucLattitudeIndex = 0;
	pRootBlock->_implementation->_params.aTreePosition[0].ucLongitudeIndex = 0;

	pRootBlock->_implementation->LoadChildsFromDirectory(in_pcwszDirectoryName);
	*out_ppRootDataBlock = pRootBlock;

	unsigned int uiMaxDepth = 0;
	GetDepthRecursive(pRootBlock, uiMaxDepth);

	if (out_uiMaximumDepth)
	{
		*out_uiMaximumDepth = uiMaxDepth;
	}

	_uiMaxDepth = uiMaxDepth;

	unsigned int uiMemUsage = 0;
	GetMemoryUsageRecursive(pRootBlock, uiMemUsage);

	LogMessage("Readed %d blocks, maximum tree depth is %d, bytes: %d", _uiTerrainBlocksCount, uiMaxDepth, uiMemUsage);

	GenerateAdjacency();

	return true;
}

void CTerrainDataManager::CTerrainDataManagerImplementation::LoadTerrainDataInfo(const wchar_t * in_pcwszDirectoryName, const DataBaseInfo & dbInfo, const LodInfoStruct * in_pLodInfoArray, unsigned int in_uiMaxDepth)
{
	double lastLodPixelsX = in_pLodInfoArray[dbInfo.LodCount - 1].CountX * in_pLodInfoArray[dbInfo.LodCount - 1].Width;
	double lastLodPixelsY = in_pLodInfoArray[dbInfo.LodCount - 1].CountY * in_pLodInfoArray[dbInfo.LodCount - 1].Height;

	float fLattitudeScaleCoeff = (lastLodPixelsX + dbInfo.DeltaX) / lastLodPixelsX;
	float fLongitudeScaleCoeff = (lastLodPixelsY + dbInfo.DeltaY) / lastLodPixelsY;

	float fLattitudeRange = static_cast<float>(M_PI) * fLattitudeScaleCoeff;

	CTerrainBlockDesc* pRootBlock = CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(this,
		static_cast<float>(M_PI*0.5) - fLattitudeRange, static_cast<float>(M_PI*0.5), 0.f, static_cast<float>(2 * M_PI) * fLongitudeScaleCoeff, std::wstring(), std::wstring(), nullptr);

	_uiTerrainBlocksCount = 0;

	pRootBlock->_implementation->_params.aTreePosition[0].ucLattitudeIndex = 0;
	pRootBlock->_implementation->_params.aTreePosition[0].ucLongitudeIndex = 0;



}

void CTerrainDataManager::CTerrainDataManagerImplementation::GenerateTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth)
{
	CTerrainBlockDesc* pRootBlock = CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(this,
		-static_cast<float>(M_PI*0.5), static_cast<float>(M_PI*0.5), 0.f, static_cast<float>(2 * M_PI), std::wstring(), std::wstring(), nullptr);

	_uiTerrainBlocksCount = 0;

	std::wstring wsDirectory = in_pcwszDirectoryName;

	wchar_t backChar = wsDirectory.back();

	if (backChar == L'\\' || backChar == L'/')
		wsDirectory = wsDirectory.substr(0, wsDirectory.size() - 1);

	std::wstring wsHeightmapDir = wsDirectory + L"\\HeightMaps";
	std::wstring wsTexturesDir = wsDirectory + L"\\Textures";

	std::vector<std::wstring> vecHeightmaps;
	GetFileListFromDirectory(wsHeightmapDir, vecHeightmaps);

	std::vector<std::wstring> vecTextures;
	GetFileListFromDirectory(wsTexturesDir, vecTextures);

	pRootBlock->_implementation->_params.aTreePosition[0].ucLattitudeIndex = 0;
	pRootBlock->_implementation->_params.aTreePosition[0].ucLongitudeIndex = 0;

	pRootBlock->_implementation->GenerateChilds(in_pcwszDirectoryName, in_uiM, in_uiN, in_uiDepth, vecTextures, vecHeightmaps);
	*out_ppRootDataBlock = pRootBlock;

	unsigned int uiMaxDepth = 0;
	GetDepthRecursive(pRootBlock, uiMaxDepth);

	_uiMaxDepth = uiMaxDepth;

	unsigned int uiMemUsage = 0;
	GetMemoryUsageRecursive(pRootBlock, uiMemUsage);

	GenerateAdjacency();

	LogMessage("Generated %d blocks, maximum tree depth is %d, bytes: %d", _uiTerrainBlocksCount, uiMaxDepth, uiMemUsage);
}


void CTerrainDataManager::CTerrainDataManagerImplementation::TerrainBlockCreated(CTerrainBlockDesc* in_pTerrainBlock)
{
	_uiTerrainBlocksCount++;

	_mapLodLevelBlocks[in_pTerrainBlock->GetParams()->uiDepth].push_back(in_pTerrainBlock);
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

	if (block->_implementation->_params.uiDepth > out_depth)
		out_depth = block->_implementation->_params.uiDepth;

	for (unsigned int i = 0; i < block->GetChildBlockDescCount(); i++)
	{
		GetDepthRecursive(block->GetChildBlockDesc(i), out_depth);
	}
}

void CTerrainDataManager::CTerrainDataManagerImplementation::GetMemoryUsageRecursive(const CTerrainBlockDesc* block, unsigned int& out_memory)
{
	if (!block)
		return;

	out_memory += block->_implementation->GetMemoryUsage();

	for (unsigned int i = 0; i < block->GetChildBlockDescCount(); i++)
	{
		GetMemoryUsageRecursive(block->GetChildBlockDesc(i), out_memory);
	}
}

inline bool EQUALS(float a, float b)
{
	return fabsf(a - b) < 1e-4f;
}

void CTerrainDataManager::CTerrainDataManagerImplementation::GenerateAdjacency()
{
	for (unsigned int uiDepth = 1; uiDepth <= _uiMaxDepth; uiDepth++)
	{

		std::vector<CTerrainBlockDesc*> vecDepthBlocks = _mapLodLevelBlocks[uiDepth];

		//for (CTerrainBlockDesc* blockA : vecDepthBlocks)
		for (size_t iBlockA = 0; iBlockA < vecDepthBlocks.size(); iBlockA++)
		{
			CTerrainBlockDesc* blockA = vecDepthBlocks[iBlockA];

			//for (CTerrainBlockDesc* blockB : vecDepthBlocks)
			for (size_t iBlockB = iBlockA + 1; iBlockB < vecDepthBlocks.size(); iBlockB++)
			{
	
				CTerrainBlockDesc* blockB = vecDepthBlocks[iBlockB];

				// horizontal
				if (EQUALS(blockA->GetParams()->fMinLattitude, blockB->GetParams()->fMinLattitude) && EQUALS(blockA->GetParams()->fMaxLattitude, blockB->GetParams()->fMaxLattitude))
				{
					// left
					if (EQUALS(blockA->GetParams()->fMinLongitude, blockB->GetParams()->fMaxLongitude) ||
						(EQUALS(blockA->GetParams()->fMinLongitude, 0) && EQUALS(blockB->GetParams()->fMaxLongitude, 2*M_PI)))
					{
						blockA->_implementation->_neighbours[6] = blockB;
						blockB->_implementation->_neighbours[2] = blockA;
						continue;
					}

					// right
					if (EQUALS(blockA->GetParams()->fMaxLongitude, blockB->GetParams()->fMinLongitude) ||
						(EQUALS(blockA->GetParams()->fMaxLongitude, 2*M_PI) && EQUALS(blockB->GetParams()->fMaxLongitude, 0)))
					{
						blockA->_implementation->_neighbours[2] = blockB;
						blockB->_implementation->_neighbours[6] = blockA;
						continue;
					}
				}


				//vertical
				if (EQUALS(blockA->GetParams()->fMinLongitude, blockB->GetParams()->fMinLongitude) && EQUALS(blockA->GetParams()->fMaxLongitude, blockB->GetParams()->fMaxLongitude))
				{

					//top
					if (EQUALS(blockA->GetParams()->fMaxLattitude, blockB->GetParams()->fMinLattitude))
					{
						blockA->_implementation->_neighbours[0] = blockB;
						blockB->_implementation->_neighbours[4] = blockA;
					}


					//bottom
					if (EQUALS(blockA->GetParams()->fMinLattitude, blockB->GetParams()->fMaxLattitude))
					{
						blockA->_implementation->_neighbours[4] = blockB;
						blockB->_implementation->_neighbours[0] = blockA;
					}

				}

				// left top
				if (EQUALS(blockA->GetParams()->fMaxLattitude, blockB->GetParams()->fMinLattitude) &&
					(EQUALS(blockA->GetParams()->fMinLongitude, blockB->GetParams()->fMaxLongitude) || (EQUALS(blockA->GetParams()->fMinLongitude, 0) && EQUALS(blockB->GetParams()->fMaxLongitude, 2 * M_PI))))

				{
					blockA->_implementation->_neighbours[7] = blockB;
					blockB->_implementation->_neighbours[3] = blockA;
				}

				// right top
				if (EQUALS(blockA->GetParams()->fMaxLattitude, blockB->GetParams()->fMinLattitude) &&
					(EQUALS(blockA->GetParams()->fMaxLongitude, blockB->GetParams()->fMinLongitude) || (EQUALS(blockA->GetParams()->fMaxLongitude, 2 * M_PI) && EQUALS(blockB->GetParams()->fMinLongitude, 0))))

				{
					blockA->_implementation->_neighbours[1] = blockB;
					blockB->_implementation->_neighbours[5] = blockA;
				}

				// right bottom
				if (EQUALS(blockA->GetParams()->fMinLattitude, blockB->GetParams()->fMaxLattitude) &&
					(EQUALS(blockA->GetParams()->fMaxLongitude, blockB->GetParams()->fMinLongitude) || (EQUALS(blockA->GetParams()->fMaxLongitude, 2 * M_PI) && EQUALS(blockB->GetParams()->fMinLongitude, 0))))

				{
					blockA->_implementation->_neighbours[3] = blockB;
					blockB->_implementation->_neighbours[7] = blockA;
				}

				// left bottom
				if (EQUALS(blockA->GetParams()->fMinLattitude, blockB->GetParams()->fMaxLattitude) &&
					(EQUALS(blockA->GetParams()->fMinLongitude, blockB->GetParams()->fMaxLongitude) || (EQUALS(blockA->GetParams()->fMinLongitude, 0) && EQUALS(blockB->GetParams()->fMaxLongitude, 2 * M_PI))))

				{
					blockA->_implementation->_neighbours[5] = blockB;
					blockB->_implementation->_neighbours[1] = blockA;
				}
			
			}
		}

	}
}
