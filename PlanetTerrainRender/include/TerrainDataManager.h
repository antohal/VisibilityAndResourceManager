#pragma once

#include <d3d11.h>

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

	// ��������� �������
	void							LoadResources();

	// ��������� �������
	void							UnloadResources();

	// �������� �������� � ������ �����
	ID3D11ShaderResourceView*		GetHeighmap() const;

	// �������� �������� �����
	ID3D11ShaderResourceView*		GetTexture() const;

protected:

	~CTerrainBlockData();

private:

	class CTerrainBlockDataImplementation;
	CTerrainBlockDataImplementation*	_implementation = nullptr;

	friend class CTerrainDataManager;
};

class TERRAINDATAMANAGER_API CTerrainDataManager
{
public:

	CTerrainDataManager();
	~CTerrainDataManager();

	// ����������������
	void	Init(ID3D11Device* in_pD3D11Device, ID3D11DeviceContext* in_pD3D11DeviceContext);

	// ��������� �������� ������ ����������� ����� [out_ppRootDataBlock] �� ��������� ���������� [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockData** out_ppRootDataBlock);

	// ���������� ����������� �������� ������
	void	ReleaseTerrainDataInfo(CTerrainBlockData* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;

	friend class CTerrainBlockData::CTerrainBlockDataImplementation;
};
