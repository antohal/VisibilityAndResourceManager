#pragma once

#include "C3DBaseObject.h"
#include "C3DBaseObjectManager.h"
#include "VisibilityManager.h"

#ifndef TERRAINDATAMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

class TERRAINDATAMANAGER_API CTerrainBlockData
{
public:

	CTerrainBlockData();

	//@{ получить минимальные значени€ по долготе и широте
	float							GetMinimumLattitude() const;
	float							GetMaximumLattitude() const;

	float							GetMinimumLongitude() const;
	float							GetMaximumLongitude() const;
	//@}

	// получить им€ файла текстуры
	const wchar_t*					GetTextureFileName() const;

	// получить им€ файла карты высот
	const wchar_t*					GetHeightmapFileName() const;

	// получить указатель на родительский блок (возвращает null, если €вл€етс€ корневым)
	const CTerrainBlockData*		GetParentBlockData() const;

	// получить количество дочерних блоков
	unsigned int					GetChildBlockDataCount() const;

	// получить указатель на дочерний блок
	const CTerrainBlockData*		GetChildBlockData(unsigned int id) const;

protected:

	~CTerrainBlockData();

private:

	class CTerrainBlockDataImplementation;
	CTerrainBlockDataImplementation*	_implementation = nullptr;

	friend class CTerrainDataManager;
};

// базовый менеджер объектов поверхности «емли
class C3DBaseTerrainObjectManager : public C3DBaseObjectManager
{
public:

	virtual const CTerrainBlockData* GetTerrainDataForObject(C3DBaseObject* pObject) const = 0;
};

class TERRAINDATAMANAGER_API CTerrainVisibilityManager : public IVisibilityManagerPlugin
{
public:

	CTerrainVisibilityManager();
	~CTerrainVisibilityManager();

	void	Init(C3DBaseTerrainObjectManager* in_pMeshTree);

	//@{ IVisibilityManagerPlugin
	virtual bool IsObjectVisible(C3DBaseObject* in_pObject) const override;
	virtual void UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection) override;
	//@}

private:

	class CTerrainVisibilityManagerImplementation;
	CTerrainVisibilityManagerImplementation*	_implementation;
};

class TERRAINDATAMANAGER_API CTerrainDataManager
{
public:

	CTerrainDataManager();
	~CTerrainDataManager();

	// «агрузить описание данных поверхности «емли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockData** out_ppRootDataBlock);

	// ќсвободить загруженное описание данных
	void	ReleaseTerrainDataInfo(CTerrainBlockData* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;

	friend class CTerrainBlockData::CTerrainBlockDataImplementation;
};
