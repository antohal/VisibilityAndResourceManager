#pragma once

#include "TerrainDataManager.h"

#include "C3DBaseObject.h"
#include "C3DBaseObjectManager.h"

#include "VisibilityManager.h"
#include "vecmath.h"

#include <map>
#include <set>


#ifndef TERRAINMANAGER_EXPORTS
#define TERRAINVISIBILITYMANAGER_API __declspec(dllimport)
#else
#define TERRAINVISIBILITYMANAGER_API __declspec(dllexport)
#endif

// базовый менеджер объектов поверхности Земли
class C3DBaseTerrainObjectManager : public C3DBaseObjectManager
{
public:

	// получить данные блока по объекту
	virtual const CTerrainBlockDesc* GetTerrainDataForObject(C3DBaseObject* pObject) const = 0;

	// получить указатель на корневой блок [с корневым узлом не должно быть связано никаких объектов, он служит как хранилище]
	virtual const CTerrainBlockDesc* GetRootTerrainData() const = 0;
};

class TERRAINVISIBILITYMANAGER_API CTerrainVisibilityManager : public IVisibilityManagerPlugin
{
public:

	CTerrainVisibilityManager();
	~CTerrainVisibilityManager();

	void Init(C3DBaseTerrainObjectManager* in_pMeshTree, float in_fWorldScale, float in_fMaximumDistance, float in_fLodDistCoeff, unsigned int in_uiMaxDepth);

	//@{ IVisibilityManagerPlugin
	bool IsObjectVisible(C3DBaseObject* in_pObject) const;
	void UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);
	unsigned int GetVisibleObjectsCount() const override;
	C3DBaseObject* GetVisibleObject(unsigned int i) override;

	//@}



	class CTerrainVisibilityManagerImpl;
	CTerrainVisibilityManagerImpl*	_implementation = nullptr;
};