#include <Windows.h>

#include "Application.h"
#include "TerrainDataManager.h"
#include "PlanetCameraController.h"
#include "GraphicsContext.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	/*CTerrainDataManager	DataManager;

	CTerrainBlockData* pTerrainData = nullptr;

	DataManager.LoadTerrainDataInfo(L"PlanetViewerData\\TestPlanet\\", &pTerrainData);


	DataManager.ReleaseTerrainDataInfo(pTerrainData);

	pTerrainData = nullptr;*/


	CD3DApplication* pApplication = new CD3DApplication;


	if (!pApplication->Initialize(L"TerrainViewer", 1024, 768, false))
	{
		delete pApplication;

		return -1;
	}

	CPlanetCameraController* pPlanetCameraController = new CPlanetCameraController();
	pPlanetCameraController->CreateDebugTextBlock();

	pApplication->GetGraphicsContext()->GetScene()->GetMainCamera()->SetController(pPlanetCameraController);
	
	pApplication->Run();

	pApplication->Shutdown();


	delete pPlanetCameraController;
	delete pApplication;

	return 0;
}
