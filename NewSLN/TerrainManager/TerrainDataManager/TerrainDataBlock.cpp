#include "TerrainDataBlock.h"

#include <strsafe.h>
#include <Windows.h>

#include "Log.h"
#include "StlUtil.h"

CTerrainBlockDesc::CTerrainBlockDesc()
{
	_implementation = new CTerrainBlockDescImplementation(this);
}

CTerrainBlockDesc::~CTerrainBlockDesc()
{
	delete _implementation;
}

//@{ получить минимальные значения по долготе и широте
float CTerrainBlockDesc::GetMinimumLattitude() const
{
	return _implementation->GetMinimumLattitude();
}

float CTerrainBlockDesc::GetMaximumLattitude() const
{
	return _implementation->GetMaximumLattitude();
}

float CTerrainBlockDesc::GetMinimumLongitude() const
{
	return _implementation->GetMinimumLongitude();
}

float CTerrainBlockDesc::GetMaximumLongitude() const
{
	return _implementation->GetMaximumLongitude();
}
//@}

// получить имя файла текстуры
const wchar_t* CTerrainBlockDesc::GetTextureFileName() const
{
	return _implementation->GetTextureFileName();
}

// получить имя файла карты высот
const wchar_t* CTerrainBlockDesc::GetHeightmapFileName() const
{
	return _implementation->GetHeightmapFileName();
}

// получить указатель на родительский блок (возвращает null, если является корневым)
const CTerrainBlockDesc* CTerrainBlockDesc::GetParentBlockDesc() const
{
	return _implementation->GetParentBlockData();
}

// получить количество дочерних блоков
unsigned int CTerrainBlockDesc::GetChildBlockDescCount() const
{
	return _implementation->GetChildBlockDataCount();
}

// получить указатель на дочерний блок
const CTerrainBlockDesc* CTerrainBlockDesc::GetChildBlockDesc(unsigned int id) const
{
	return _implementation->GetChildBlockData(id);
}




//
// CTerrainBlockDesc::CTerrainBlockDescImplementation
//

CTerrainBlockDesc::CTerrainBlockDescImplementation::CTerrainBlockDescImplementation(CTerrainBlockDesc* in_pHolder)
	:	_pHolder(in_pHolder)
{

}

CTerrainBlockDesc::CTerrainBlockDescImplementation::~CTerrainBlockDescImplementation()
{
	for (CTerrainBlockDesc* pChildBlock : _vecChildBlocks)
	{
		delete pChildBlock;
	}
}

// инициализация
void CTerrainBlockDesc::CTerrainBlockDescImplementation::Init(CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner, float in_fMinLattitude, float in_fMaxLattitude, float in_fMinLongitude, float in_fMaxLongitude,
	const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName, CTerrainBlockDesc* in_pParent)
{
	_pOwner = in_pOwner;

	_fMinLattitude = in_fMinLattitude;
	_fMaxLattitude = in_fMaxLattitude;

	_fMinLongitude = in_fMinLongitude;
	_fMaxLongitude = in_fMaxLongitude;

	_wsTextureFileName = in_wsTextureFileName;
	_wsHeightmapFileName = in_wsHeightmapFileName;

	_pParentBlock = in_pParent;
	_vecChildBlocks.clear();
}

//@{ получить минимальные значения по долготе и широте
float CTerrainBlockDesc::CTerrainBlockDescImplementation::GetMinimumLattitude() const
{
	return _fMinLattitude;
}

float CTerrainBlockDesc::CTerrainBlockDescImplementation::GetMaximumLattitude() const
{
	return _fMaxLattitude;
}

float CTerrainBlockDesc::CTerrainBlockDescImplementation::GetMinimumLongitude() const
{
	return _fMinLongitude;
}

float CTerrainBlockDesc::CTerrainBlockDescImplementation::GetMaximumLongitude() const
{
	return _fMaxLongitude;
}
//@}

// получить имя файла текстуры
const wchar_t* CTerrainBlockDesc::CTerrainBlockDescImplementation::GetTextureFileName() const
{
	return _wsTextureFileName.c_str();
}

// получить имя файла карты высот
const wchar_t* CTerrainBlockDesc::CTerrainBlockDescImplementation::GetHeightmapFileName() const
{
	return _wsHeightmapFileName.c_str();
}

// получить указатель на родительский блок (возвращает null, если является корневым)
const CTerrainBlockDesc* CTerrainBlockDesc::CTerrainBlockDescImplementation::GetParentBlockData() const
{
	return _pParentBlock;
}

// получить количество дочерних блоков
unsigned int CTerrainBlockDesc::CTerrainBlockDescImplementation::GetChildBlockDataCount() const
{
	return static_cast<unsigned int>(_vecChildBlocks.size());
}

// получить указатель на дочерний блок
const CTerrainBlockDesc* CTerrainBlockDesc::CTerrainBlockDescImplementation::GetChildBlockData(unsigned int id) const
{
	return _vecChildBlocks[id];
}

// создание экземпляра 
/*static*/ CTerrainBlockDesc* CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(
	CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner,
	float in_fMinLattitude, 
	float in_fMaxLattitude, 
	float in_fMinLongitude, 
	float in_fMaxLongitude,
	const std::wstring& in_wsTextureFileName, 
	const std::wstring& in_wsHeightmapFileName, 
	CTerrainBlockDesc* in_pParent)
{
	CTerrainBlockDesc* pNewDataBlock = new CTerrainBlockDesc();
	pNewDataBlock->_implementation->Init(in_pOwner, in_fMinLattitude, in_fMaxLattitude, in_fMinLongitude, in_fMaxLongitude, in_wsTextureFileName, in_wsHeightmapFileName, in_pParent);

	return pNewDataBlock;
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

	float fDeltaLongitude = (_fMaxLongitude - _fMinLongitude) / uiCountX;
	float fDeltaLattitude = (_fMaxLattitude - _fMinLattitude) / uiCountY;

	_vecChildBlocks.reserve(uiCountX * uiCountY);

	for (unsigned long uiXX = 0; uiXX < uiCountX; uiXX++)
	{
		float fChildMinLongitude = _fMinLongitude + fDeltaLongitude*uiXX;
		float fChildMaxLongitude = _fMinLongitude + fDeltaLongitude*uiXX + fDeltaLongitude;

		for (unsigned long uiYY = 0; uiYY < uiCountY; uiYY++)
		{
			std::wstring wsTextureFileName = L"T_" + std::to_wstring(uiXX) + L"_" + std::to_wstring(uiYY) + L".dds";
			std::wstring wsHeightmapFileName = L"N_" + std::to_wstring(uiXX) + L"_" + std::to_wstring(uiYY) + L".dds";
			std::wstring wsChildsDirectory = wsDirectory + L"\\" + std::to_wstring(uiXX) + L"_" + std::to_wstring(uiYY);

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

			float fChildMinLattitude = _fMinLattitude + fDeltaLattitude*uiYY;
			float fChildMaxLattitude = _fMinLattitude + fDeltaLattitude*uiYY + fDeltaLattitude;

			CTerrainBlockDesc* pChildBlock = CTerrainBlockDesc::CTerrainBlockDescImplementation::CreateTerrainBlockDataInstance(_pOwner,
				fChildMinLattitude, fChildMaxLattitude, fChildMinLongitude, fChildMaxLongitude, wsTexturePath, wsHeightmapPath, _pHolder);

			pChildBlock->_implementation->_uiDepth = _uiDepth + 1;

			_vecChildBlocks.push_back(pChildBlock);


			if (PathFileExistsW(wsChildsDirectory.c_str()))
			{
				pChildBlock->_implementation->LoadChildsFromDirectory(wsChildsDirectory);
			}
		}
	}
}
