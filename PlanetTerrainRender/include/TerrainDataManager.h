#pragma once


#ifndef TERRAINMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

// Описание блока Земли
class TERRAINDATAMANAGER_API CTerrainBlockDesc
{
public:

	CTerrainBlockDesc();

	//@{ получить минимальные значения по долготе и широте
	float							GetMinimumLattitude() const;
	float							GetMaximumLattitude() const;

	float							GetMinimumLongitude() const;
	float							GetMaximumLongitude() const;
	//@}

	// получить имя файла текстуры
	const wchar_t*					GetTextureFileName() const;

	// получить имя файла карты высот
	const wchar_t*					GetHeightmapFileName() const;

	// получить указатель на родительский блок (возвращает null, если является корневым)
	const CTerrainBlockDesc*		GetParentBlockDesc() const;

	// получить количество дочерних блоков
	unsigned int					GetChildBlockDescCount() const;

	// получить указатель на дочерний блок
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

	// Загрузить описание данных поверхности Земли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock);

	// Освободить загруженное описание данных
	void	ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;

	friend class CTerrainBlockDesc::CTerrainBlockDescImplementation;
};


class CTerrainManagerClient
{
public:

	virtual size_t	CreateObject(CTerrainBlockData*) = 0;
	virtual void	DeleteObject(size_t ID) = 0;

	virtual void	ObjectBecomeVisible(size_t ID) = 0;
	virtual void	ObjectBecomeInvisible(size_t ID) = 0;
};

class CTerrainManager
{
	// тут будет все
};
