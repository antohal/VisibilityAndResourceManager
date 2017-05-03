#include <Windows.h>

#include "Application.h"
#include "TerrainDataManager.h"
#include "PlanetCameraController.h"
#include "GraphicsContext.h"
#include "StaticTerrainRenderer.h"

#include "Log.h"

const float g_fWorldScale = 0.001f;

class CAppHandler : public CD3DAppHandler
{
public:
	

	virtual void OnKeyDown(unsigned int in_wKey) 
	{
		if (in_wKey == VK_F1)
		{
			bool bWireframe = GetApplicationHandle()->GetGraphicsContext()->GetSystem()->IsWireframe();
			GetApplicationHandle()->GetGraphicsContext()->GetSystem()->SetWireframe(!bWireframe);
		}
	}

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	LogInit("TerrainViewer.log");

	LogEnable(true);

	CD3DApplication* pApplication = new CD3DApplication;


	if (!pApplication->Initialize(L"TerrainViewer", 1024, 768, g_fWorldScale * 100.f, g_fWorldScale * 15000000.f, false))
	{
		delete pApplication;

		return -1;
	}

	CAppHandler* pAppHandler = new CAppHandler;

	pApplication->InstallAppHandler(pAppHandler);
	

	//@{ Prepare camera controller

	CPlanetCameraController* pPlanetCameraController = new CPlanetCameraController();
	pPlanetCameraController->CreateDebugTextBlock();

	pPlanetCameraController->SetWorldScale(g_fWorldScale);
	pPlanetCameraController->SetCoordinates(CPlanetCameraController::Coordinates(D2R*20, D2R*20, g_fWorldScale * 10000000.f, 0, 0));
	pPlanetCameraController->SetMaxHeight(20000000.0);

	pApplication->GetGraphicsContext()->GetScene()->GetMainCamera()->SetController(pPlanetCameraController);
	
	//@}


	//@{ Prepare terrain rendering pipeline

	CD3DStaticTerrainRenderer* pTerrainRenderer = new CD3DStaticTerrainRenderer;

	pTerrainRenderer->Init(pApplication->GetGraphicsContext()->GetSystem(), g_fWorldScale);

	pTerrainRenderer->LoadPlanet(L"PlanetViewerData//TestPlanet");

	pApplication->GetGraphicsContext()->GetScene()->RegisterRenderer(pTerrainRenderer);

	pApplication->GetGraphicsContext()->GetScene()->SetWorldRadius(g_fWorldScale * 100000000.f);
	pApplication->GetGraphicsContext()->GetScene()->SetMinCellSize(g_fWorldScale * 100.f);

	pApplication->GetGraphicsContext()->GetScene()->RegisterObjectManager(pTerrainRenderer);

	pApplication->GetGraphicsContext()->GetScene()->CreateDebugTextBlock();
	pApplication->GetGraphicsContext()->GetScene()->ShowDebugTextBlock(true);


	CTerrainVisibilityManager* pTerrainVisibilityManager = new CTerrainVisibilityManager;

	pTerrainVisibilityManager->Init(pTerrainRenderer, g_fWorldScale);

	pApplication->GetGraphicsContext()->GetScene()->InstallVisibilityPlugin(pTerrainRenderer, pTerrainVisibilityManager);

	//@}


	// Run the application
	pApplication->Run();

	// Shutdown application
	pApplication->Shutdown();



	// delete objects

	delete pTerrainVisibilityManager;
	delete pTerrainRenderer;
	delete pPlanetCameraController;
	delete pApplication;
	delete pAppHandler;

	return 0;
}
