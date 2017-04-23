#pragma once

#include <d3d11.h>

#ifndef TERRAINDATAMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

class TERRAINDATAMANAGER_API CTerrainBlockData
{
public:

	CTerrainBlockData();

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
	const CTerrainBlockData*		GetParentBlockData() const;

	// получить количество дочерних блоков
	unsigned int					GetChildBlockDataCount() const;

	// получить указатель на дочерний блок
	const CTerrainBlockData*		GetChildBlockData(unsigned int id) const;

	// загрузить ресурсы
	void							LoadResources();

	// выгрузить ресурсы
	void							UnloadResources();

	// получить текстуру с картой высот
	ID3D11ShaderResourceView*		GetHeighmap() const;

	// получить текстуру Земли
	ID3D11ShaderResourceView*		GetTexture() const;

protected:

	~CTerrainBlockData();

private:

	class CTerrainBlockDataImplementation;
	CTerrainBlockDataImplementation*	_implementation = nullptr;

	friend class CTerrainDataManager;
};

class TERRAINDATAMANAGER_API CTerrainDataManager
{
public:

	CTerrainDataManager();
	~CTerrainDataManager();

	// Инициализировать
	void	Init(ID3D11Device* in_pD3D11Device, ID3D11DeviceContext* in_pD3D11DeviceContext);

	// Загрузить описание данных поверхности Земли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockData** out_ppRootDataBlock);

	// Освободить загруженное описание данных
	void	ReleaseTerrainDataInfo(CTerrainBlockData* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;

	friend class CTerrainBlockData::CTerrainBlockDataImplementation;
};
