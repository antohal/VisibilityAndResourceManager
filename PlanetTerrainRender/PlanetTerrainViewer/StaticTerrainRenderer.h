#pragma once

#include <string>
#include <list>
#include <vector>

#include "Scene.h"

#include "C3DBaseObject.h"
#include "C3DBaseFaceSet.h"
#include "C3DBaseMaterial.h"
#include "C3DBaseObjectManager.h"

#include "TerrainDataManager.h"

#include "D3DX10.h"

class CD3DStaticTerrainRenderer;
class CD3DStaticTerrainMaterial;
class CD3DStaticTerrainFaceset;
class HeightfieldConverter;
class CDirect3DSystem;

class CD3DStaticTerrainObject : public C3DBaseObject
{
public:

	CD3DStaticTerrainObject(CD3DStaticTerrainRenderer* in_pOwner);

	void	SetFaceset(CD3DStaticTerrainFaceset*);

	//@{ C3DBaseObject

	// ������� ���������� ���������� �� �������
	virtual float GetDistance(const D3DXVECTOR3* in_pvPointFrom) const;

	// �������� ����������� ���������� ��������� �� �������
	virtual float GetMinimalVisibleDistance() const;

	// �������� ������������ ���������� ��������� �� �������
	virtual float GetMaximalVisibleDistance() const;

	// �������� ������������ ������-���
	virtual C3DBaseObject*	GetParentLODObject();

	// �������� ���������� �������� ��������-�����
	virtual unsigned int	GetNumChildLODObjects();

	// �������� �������� ���-������
	virtual C3DBaseObject*	GetChildLODObject(unsigned int id);

	// ��� 3D ������� ������ ����� ���������� �����-����. ������, ���� ������ - �����, � �� ���, ��
	// ����� ������ ���������� �������� � out_vBBMin � out_vBBMax.
	virtual void							GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax) override;

	// �������� ������� �������������
	virtual D3DXMATRIX*						GetWorldTransform() override;

	// ������� ������ ����������: ��������-�� �������� ������� ������� �� ������
	virtual bool							IsMinimalSizeCheckEnabled() const override { return false; };

	// ������� ���������� ���������� ����� ������� �������
	virtual size_t							GetMeshesCount() const override { return 0; }

	// ������� ���������� ���������� ��� ������� �� ��� ��������������
	virtual C3DBaseMesh*					GetMeshById(size_t id) const { return nullptr; }

	// �������� ������ ���������
	virtual size_t							GetFaceSetsCount() const { return 1; }
	virtual C3DBaseFaceSet*					GetFaceSetById(size_t id) const;

	virtual C3DBaseManager*					GetManager() const { return nullptr; }

	//@}

private:

	CD3DStaticTerrainFaceset*				_pFaceset = nullptr;

	D3DXMATRIX								_mTransform;
	D3DXVECTOR3								_vBBoxMin;
	D3DXVECTOR3								_vBBoxMax;

	CD3DStaticTerrainRenderer*				_owner = nullptr;
};

class CD3DStaticTerrainFaceset : public C3DBaseFaceSet
{
public:

	CD3DStaticTerrainFaceset(CD3DStaticTerrainObject* in_pObject, CD3DStaticTerrainMaterial* in_pMaterial, const CTerrainBlockData* in_pTerrainBlockData);

	//@{ C3DBaseFaceSet

	// �������� ������ �� ��������
	virtual C3DBaseMaterial*				GetMaterialRef() override;

	virtual C3DBaseManager*					GetManager() const;

	const CTerrainBlockData*				GetTerrainBlockData() const;

	//@}

	void	Load();
	void	Unload();

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

	// ������� ������������ � �������� ���������� [��� �������������� �������� ������� � Visman]
	virtual void	AddVisibleFaceSet(C3DBaseFaceSet*, unsigned int in_uiEyeId) override;

	// �������� ����� ������
	virtual size_t	GetTechniquesCount() const override { return 0; }
	virtual C3DBaseTechnique*	GetTechniqueById(size_t id) const override { return nullptr; }

	// �������� ������ �������
	virtual size_t	GetTexturesCount() const override { return 0; }
	virtual C3DBaseTexture*	GetTextureById(size_t id) const override { return nullptr; }

	virtual C3DBaseManager*					GetManager() const;

	void	Load();
	void	Unload();

private:

	CD3DStaticTerrainRenderer*				_pOwner = nullptr;

	std::wstring							_wsTextureFileName;

	std::set<CD3DStaticTerrainFaceset*>		_setVisibleFacesets;
};


class CD3DStaticTerrainRenderer : public CD3DSceneRenderer
{
public:

	CD3DStaticTerrainRenderer();
	~CD3DStaticTerrainRenderer();

	void			Init(CDirect3DSystem* in_pSystem);

	void			LoadPlanet(const wchar_t* in_pcwszDirectory);

	void			AddVisibleMaterial(CD3DStaticTerrainMaterial*);


	//@{ C3DBaseManager

	// ��������� �������� �������
	virtual void RequestLoadResource(C3DBaseResource* in_pResource);

	// ��������� �������� �������
	virtual void RequestUnloadResource(C3DBaseResource* in_pResource);

	//@}

	HeightfieldConverter*	GetHeightfieldConverter() { return _pHeightfieldConverter; }

protected:

	//@{ C3DBaseObjectManager

	// �������� ������ ��������
	virtual size_t GetObjectsCount() const override;
	virtual C3DBaseObject*	GetObjectByIndex(size_t id) const override;

	//@}

	//@{ CD3DSceneRenderer

	virtual void	Render(CD3DGraphicsContext* in_pContext) override;

	virtual float	GetWorldRadius() const;
	virtual float	GetMinCellSize() const;

	//@}

private:

	void			CreateObjects();
	void			CreateObjectsRecursive(const CTerrainBlockData* in_pData);
	void			CreateObject(const CTerrainBlockData* in_pData);

	CTerrainBlockData*						_pPlanetTerrainData = nullptr;
	CTerrainDataManager*					_pTerrainDataManager = nullptr;

	HeightfieldConverter*					_pHeightfieldConverter = nullptr;

	std::set<CD3DStaticTerrainMaterial*>	_setVisibleMaterials;
	std::vector<CD3DStaticTerrainObject*>	_vecTerrainObjects;
	std::list<CD3DStaticTerrainMaterial*>	_lstMaterials;
	std::list<CD3DStaticTerrainFaceset*>	_lstFacesets;
};