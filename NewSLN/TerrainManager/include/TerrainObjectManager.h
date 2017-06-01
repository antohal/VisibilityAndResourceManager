#pragma once

#ifndef TERRAINMANAGER_EXPORTS
#define TERRAINOBJECTMANAGER_API __declspec(dllimport)
#else
#define TERRAINOBJECTMANAGER_API __declspec(dllexport)
#endif

class CTerrainBlockDesc;

struct D3DXVECTOR3;
struct _D3DMATRIX;
typedef struct _D3DMATRIX D3DMATRIX;

class CResourceManager;

typedef size_t TerrainObjectID;

class TERRAINOBJECTMANAGER_API CTerrainObjectManager
{
public:

	CTerrainObjectManager();
	~CTerrainObjectManager();

	// �������������. ���������:
	// in_pD3DDevice11, in_pDeviceContext - ������� Direct3D 11
	// in_pcwszPlanetDirectory - ��� ����������, ��� ����� ������ �����
	// in_fWorldScale - ����������� �������� ����
	// in_fHeightScale - ����������� �������� ������
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

private:

	class CTerrainObjectManagerImpl;
	CTerrainObjectManagerImpl*	_implementation = nullptr;
};