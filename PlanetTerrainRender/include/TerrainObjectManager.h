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

	void Init(CTerrainObjectCreator* in_pObjectCreator, const wchar_t* in_pcwszPlanetDirectory);

	CResourceManager* GetResourceManager();

	void SetViewProjection(const D3DXVECTOR3& in_vPos, const D3DXVECTOR3& in_vDir, const D3DXVECTOR3& in_vUp, const D3DMATRIX* in_pmProjection);

	void Update(float in_fDeltaTime);

	size_t GetVisibleTerrainObjectCount() const;

	size_t GetVisibleObjectID(size_t index) const;

private:

	class CTerrainObjectManagerImpl;
	CTerrainObjectManagerImpl*	_implementation = nullptr;
};