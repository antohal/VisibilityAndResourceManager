#include "TerrainDataBlock.h"

#include <strsafe.h>
#include <Windows.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "Log.h"
#include "StlUtil.h"
#include "FileUtil.h"

#include <sstream>

CTerrainBlockDesc::CTerrainBlockDesc()
{
	_implementation = new CTerrainBlockDescImplementation(this);
}

const STerrainBlockParams * CTerrainBlockDesc::GetParams() const
{
	return _implementation->GetParams();
}

CTerrainBlockDesc::~CTerrainBlockDesc()
{
	delete _implementation;
}

// �������� ��� ����� ��������
const wchar_t* CTerrainBlockDesc::GetTextureFileName() const
{
	return _implementation->GetTextureFileName();
}

// �������� ��� ����� ����� �����
const wchar_t* CTerrainBlockDesc::GetHeightmapFileName() const
{
	return _implementation->GetHeightmapFileName();
}

// �������� ��������� �� ������������ ���� (���������� null, ���� �������� ��������)
const CTerrainBlockDesc* CTerrainBlockDesc::GetParentBlockDesc() const
{
	return _implementation->GetParentBlockData();
}

// �������� ���������� �������� ������
unsigned int CTerrainBlockDesc::GetChildBlockDescCount() const
{
	return _implementation->GetChildBlockDataCount();
}

// �������� ��������� �� �������� ����
const CTerrainBlockDesc* CTerrainBlockDesc::GetChildBlockDesc(unsigned int id) const
{
	return _implementation->GetChildBlockData(id);
}

const CTerrainBlockDesc* CTerrainBlockDesc::GetNeighbour(unsigned int id) const
{
	return _implementation->GetNeighbour(id);
}

//
// CTerrainBlockDesc::CTerrainBlockDescImplementation
//

CTerrainBlockDesc::CTerrainBlockDescImplementation::CTerrainBlockDescImplementation(CTerrainBlockDesc* in_pHolder)
	:	_pHolder(in_pHolder)
{
	for (int i = 0; i < 4; i++)
		_neighbours[i] = nullptr;
}

CTerrainBlockDesc::CTerrainBlockDescImplementation::~CTerrainBlockDescImplementation()
{
	for (CTerrainBlockDesc* pChildBlock : _vecChildBlocks)
	{
		delete pChildBlock;
	}
}

// �������������
void CTerrainBlockDesc::CTerrainBlockDescImplementation::Init(CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner, float in_fMinLattitude, float in_fMaxLattitude, float in_fMinLongitude, float in_fMaxLongitude, CTerrainBlockDesc* in_pParent)
{
	_pOwner = in_pOwner;

	_params.fMinLattitude = in_fMinLattitude;
	_params.fMaxLattitude = in_fMaxLattitude;

	_params.fMinLongitude = in_fMinLongitude;
	_params.fMaxLongitude = in_fMaxLongitude;

	_pParentBlock = in_pParent;
	_vecChildBlocks.clear();
}


// �������� ��� ����� ��������
const wchar_t* CTerrainBlockDesc::CTerrainBlockDescImplementation::GetTextureFileName() const
{
	/*static std::wstring wsTexFileName;

	wsTexFileName.clear();

	for (size_t iDepth = 0; iDepth < _params.uiDepth; iDepth++)
	{
		unsigned char ucLongIndex = _params.aTreePosition[iDepth].ucLongitudeIndex;
		unsigned char ucLatIndex = _params.aTreePosition[iDepth].ucLattitudeIndex;

		std::wstring wsXX = std::to_wstring(ucLatIndex);
		std::wstring wsYY = std::to_wstring(ucLongIndex);

		if (wsXX.size() == 1)
			wsXX = L"0" + wsXX;

		if (wsYY.size() == 1)
			wsYY = L"0" + wsYY;


		wsTexFileName += L"\\" + wsXX + L"_" + wsYY;
	}

	unsigned char ucLongIndex = _params.aTreePosition[_params.uiDepth].ucLongitudeIndex;
	unsigned char ucLatIndex = _params.aTreePosition[_params.uiDepth].ucLattitudeIndex;

	std::wstring wsXX = std::to_wstring(ucLatIndex);
	std::wstring wsYY = std::to_wstring(ucLongIndex);

	if (wsXX.size() == 1)
		wsXX = L"0" + wsXX;

	if (wsYY.size() == 1)
		wsYY = L"0" + wsYY;

	wsTexFileName += L"\\T_" + wsXX + L"_" + wsYY + L".dds";

	return wsTexFileName.c_str();*/

	return _wsTextureFileName.c_str();
}

// �������� ��� ����� ����� �����
const wchar_t* CTerrainBlockDesc::CTerrainBlockDescImplementation::GetHeightmapFileName() const
{
	/*static std::wstring wsTexFileName;

	wsTexFileName.clear();

	for (size_t iDepth = 0; iDepth < _params.uiDepth; iDepth++)
	{
		unsigned char ucLongIndex = _params.aTreePosition[iDepth].ucLongitudeIndex;
		unsigned char ucLatIndex = _params.aTreePosition[iDepth].ucLattitudeIndex;

		std::wstring wsXX = std::to_wstring(ucLatIndex);
		std::wstring wsYY = std::to_wstring(ucLongIndex);

		if (wsXX.size() == 1)
			wsXX = L"0" + wsXX;

		if (wsYY.size() == 1)
			wsYY = L"0" + wsYY;


		wsTexFileName += L"\\" + wsXX + L"_" + wsYY;
	}

	unsigned char ucLongIndex = _params.aTreePosition[_params.uiDepth].ucLongitudeIndex;
	unsigned char ucLatIndex = _params.aTreePosition[_params.uiDepth].ucLattitudeIndex;

	std::wstring wsXX = std::to_wstring(ucLatIndex);
	std::wstring wsYY = std::to_wstring(ucLongIndex);

	if (wsXX.size() == 1)
		wsXX = L"0" + wsXX;

	if (wsYY.size() == 1)
		wsYY = L"0" + wsYY;

	wsTexFileName += L"\\H_" + wsXX + L"_" + wsYY + L".dds";

	return wsTexFileName.c_str();*/

	return _wsHeightmapFileName.c_str();
}

// �������� ��������� �� ������������ ���� (���������� null, ���� �������� ��������)
const CTerrainBlockDesc* CTerrainBlockDesc::CTerrainBlockDescImplementation::GetParentBlockData() const
{
	return _pParentBlock;
}

// �������� ���������� �������� ������
unsigned int CTerrainBlockDesc::CTerrainBlockDescImplementation::GetChildBlockDataCount() const
{
	return static_cast<unsigned int>(_vecChildBlocks.size());
}

// �������� ��������� �� �������� ����
const CTerrainBlockDesc* CTerrainBlockDesc::CTerrainBlockDescImplementation::GetChildBlockData(unsigned int id) const
{
	return _vecChildBlocks[id];
}

// �������� ���������� 
/*static*/ CTerrainBlockDesc* CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(
	CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner,
	float in_fMinLattitude, 
	float in_fMaxLattitude, 
	float in_fMinLongitude, 
	float in_fMaxLongitude,
	CTerrainBlockDesc* in_pParent)
{
	CTerrainBlockDesc* pNewDataBlock = new CTerrainBlockDesc();
	pNewDataBlock->_implementation->Init(in_pOwner, in_fMinLattitude, in_fMaxLattitude, in_fMinLongitude, in_fMaxLongitude, in_pParent);

	return pNewDataBlock;
}

size_t CTerrainBlockDesc::CTerrainBlockDescImplementation::GetMemoryUsage() const
{
	return sizeof(CTerrainBlockDescImplementation) + sizeof(CTerrainBlockDesc);
}

void CTerrainBlockDesc::CTerrainBlockDescImplementation::GenerateChilds(const wchar_t* in_pcwszDirectoryName, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth, const std::vector<std::wstring>& vecTextures, const std::vector<std::wstring>& vecHeightmaps)
{
	// �������� ������ ������� - �������
	if (_params.uiDepth >= in_uiDepth)
		return;

	if (vecHeightmaps.empty() || vecTextures.empty())
	{
		LogMessage("Error loading heightmaps or textures - cannot generate block");
		return;
	}

	float fDeltaLongitude = (_params.fMaxLongitude - _params.fMinLongitude) / in_uiM;
	float fDeltaLattitude = (_params.fMaxLattitude - _params.fMinLattitude) / in_uiN;

	for (unsigned int uiXX = 0; uiXX < in_uiM; uiXX++)
	{
		float fChildMinLongitude = _params.fMinLongitude + fDeltaLongitude*uiXX;
		float fChildMaxLongitude = _params.fMinLongitude + fDeltaLongitude*uiXX + fDeltaLongitude;

		for (unsigned int uiYY = 0; uiYY < in_uiN; uiYY++)
		{
			std::wstring wsRandomHeightmap = vecHeightmaps[rand() % vecHeightmaps.size()];
			std::wstring wsRandomTexture = vecTextures[rand() % vecTextures.size()];

			float fChildMinLattitude = _params.fMinLattitude + fDeltaLattitude*uiYY;
			float fChildMaxLattitude = _params.fMinLattitude + fDeltaLattitude*uiYY + fDeltaLattitude;

			CTerrainBlockDesc* pChildBlock = CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(_pOwner,
				fChildMinLattitude, fChildMaxLattitude, fChildMinLongitude, fChildMaxLongitude, _pHolder);

			memcpy(pChildBlock->_implementation->_params.aTreePosition, _params.aTreePosition, sizeof(_params.aTreePosition));
			pChildBlock->_implementation->_params.uiDepth = _params.uiDepth + 1;

			pChildBlock->_implementation->_params.aTreePosition[_params.uiDepth + 1].ucLattitudeIndex = uiXX;
			pChildBlock->_implementation->_params.aTreePosition[_params.uiDepth + 1].ucLongitudeIndex = uiYY;

			_vecChildBlocks.push_back(pChildBlock);

			pChildBlock->_implementation->GenerateChilds(in_pcwszDirectoryName, in_uiM, in_uiN, in_uiDepth, vecTextures, vecHeightmaps);

			_pOwner->TerrainBlockCreated(pChildBlock);
		}
	}
}

void CTerrainBlockDesc::CTerrainBlockDescImplementation::ReadDataStructure(const std::wstring in_wsParentDir, const LodInfoStruct * in_pLodInfoArray, unsigned int in_uiMaxDepth)
{
	// �������� ������ ������� - �������
	if (_params.uiDepth >= in_uiMaxDepth)
		return;


	const LodInfoStruct& currentLodData = in_pLodInfoArray[_params.uiDepth];


	unsigned int uiCountY = currentLodData.CountX;
	unsigned int uiCountX = currentLodData.CountY;

	float fDeltaLongitude = (_params.fMaxLongitude - _params.fMinLongitude) / uiCountY;
	float fDeltaLattitude = (_params.fMaxLattitude - _params.fMinLattitude) / uiCountX;

	_vecChildBlocks.reserve(uiCountX * uiCountY);

	for (unsigned long uiXX = 0; uiXX < uiCountX; uiXX++)
	{
		float fChildMaxLattitude = _params.fMaxLattitude - fDeltaLattitude * uiXX;
		float fChildMinLattitude = _params.fMaxLattitude - fDeltaLattitude * uiXX - fDeltaLattitude;

		if (fChildMaxLattitude > -M_PI*0.5)
		{

			float fLattitudeCoeff = 1.f;
			float fCorrectedMinLattitude = fChildMinLattitude;

			if (fChildMinLattitude < -M_PI*0.5)
			{
				fLattitudeCoeff = (fChildMaxLattitude + M_PI*0.5) / (fChildMaxLattitude - fChildMinLattitude);
				fCorrectedMinLattitude = -M_PI*0.5;
			}

			for (unsigned long uiYY = 0; uiYY < uiCountY; uiYY++)
			{
				std::wstring wsXX = std::to_wstring(uiXX);
				std::wstring wsYY = std::to_wstring(uiYY);

				if (wsXX.size() == 1)
					wsXX = L"0" + wsXX;

				if (wsYY.size() == 1)
					wsYY = L"0" + wsYY;

				std::wstring wsTextureFileName = L"T_" + wsXX + L"_" + wsYY + L".dds";
				std::wstring wsHeightmapFileName = L"H_" + wsXX + L"_" + wsYY + L".dds";
				std::wstring wsChildsDirectory = in_wsParentDir + L"\\" + wsXX + L"_" + wsYY;

				std::wstring wsTexturePath = in_wsParentDir + L"\\" + wsTextureFileName;
				std::wstring wsHeightmapPath = in_wsParentDir + L"\\" + wsHeightmapFileName;
	
				
				float fChildMinLongitude = _params.fMinLongitude + fDeltaLongitude*uiYY;
				float fChildMaxLongitude = _params.fMinLongitude + fDeltaLongitude*uiYY + fDeltaLongitude;

				if (fChildMinLongitude < 2 * M_PI)
				{
					float fLongitudeCoeff = 1.f;
					float fCorrectedMaxLongitude = fChildMaxLongitude;

					if (fChildMaxLongitude > 2 * M_PI)
					{
						fLongitudeCoeff = (2 * M_PI - fChildMinLongitude) / (fChildMaxLongitude - fChildMinLongitude);
						fCorrectedMaxLongitude = 2 * M_PI;
					}

					CTerrainBlockDesc* pChildBlock = CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(_pOwner,
						fChildMinLattitude, fChildMaxLattitude, fChildMinLongitude, fChildMaxLongitude, _pHolder);

					memcpy(pChildBlock->_implementation->_params.aTreePosition, _params.aTreePosition, sizeof(_params.aTreePosition));

					pChildBlock->_implementation->_params.uiDepth = _params.uiDepth + 1;
					
					pChildBlock->_implementation->_wsTextureFileName = wsTexturePath;
					pChildBlock->_implementation->_wsHeightmapFileName = wsHeightmapPath;

					pChildBlock->_implementation->_params.aTreePosition[_params.uiDepth + 1].ucLattitudeIndex = uiXX;
					pChildBlock->_implementation->_params.aTreePosition[_params.uiDepth + 1].ucLongitudeIndex = uiYY;

					_vecChildBlocks.push_back(pChildBlock);

					if (PathFileExistsW(wsChildsDirectory.c_str()))
					{
						pChildBlock->_implementation->ReadDataStructure(wsChildsDirectory, in_pLodInfoArray, in_uiMaxDepth);
					}

					// This correction goes after loading childs recursively, because childs subdivision is occured according to "wrong" interval, that can be bigger than limits of lattitude and longitude

					pChildBlock->_implementation->_params.fLattitudeCutCoeff = fLattitudeCoeff;
					pChildBlock->_implementation->_params.fLongitude�utCoeff = fLongitudeCoeff;

					pChildBlock->_implementation->_params.fMinLattitude = fCorrectedMinLattitude;
					pChildBlock->_implementation->_params.fMaxLongitude = fCorrectedMaxLongitude;

					_pOwner->TerrainBlockCreated(pChildBlock);

				} // end if (fChildMinLongitude < 2 * M_PI)

			} // end for (unsigned long uiYY = 0; uiYY < uiCountY; uiYY++)

		} // end if (fChildMaxLattitude > -M_PI*0.5)

	} // end for (unsigned long uiXX = 0; uiXX < uiCountX; uiXX++)

}

void CTerrainBlockDesc::CTerrainBlockDescImplementation::LoadChildsFromDirectory(const std::wstring& in_wsDirectory)
{
	std::wstring wsDirectory = in_wsDirectory;

	if (wsDirectory.empty())
		return;

	wchar_t backChar = wsDirectory.back();

	if (backChar == L'\\' || backChar == L'/')
		wsDirectory = wsDirectory.substr(0, wsDirectory.size() - 1);

	std::wstring wsSearchRequest = wsDirectory + L"\\*";

	WIN32_FIND_DATA ffd;

	HANDLE hFind = INVALID_HANDLE_VALUE;

	hFind = FindFirstFileW(wsSearchRequest.c_str(), &ffd);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		LogMessage("CTerrainBlockDesc::LoadChildsFromDirectory('%ls'), ERROR: Directory is empty.", wsDirectory.c_str());
		return;
	}

	unsigned long uiCountX = 0;
	unsigned long uiCountY = 0;

	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		std::wstring wsFileName = ffd.cFileName;
		std::vector<std::wstring> vecFileNameParts;

		SplitByStringSeparatedList(wsFileName, L".", vecFileNameParts);

		if (vecFileNameParts.size() != 2)
		{
			LogMessage("CTerrainBlockDesc::LoadChildsFromDirectory('%ls'), ERROR: Illegal file name (%ls)", wsDirectory.c_str(), wsFileName.c_str());
			continue;
		}

		std::wstring wsFileNameWithoutExtension = vecFileNameParts[0];
		std::vector<std::wstring> vecBaseParts;

		SplitByStringSeparatedList(wsFileNameWithoutExtension, L"_", vecBaseParts);

		if (vecBaseParts.size() != 3)
		{
			LogMessage("CTerrainBlockDesc::LoadChildsFromDirectory('%ls'), ERROR: Illegal file name (%ls)", wsDirectory.c_str(), wsFileName.c_str());
			continue;
		}

		unsigned long uiCurXX = std::stoul(vecBaseParts[1]);
		unsigned long uiCurYY = std::stoul(vecBaseParts[2]);

		if (uiCurXX + 1 > uiCountX)
			uiCountX = uiCurXX + 1;

		if (uiCurYY + 1 > uiCountY)
			uiCountY = uiCurYY + 1;
	} 
	while (FindNextFileW(hFind, &ffd) != 0);

	DWORD dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		LogMessage("CTerrainBlockDesc::LoadChildsFromDirectory('%ls'), ERROR: FindFirstFile/FindNextFile", wsDirectory.c_str());
	}

	FindClose(hFind);

	if (uiCountX == 0 || uiCountY == 0)
	{
		LogMessage("CTerrainBlockDesc::LoadChildsFromDirectory('%ls'), ERROR: directory contents are illegal");
		return;
	}

	float fDeltaLongitude = (_params.fMaxLongitude - _params.fMinLongitude) / uiCountY;
	float fDeltaLattitude = (_params.fMaxLattitude - _params.fMinLattitude) / uiCountX;

	_vecChildBlocks.reserve(uiCountX * uiCountY);

	for (unsigned long uiXX = 0; uiXX < uiCountX; uiXX++)
	{
		float fChildMaxLattitude = _params.fMaxLattitude - fDeltaLattitude * uiXX;
		float fChildMinLattitude = _params.fMaxLattitude - fDeltaLattitude * uiXX - fDeltaLattitude;

		if (fChildMaxLattitude > -M_PI*0.5)
		{

			float fLattitudeCoeff = 1.f;
			float fCorrectedMinLattitude = fChildMinLattitude;

			if (fChildMinLattitude < -M_PI*0.5)
			{
				fLattitudeCoeff = (fChildMaxLattitude + M_PI*0.5) / (fChildMaxLattitude - fChildMinLattitude);
				fCorrectedMinLattitude = -M_PI*0.5;
			}

			for (unsigned long uiYY = 0; uiYY < uiCountY; uiYY++)
			{
				std::wstring wsXX = std::to_wstring(uiXX);
				std::wstring wsYY = std::to_wstring(uiYY);

				if (wsXX.size() == 1)
					wsXX = L"0" + wsXX;

				if (wsYY.size() == 1)
					wsYY = L"0" + wsYY;

				std::wstring wsTextureFileName = L"T_" + wsXX + L"_" + wsYY + L".dds";
				std::wstring wsHeightmapFileName = L"H_" + wsXX + L"_" + wsYY + L".dds";
				std::wstring wsChildsDirectory = wsDirectory + L"\\" + wsXX + L"_" + wsYY;

				std::wstring wsTexturePath = wsDirectory + L"\\" + wsTextureFileName;
				std::wstring wsHeightmapPath = wsDirectory + L"\\" + wsHeightmapFileName;

				if (!PathFileExistsW(wsTexturePath.c_str()))
				{
					LogMessage("CTerrainBlockDesc::LoadChildsFromDirectory('%ls'), ERROR: texture file (%ls) does not exist!", wsDirectory.c_str(), wsTextureFileName.c_str());
					wsTexturePath.clear();
				}

				if (!PathFileExistsW(wsHeightmapPath.c_str()))
				{
					LogMessage("CTerrainBlockDesc::LoadChildsFromDirectory('%ls'), ERROR: heightmap file (%ls) does not exist!", wsDirectory.c_str(), wsHeightmapFileName.c_str());
					wsHeightmapPath.clear();
				}

				float fChildMinLongitude = _params.fMinLongitude + fDeltaLongitude*uiYY;
				float fChildMaxLongitude = _params.fMinLongitude + fDeltaLongitude*uiYY + fDeltaLongitude;

				if (fChildMinLongitude < 2 * M_PI)
				{
					float fLongitudeCoeff = 1.f;
					float fCorrectedMaxLongitude = fChildMaxLongitude;

					if (fChildMaxLongitude > 2 * M_PI)
					{
						fLongitudeCoeff = (2 * M_PI - fChildMinLongitude) / (fChildMaxLongitude - fChildMinLongitude);
						fCorrectedMaxLongitude = 2 * M_PI;
					}


					CTerrainBlockDesc* pChildBlock = CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(_pOwner,
						fChildMinLattitude, fChildMaxLattitude, fChildMinLongitude, fChildMaxLongitude, _pHolder);

					memcpy(pChildBlock->_implementation->_params.aTreePosition, _params.aTreePosition, sizeof(_params.aTreePosition));

					pChildBlock->_implementation->_params.uiDepth = _params.uiDepth + 1;

					pChildBlock->_implementation->_wsTextureFileName = wsTexturePath;
					pChildBlock->_implementation->_wsHeightmapFileName = wsHeightmapPath;

					pChildBlock->_implementation->_params.aTreePosition[_params.uiDepth + 1].ucLattitudeIndex = uiXX;
					pChildBlock->_implementation->_params.aTreePosition[_params.uiDepth + 1].ucLongitudeIndex = uiYY;

					_vecChildBlocks.push_back(pChildBlock);

					if (PathFileExistsW(wsChildsDirectory.c_str()))
					{
						pChildBlock->_implementation->LoadChildsFromDirectory(wsChildsDirectory);
					}

					// This correction goes after loading childs recursively, because childs subdivision is occured according to "wrong" interval, that can be bigger than limits of lattitude and longitude

					pChildBlock->_implementation->_params.fLattitudeCutCoeff = fLattitudeCoeff;
					pChildBlock->_implementation->_params.fLongitude�utCoeff = fLongitudeCoeff;

					pChildBlock->_implementation->_params.fMinLattitude = fCorrectedMinLattitude;
					pChildBlock->_implementation->_params.fMaxLongitude = fCorrectedMaxLongitude;

					_pOwner->TerrainBlockCreated(pChildBlock);

				} // end if (fChildMinLongitude < 2 * M_PI)

			} // end for (unsigned long uiYY = 0; uiYY < uiCountY; uiYY++)

		} // end if (fChildMaxLattitude > -M_PI*0.5)

	} // end for (unsigned long uiXX = 0; uiXX < uiCountX; uiXX++)

}