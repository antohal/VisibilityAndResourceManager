#pragma once

#include <d3d11.h>

#ifndef TERRAINDATAMANAGER_EXPORTS
#define TERRAINDATAMANAGER_API __declspec(dllimport)
#else
#define TERRAINDATAMANAGER_API __declspec(dllexport)
#endif

class CTerrainDataBlockInfo
{
public:

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
	const CTerrainDataBlockInfo*	GetParentBlockInfo() const;

	// �������� ���������� �������� ������
	unsigned int					GetChildBlockInfoCount() const;

	// �������� ��������� �� �������� ����
	const CTerrainDataBlockInfo*	GetChildBlockInfo(unsigned int id) const;

private:

	class CTerrainDataBlockInfoImplementation;
	CTerrainDataBlockInfoImplementation*	_implementation = nullptr;
};

class TERRAINDATAMANAGER_API CTerrainDataManager
{
public:

	CTerrainDataManager();
	~CTerrainDataManager();

	// �������������
	void	Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext);

	// ��������� �������� ������ ����������� ����� [out_ppRootDataBlock] �� ��������� ���������� [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainDataBlockInfo** out_ppRootDataBlock);

	// ���������� ����������� �������� ������
	void	ReleaseTerrainDataInfo(CTerrainDataBlockInfo* in_pTerrainDataBlock);


private:

	class CTerrainDataManagerImplementation;
	CTerrainDataManagerImplementation*		_implementation = nullptr;
};
