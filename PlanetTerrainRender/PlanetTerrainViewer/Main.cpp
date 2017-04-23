#include <Windows.h>

#include "TerrainDataManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	CTerrainDataManager	DataManager;

	CTerrainBlockData* pTerrainData = nullptr;

	DataManager.LoadTerrainDataInfo(L"PlanetViewerData\\TestPlanet\\", &pTerrainData);


	DataManager.ReleaseTerrainDataInfo(pTerrainData);

	pTerrainData = nullptr;

	return 0;
}
