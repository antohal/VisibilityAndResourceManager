#pragma once


#ifndef TERRAINMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

// �������� ����� �����
class TERRAINDATAMANAGER_API CTerrainBlockDesc
{
public:

	CTerrainBlockDesc();

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
	const CTerrainBlockDesc*		GetParentBlockDesc() const;

	// �������� ���������� �������� ������
	unsigned int					GetChildBlockDescCount() const;

	// �������� ��������� �� �������� ����
	const CTerrainBlockDesc*		GetChildBlockDesc(unsigned int id) const;

protected:

	~CTerrainBlockDesc();

private:

	class CTerrainBlockDescImplementation;
	CTerrainBlockDescImplementation*	_implementation = nullptr;

	friend class CTerrainDataManager;
};

class TERRAINDATAMANAGER_API CTerrainDataManager
{
public:

	CTerrainDataManager();
	~CTerrainDataManager();

	// ��������� �������� ������ ����������� ����� [out_ppRootDataBlock] �� ��������� ���������� [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockDesc** out_ppRootDataBlock);

	// ���������� ����������� �������� ������
	void	ReleaseTerrainDataInfo(CTerrainBlockDesc* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;

	friend class CTerrainBlockDesc::CTerrainBlockDescImplementation;
};


class CTerrainManagerClient
{
public:

	virtual size_t	CreateObject(CTerrainBlockData*) = 0;
	virtual void	DeleteObject(size_t ID) = 0;

	virtual void	ObjectBecomeVisible(size_t ID) = 0;
	virtual void	ObjectBecomeInvisible(size_t ID) = 0;
};

class CTerrainManager
{
	// ��� ����� ���
};
