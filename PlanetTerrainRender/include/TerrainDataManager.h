#pragma once

#include <d3d11.h>

#ifndef TERRAINDATAMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

class CTerrainDataBlockInfo
{
public:

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
	const CTerrainDataBlockInfo*	GetParentBlockInfo() const;

	// получить количество дочерних блоков
	unsigned int					GetChildBlockInfoCount() const;

	// получить указатель на дочерний блок
	const CTerrainDataBlockInfo*	GetChildBlockInfo(unsigned int id) const;

private:

	class CTerrainDataBlockInfoImplementation;
	CTerrainDataBlockInfoImplementation*	_implementation = nullptr;
};

class TERRAINDATAMANAGER_API CTerrainDataManager
{
public:

	CTerrainDataManager();
	~CTerrainDataManager();

	// инициализация
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext);

	// Загрузить описание данных поверхности Земли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainDataBlockInfo** out_ppRootDataBlock);

	// Освободить загруженное описание данных
	void	ReleaseTerrainDataInfo(CTerrainDataBlockInfo* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;
};
