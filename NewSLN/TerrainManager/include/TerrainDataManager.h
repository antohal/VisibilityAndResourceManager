#pragma once

#include <utility>

#ifndef TERRAINMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

// максимальная глубина дерева
#define MAX_TERRAIN_TREE_DEPTH	20

// индексы террейн блока на текущем уровне
struct STerrainBlockIndex
{
	unsigned char	ucLongitudeIndex = 255;
	unsigned char	ucLattitudeIndex = 255;
};

// параметры блока Земли
struct STerrainBlockParams
{
	// минимальная и максимальная долгота
	float				fMinLongitude = 0;
	float				fMaxLongitude = 0;

	// минимальная и максимальная широта
	float				fMinLattitude = 0;
	float				fMaxLattitude = 0;

	// коэффициенты обрезки
	float				fLongitudeСutCoeff = 1;
	float				fLattitudeCutCoeff = 1;

	// уровень глубины блока
	unsigned int		uiDepth = 0;

	// положение в дереве (индекс по долготе и широте по каждому уровню глубины). Для уровня глубины больше текущего будут значения по умолчанию = 255
	STerrainBlockIndex	aTreePosition[MAX_TERRAIN_TREE_DEPTH];
};


// Описание блока Земли
class TERRAINDATAMANAGER_API CTerrainBlockDesc
{
public:

	CTerrainBlockDesc();

	const STerrainBlockParams*		GetParams() const;

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
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, float in_fLongitudeScaleCoeff, float in_fLattitudeScaleCoeff, unsigned int* out_uiMaximumDepth = nullptr);

	// Сгенерировать данные планеты. Текстуры и карты нормалей будут браться из директории [in_pcwszDirectoryName] случайным образом 
	// Карты нормалей из HeightMaps, текстуры из Textures
	// in_uiM, in_uiN - количество разбиений каждого уровня по широте и долготе
	// in_uiDepth - глубина дерева
	void	GenerateTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock, unsigned int in_uiM, unsigned int in_uiN, unsigned int in_uiDepth);

	// Освободить загруженное описание данных
	void	ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;

	friend class CTerrainBlockDesc::CTerrainBlockDescImplementation;
};
