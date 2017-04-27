#pragma once

#include "TerrainDataManager.h"

class CTerrainDataManager::CTerrainDataManagerImplementation
{
public:

	CTerrainDataManagerImplementation();
	~CTerrainDataManagerImplementation();

	// ��������� �������� ������ ����������� ����� [out_ppRootDataBlock] �� ��������� ���������� [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockData** out_ppRootDataBlock);

	// ���������� ����������� �������� ������
	void	ReleaseTerrainDataInfo(CTerrainBlockData* in_pTerrainDataBlock);

};
