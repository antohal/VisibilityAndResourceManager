#include <Windows.h>

#include "Application.h"
#include "TerrainDataManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	/*CTerrainDataManager	DataManager;

	CTerrainBlockData* pTerrainData = nullptr;

	DataManager.LoadTerrainDataInfo(L"PlanetViewerData\\TestPlanet\\", &pTerrainData);


	DataManager.ReleaseTerrainDataInfo(pTerrainData);

	pTerrainData = nullptr;*/


	CD3DApplication* pD3DApplication = new CD3DApplication;


	if (pD3DApplication->Initialize(L"TerrainViewer", 1024, 768, false))
	{

		pD3DApplication->Run();

	}

	pD3DApplication->Shutdown();

	delete pD3DApplication;

	return 0;
}
