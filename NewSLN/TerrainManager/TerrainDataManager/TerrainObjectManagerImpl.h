#pragma once

#include "TerrainDataManager.h"
#include "TerrainObjectManager.h"
#include "HeightfieldConverter.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"
#include "TerrainVisibilityManager.h"

#include <set>
#include <map>
#include <mutex>

class CInternalTerrainObject : public C3DBaseObject
{
public:

	CInternalTerrainObject(C3DBaseManager* in_pOwner, TerrainObjectID ID, const CTerrainBlockDesc* in_pBlockDesc, const STriangulationCoordsInfo& in_coordsInfo);

	const TerrainObjectID&	GetID() const {
		return _ID;
	}

	const CTerrainBlockDesc*	GetDesc() const {
		return _pBlockDesc;
	}

protected:

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
	virtual size_t							GetFaceSetsCount() const { return 0; }
	virtual C3DBaseFaceSet*					GetFaceSetById(size_t id) const { return nullptr; }

	virtual C3DBaseManager*					GetManager() const { return _pOwner; }


private:

	TerrainObjectID				_ID = -1;
	const CTerrainBlockDesc*	_pBlockDesc = nullptr;

	C3DBaseManager*				_pOwner = nullptr;

	D3DXMATRIX					_mTransform;
	D3DXVECTOR3					_vBBoxMin;
	D3DXVECTOR3					_vBBoxMax;
};


class CTerrainObjectManager::CTerrainObjectManagerImpl : public C3DBaseTerrainObjectManager
{
public:

	~CTerrainObjectManagerImpl();

	// �������������. �������� - ��� ����������, ��� ����� ������ �����
	void Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcwszPlanetDirectory, float in_fWorldScale, float in_fHeightScale);

	void SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection);

	// � ������ ������ ���� ������� ����������� 4 ������: 
	// �������, ������� ����� �������
	// �������, ������� ����� ��������
	// �������, ������� ����� ����������
	// �������, ������� ����� �������
	void Update(float in_fDeltaTime);

	//�������� �������� ������� ����� �� ��������������
	const CTerrainBlockDesc*	GetTerrainObjectDesc(TerrainObjectID ID) const;

	//@{ ������ ����� ��������, ������� ����� ������� (����� ����� ��������)
	size_t GetNewObjectsCount() const;
	TerrainObjectID	GetNewObjectID(size_t index) const;
	//@}

	//@{ ������ ��������, ������� ����� ��������
	//size_t GetNewVisibleObjectsCount() const;
	//TerrainObjectID GetNewVisibleObjectID(size_t index) const;
	////@}

	////@{ ������ ��������, ������� ����� ����������
	//size_t GetNewInvisibleObjectsCount() const;
	//TerrainObjectID GetNewInvisibleObjectID(size_t index);
	//@}

	//@{ ������ ��������, ������� ����� ������� (������ �� ������ ������������ �������)
	size_t GetObjectsToDeleteCount() const;
	TerrainObjectID GetObjectToDeleteID(size_t index) const;
	//@}

	//@{ ������ ������� ������� ��������
	size_t GetVisibleObjectsCount() const;
	TerrainObjectID GetVisibleObjectID(size_t index) const;
	//@}


	// �������� ��������� �� �������� �������� (���� ���������� ������ ���������� ������������� ��������� ��������, �������� �� �������� ��-���������)
	CResourceManager* GetResourceManager();

	HeightfieldConverter*	GetHeightfieldConverter();

	//@{ C3DBaseObjectManager
	// �������� ������ ��������
	
	virtual size_t GetObjectsCount() const override;
	virtual C3DBaseObject*	GetObjectByIndex(size_t id) const override;
	
	//@}

	//@{ C3DBaseTerrainObjectManager

	// �������� ������ ����� �� �������
	virtual const CTerrainBlockDesc* GetTerrainDataForObject(C3DBaseObject* pObject) const override;

	// �������� ��������� �� �������� ���� [� �������� ����� �� ������ ���� ������� ������� ��������, �� ������ ��� ���������]
	virtual const CTerrainBlockDesc* GetRootTerrainData() const override;

	//@}


	//@{ C3DBaseManager

	// ��������� �������� �������
	virtual void RequestLoadResource(C3DBaseResource*) override;

	// ��������� �������� �������
	virtual void RequestUnloadResource(C3DBaseResource*) override;

	//@} C3DBaseManager

private:

	float GetWorldRadius() const;
	float GetMinCellSize() const;

	void CreateObjects();
	void CreateObjectsRecursive(const CTerrainBlockDesc* in_pData);
	void CreateObject(const CTerrainBlockDesc* in_pData);
	void DestroyObjects();

	//@{ Main objects
	CResourceManager*		_pResourceManager = nullptr;
	CVisibilityManager*		_pVisibilityManager = nullptr;

	CTerrainBlockDesc*		_pPlanetTerrainData = nullptr;
	CTerrainDataManager*	_pTerrainDataManager = nullptr;

	HeightfieldConverter*	_pHeightfieldConverter = nullptr;
	//@}

	//@{ Vars
	TerrainObjectID			_idCurrentIDForNewObject = 0;
	float					_fWorldScale = 1.f;
	//@}

	//@{ Containers
	std::vector<CInternalTerrainObject*>				_vecObjects;
	std::map<TerrainObjectID, CInternalTerrainObject*>	_mapId2Object;

	mutable std::mutex									_containersMutex;

		//@{ following containers are guarded by mutex (_containersMutex)
	std::vector<TerrainObjectID>						_vecNewObjectIDs;
	std::vector<TerrainObjectID>						_vecObjectsToDelete;


		//@}

	//@}
};