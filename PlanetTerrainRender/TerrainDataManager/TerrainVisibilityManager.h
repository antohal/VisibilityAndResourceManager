#pragma once

#include "TerrainDataManager.h"

#include "C3DBaseObject.h"
#include "C3DBaseObjectManager.h"

#include "VisibilityManager.h"
#include "vecmath.h"

#include <map>
#include <set>


// базовый менеджер объектов поверхности Земли
class C3DBaseTerrainObjectManager : public C3DBaseObjectManager
{
public:

	// получить данные блока по объекту
	virtual const CTerrainBlockDesc* GetTerrainDataForObject(C3DBaseObject* pObject) const = 0;

	// получить указатель на корневой блок [с корневым узлом не должно быть связано никаких объектов, он служит как хранилище]
	virtual const CTerrainBlockDesc* GetRootTerrainData() const = 0;
};

class CTerrainVisibilityManager : public IVisibilityManagerPlugin
{
public:

	void Init(C3DBaseTerrainObjectManager* in_pMeshTree, float in_fWorldScale);

	//@{ IVisibilityManagerPlugin
	bool IsObjectVisible(C3DBaseObject* in_pObject) const;
	void UpdateObjectsVisibility(const Vector3& in_vPos, const Vector3& in_vDir, const Vector3& in_vUp, D3DMATRIX* in_pmProjection);
	//@}

private:

	void UpdateVisibilityRecursive(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos);
	void AddVisibleBlock(const CTerrainBlockDesc*);
	bool IsFar(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos) const;
	bool IsSomeChildVisible(const CTerrainBlockDesc*, const vm::Vector3df& in_vPos) const;

	std::map<C3DBaseObject*, const CTerrainBlockDesc*>	_mapTerrainBlockInfo;
	std::map<const CTerrainBlockDesc*, C3DBaseObject*>	_mapObjects;

	std::set<C3DBaseObject*> _setVisibleObjects;

	const CTerrainBlockDesc* _pRoot = nullptr;

	float	_fWorldScale = 1;
};