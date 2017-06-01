#pragma once

#include "TerrainDataManager.h"
#include "TerrainObjectManager.h"
#include "HeightfieldConverter.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"
#include "TerrainVisibilityManager.h"


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
	size_t GetNewVisibleObjectsCount() const;
	TerrainObjectID GetNewVisibleObjectID(size_t index) const;
	//@}

	//@{ ������ ��������, ������� ����� ����������
	size_t GetNewInvisibleObjectsCount() const;
	TerrainObjectID GetNewInvisibleObjectID(size_t index);
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

	CResourceManager*		_pResourceManager = nullptr;
	CVisibilityManager*		_pVisibilityManager = nullptr;

	CTerrainBlockDesc*		_pPlanetTerrainData = nullptr;
	CTerrainDataManager*	_pTerrainDataManager = nullptr;

	HeightfieldConverter*	_pHeightfieldConverter = nullptr;

};