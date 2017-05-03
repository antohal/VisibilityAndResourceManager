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

	//@{ �������� ����������� �������� �� ������� � ������
	float							GetMinimumLattitude() const;
	float							GetMaximumLattitude() const;

	float							GetMinimumLongitude() const;
	float							GetMaximumLongitude() const;
	//@}

	// �������� ��� ����� ��������
	const wchar_t*					GetTextureFileName() const;

	// �������� ��� ����� ����� �����
	const wchar_t*					GetHeightmapFileName() const;

	// �������� ��������� �� ������������ ���� (���������� null, ���� �������� ��������)
	const CTerrainBlockData*		GetParentBlockData() const;

	// �������� ���������� �������� ������
	unsigned int					GetChildBlockDataCount() const;

	// �������� ��������� �� �������� ����
	const CTerrainBlockData*		GetChildBlockData(unsigned int id) const;

protected:

	~CTerrainBlockData();

private:

	class CTerrainBlockDataImplementation;
	CTerrainBlockDataImplementation*	_implementation = nullptr;

	friend class CTerrainDataManager;
};

// ������� �������� �������� ����������� �����
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

	// ��������� �������� ������ ����������� ����� [out_ppRootDataBlock] �� ��������� ���������� [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockData** out_ppRootDataBlock);

	// ���������� ����������� �������� ������
	void	ReleaseTerrainDataInfo(CTerrainBlockData* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;

	friend class CTerrainBlockData::CTerrainBlockDataImplementation;
};
