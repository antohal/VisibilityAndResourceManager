#pragma once

#include <string>
#include <list>

#include "Scene.h"

#include "C3DBaseObject.h"
#include "C3DBaseFaceSet.h"
#include "C3DBaseMaterial.h"

#include "TerrainDataManager.h"

class CD3DStaticTerrainRenderer;
class CD3DStaticTerrainMaterial;
class CD3DStaticTerrainFaceset;

class CD3DStaticTerrainObject : public C3DBaseObject
{
public:

	void	SetFaceset(CD3DStaticTerrainFaceset*);

	//@{ C3DBaseObject

	// Все 3D объекты должны будут возвращать Баунд-Бокс. Причем, если объект - точка, а не меш, то
	// пусть вернет одинаковые значения в out_vBBMin и out_vBBMax.
	virtual void							GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax) override;

	// Получить матрицу трансформации
	virtual D3DXMATRIX*						GetWorldTransform() override;

	// Функция должна возвращать: включена-ли проверка размера объекта на экране
	virtual bool							IsMinimalSizeCheckEnabled() const override { return false; };

	// Функция возврящает количество мешей данного объекта
	virtual size_t							GetMeshesCount() const override { return 0; }

	// Функция возвращает конкретный меш объекта по его идентификатору
	virtual C3DBaseMesh*					GetMeshById(size_t id) const { return nullptr; }

	// получить список фейссетов
	virtual size_t							GetFaceSetsCount() const { return 1; }
	virtual C3DBaseFaceSet*					GetFaceSetById(size_t id) const { if (id == 0) return _pFaceset; }

	virtual C3DBaseManager*					GetManager() const { return nullptr; }

	//@}

private:

	CD3DStaticTerrainFaceset*				_pFaceset = nullptr;
};

class CD3DStaticTerrainFaceset : public C3DBaseFaceSet
{
public:

	CD3DStaticTerrainFaceset(CD3DStaticTerrainObject* in_pObject, CD3DStaticTerrainMaterial* in_pMaterial, const CTerrainBlockData* in_pTerrainBlockData);

	//@{ C3DBaseFaceSet

	// получить ссылку на материал
	virtual C3DBaseMaterial*				GetMaterialRef() override;

	virtual C3DBaseManager*					GetManager() const { 
	
		if (!_pMaterialRef)
			return nullptr;

		return _pMaterialRef->GetManager();
	}

	//@}

private:

	CD3DStaticTerrainMaterial*				_pMaterialRef = nullptr;
	CD3DStaticTerrainObject*				_pTerrainObject = nullptr;
	const CTerrainBlockData*				_pTerrainBlockData = nullptr;
};

class CD3DStaticTerrainMaterial : public C3DBaseMaterial
{
public:

	CD3DStaticTerrainMaterial(CD3DStaticTerrainRenderer* in_pOwner, const std::wstring& in_wsFileName);

	void	Render(CD3DGraphicsContext* in_pContext);

	// Функция используется в процессе рендеринга [для взаимодействия процесса рендера с Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*, unsigned int in_uiEyeId) override;

	// Получить набор техник
	virtual size_t	GetTechniquesCount() const override { return 0; }
	virtual C3DBaseTechnique*	GetTechniqueById(size_t id) const override { return nullptr; }

	// получить список текстур
	virtual size_t	GetTexturesCount() const override { return 0; }
	virtual C3DBaseTexture*	GetTextureById(size_t id) const override { return nullptr; }

	virtual C3DBaseManager*					GetManager() const { return _pOwner; }

private:

	CD3DStaticTerrainRenderer*				_pOwner = nullptr;

	std::wstring							_wsTextureFileName;

	std::set<CD3DStaticTerrainFaceset*>		_setVisibleFacesets;
};


class CD3DStaticTerrainRenderer : public CD3DSceneRenderer, public C3DBaseManager
{
public:

	CD3DStaticTerrainRenderer();
	~CD3DStaticTerrainRenderer();

	void			LoadPlanet(const wchar_t* in_pcwszDirectory);

	void			AddVisibleMaterial(CD3DStaticTerrainMaterial*);

	//@{ CD3DSceneRenderer

	virtual void	GetObjects(std::list<C3DBaseObject*>& out_lstObjects) const;
	virtual void	Render(CD3DGraphicsContext* in_pContext) override;

	//@}


	//@{ C3DBaseManager

	// Запросить загрузку ресурса
	virtual void RequestLoadResource(C3DBaseResource*);

	// запросить выгрузку ресурса
	virtual void RequestUnloadResource(C3DBaseResource*);

	//@}

private:

	void			CreateObjects();
	void			CreateObjectsRecursive(const CTerrainBlockData* in_pData);
	void			CreateObject(const CTerrainBlockData* in_pData);

	CTerrainBlockData*						_pPlanetTerrainData = nullptr;
	CTerrainDataManager*					_pTerrainDataManager = nullptr;

	std::set<CD3DStaticTerrainMaterial*>	_setVisibleMaterials;
	std::list<CD3DStaticTerrainObject*>		_lstTerrainObjects;
	std::list<CD3DStaticTerrainMaterial*>	_lstMaterials;
	std::list<CD3DStaticTerrainFaceset*>	_lstFacesets;
};