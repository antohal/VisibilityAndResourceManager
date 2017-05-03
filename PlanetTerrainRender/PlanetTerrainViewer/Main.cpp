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


	if (!pApplication->Initialize(L"TerrainViewer", 1024, 768, 10.f, 10000.f, false))
	{
		delete pApplication;

		return -1;
	}

	//@{ Prepare camera controller

	CPlanetCameraController* pPlanetCameraController = new CPlanetCameraController();
	pPlanetCameraController->CreateDebugTextBlock();

	pPlanetCameraController->SetCoordinates(CPlanetCameraController::Coordinates(D2R*20, D2R*20, 10000.f, 0, 0));

	pApplication->GetGraphicsContext()->GetScene()->GetMainCamera()->SetController(pPlanetCameraController);
	
	//@}


	//@{ Prepare terrain rendering pipeline

	CD3DStaticTerrainRenderer* pTerrainRenderer = new CD3DStaticTerrainRenderer;

	pTerrainRenderer->Init(pApplication->GetGraphicsContext()->GetSystem());

	pTerrainRenderer->LoadPlanet(L"PlanetViewerData//SimplePlanet");

	pApplication->GetGraphicsContext()->GetScene()->RegisterRenderer(pTerrainRenderer);

	pApplication->GetGraphicsContext()->GetScene()->SetWorldRadius(20000.f);
	pApplication->GetGraphicsContext()->GetScene()->SetMinCellSize(1.f);

	pApplication->GetGraphicsContext()->GetScene()->RegisterObjectManager(pTerrainRenderer);

	pApplication->GetGraphicsContext()->GetScene()->CreateDebugTextBlock();
	pApplication->GetGraphicsContext()->GetScene()->ShowDebugTextBlock(true);

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
