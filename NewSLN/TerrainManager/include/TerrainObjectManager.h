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

class CTerrainObjectCreator
{
public:

	// ������� ������ ����� �� �������� �����. ������� �������������
	virtual size_t	CreateTerrainObject(const CTerrainBlockDesc* in_pTerrainBlockDesc) = 0;

	// ������� ������ ����� �� ��������������
	virtual void	DeleteTerrainObject(size_t ID) = 0;

	// ��������� ������ ������� ����� (����� �� ���������� ������������ �������)
	virtual void	LoadTerrainObjectData(size_t ID) = 0;

	// ��������� ������ ������� ����� (����� �� ���������� ���������)
	virtual void	UnloadTerrainObjectData(size_t ID) = 0;
};

class TERRAINOBJECTMANAGER_API CTerrainObjectManager
{
public:

	CTerrainObjectManager();
	~CTerrainObjectManager();

	// �������������. �������� - ��� ����������, ��� ����� ������ �����
	void Init(const wchar_t* in_pcwszPlanetDirectory);

	void SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection);

	// � ������ ������ ���� ������� ����������� 4 ������: 
	// �������, ������� ����� �������
	// �������, ������� ����� ��������
	// �������, ������� ����� ����������
	// �������, ������� ����� �������
	void Update(float in_fDeltaTime);

	// �������� ���������� ����� ��������
	size_t GetNewObjectsCount() const;

	// �������� ������������� ������ ������� ����� �� �������
	TerrainObjectID	GetNewObjectID(size_t index) const;

	//�������� �������� ������� ����� �� ��������������
	const CTerrainBlockDesc*	GetTerrainObjectDesc(TerrainObjectID ID) const;

	// �������� ���������� ����� ������� �������� ��������
	size_t GetNewVisibleObjectsCount() const;

	// �������� ������������� ������ �������� ������� �������� �� �������
	TerrainObjectID GetNewVisibleObjectID(size_t index) const;


	size_t GetNewInvisibleObjectsCount() const;

	TerrainObjectID GetNewInvisibleObjectID(size_t index);

	size_t GetObjectsToDeleteCount() const;

	TerrainObjectID GetObjectToDeleteID(size_t index) const;

	// �������� ��������� �� �������� �������� (���� ���������� ������ ���������� ������������� ��������� ��������, �������� �� �������� ��-���������)
	CResourceManager* GetResourceManager();

private:

	class CTerrainObjectManagerImpl;
	CTerrainObjectManagerImpl*	_implementation = nullptr;
};