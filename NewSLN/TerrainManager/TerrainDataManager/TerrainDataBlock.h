#pragma once

#include "TerrainDataManager.h"
#include "TerrainDataManagerImpl.h"

#include <string>
#include <vector>

#include <atlbase.h>

class CTerrainBlockDesc::CTerrainBlockDescImplementation
{
public:

	CTerrainBlockDescImplementation(CTerrainBlockDesc* in_pHolder);
	~CTerrainBlockDescImplementation();

	// инициализация
	void									Init(CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner, float in_fMinLattitude, float in_fMaxLattitude, float in_fMinLongitude, float in_fMaxLongitude,
		const std::wstring& in_wsTextureFileName, const std::wstring& in_wsHeightmapFileName, CTerrainBlockDesc* in_pParent);

	//@{ получить минимальные значения по долготе и широте
	float									GetMinimumLattitude() const;
	float									GetMaximumLattitude() const;

	float									GetMinimumLongitude() const;
	float									GetMaximumLongitude() const;
	//@}

	// получить имя файла текстуры
	const wchar_t*							GetTextureFileName() const;

	// получить имя файла карты высот
	const wchar_t*							GetHeightmapFileName() const;

	// получить указатель на родительский блок (возвращает null, если является корневым)
	const CTerrainBlockDesc*				GetParentBlockData() const;

	// получить количество дочерних блоков
	unsigned int							GetChildBlockDataCount() const;

	// получить указатель на дочерний блок
	const CTerrainBlockDesc*				GetChildBlockData(unsigned int id) const;

	// загрузить дочерние блоки из указанной директории
	void									LoadChildsFromDirectory(const std::wstring& in_wsDirectory);

	// сгенерировать дочерние узлы случайным образом
	void									GenerateChilds(const wchar_t* in_pcwszDirectoryName, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth, const std::vector<std::wstring>& vecTextures, const std::vector<std::wstring>& vecHeightmaps);

	unsigned int							Depth() const { return _uiDepth; }

	// создание экземпляра [статическая]
	static CTerrainBlockDesc*				CreateTerrainBlockDataInstance(CTerrainDataManager::CTerrainDataManagerImplementation* in_pOwner,
		float in_fMinLattitude, float in_fMaxLattitude, float in_fMinLongitude, float in_fMaxLongitude, const std::wstring& in_wsTextureFileName, 
		const std::wstring& in_wsHeightmapFileName, CTerrainBlockDesc* in_pParent);

	size_t									GetMemoryUsage() const;

private:

	float									_fMinLattitude = 0;
	float									_fMaxLattitude = 0;

	float									_fMinLongitude = 0;
	float									_fMaxLongitude = 0;

	unsigned int							_uiDepth = 0;

	std::wstring							_wsTextureFileName;
	std::wstring							_wsHeightmapFileName;

	CTerrainBlockDesc*						_pParentBlock = nullptr;
	std::vector<CTerrainBlockDesc*>			_vecChildBlocks;

	CTerrainDataManager::CTerrainDataManagerImplementation*		_pOwner = nullptr;
	CTerrainBlockDesc*						_pHolder = nullptr;

	friend class CTerrainDataManager::CTerrainDataManagerImplementation;
};
