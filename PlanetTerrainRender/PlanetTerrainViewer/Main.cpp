#include <Windows.h>

#include "Application.h"
#include "TerrainDataManager.h"
#include "PlanetCameraController.h"
#include "GraphicsContext.h"
#include "StaticTerrainRenderer.h"

#include "Log.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	LogInit("TerrainViewer.log");

	LogEnable(true);

	CD3DApplication* pApplication = new CD3DApplication;


	if (!pApplication->Initialize(L"TerrainViewer", 1024, 768, false))
	{
		delete pApplication;

		return -1;
	}

	//@{ Prepare camera controller

	CPlanetCameraController* pPlanetCameraController = new CPlanetCameraController();
	pPlanetCameraController->CreateDebugTextBlock();

	pApplication->GetGraphicsContext()->GetScene()->GetMainCamera()->SetController(pPlanetCameraController);
	
	//@}


	//@{ Prepare terrain rendering pipeline

	CD3DStaticTerrainRenderer* pTerrainRenderer = new CD3DStaticTerrainRenderer;

	pTerrainRenderer->Init(pApplication->GetGraphicsContext()->GetSystem());

	pTerrainRenderer->LoadPlanet(L"PlanetViewerData//TestPlanet");

	pApplication->GetGraphicsContext()->GetScene()->RegisterRenderer(pTerrainRenderer);

	//@}


	// Run the application
	pApplication->Run();

	// Shutdown application
	pApplication->Shutdown();



	// delete objects

	delete pTerrainRenderer;
	delete pPlanetCameraController;
	delete pApplication;

	return 0;
}
