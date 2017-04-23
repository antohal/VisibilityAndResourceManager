#pragma once

#include <atlbase.h>

#include "TerrainDataManager.h"

class CTerrainDataManager::CTerrainDataManagerImplementation
{
public:

	CTerrainDataManagerImplementation();
	~CTerrainDataManagerImplementation();

	void	Init(ID3D11Device* in_pD3D11Device, ID3D11DeviceContext* in_pD3D11DeviceContext);

	// «агрузить описание данных поверхности «емли [out_ppRootDataBlock] из указанной директории [in_pcwszDirectoryName]
	bool	LoadTerrainDataInfo(const wchar_t* in_pcwszDirectoryName, CTerrainBlockData** out_ppRootDataBlock);

	// ќсвободить загруженное описание данных
	void	ReleaseTerrainDataInfo(CTerrainBlockData* in_pTerrainDataBlock);

private:

	CComPtr<ID3D11Device>			_ptrD3DDevice;
	CComPtr<ID3D11DeviceContext>	_ptrDeviceContext;

};
