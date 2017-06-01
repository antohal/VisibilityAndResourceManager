#include <Windows.h>

#include "Application.h"
#include "TerrainDataManager.h"
#include "PlanetCameraController.h"
#include "GraphicsContext.h"
//#include "StaticTerrainRenderer.h"

#include "SimpleTerrainRenderer.h"
#include "TerrainObjectManager.h"

#include "Log.h"

const float g_fWorldScale = 0.001f;


class CMyAppHandler : public CD3DAppHandler
{
public:

	CMyAppHandler(CTerrainObjectManager* in_pTerrainObjectManager, CSimpleTerrainRenderer* in_pSimpleTerrainRenderer)
		:_pTerrainManager(in_pTerrainObjectManager),  _pTerrainRenderer(in_pSimpleTerrainRenderer)
	{}
	

	virtual void OnKeyDown(unsigned int in_wKey) 
	{
		if (in_wKey == VK_F1)
		{
			bool bWireframe = GetApplicationHandle()->GetGraphicsContext()->GetSystem()->IsWireframe();
			GetApplicationHandle()->GetGraphicsContext()->GetSystem()->SetWireframe(!bWireframe);
		}

		if (in_wKey == VK_F2)
		{
			_pTerrainRenderer->SetRenderingMode(CSimpleTerrainRenderer::PSRenderingMode::STANDARD);
		}

		if (in_wKey == VK_F3)
		{
			_pTerrainRenderer->SetRenderingMode(CSimpleTerrainRenderer::PSRenderingMode::SHOW_NORMALS);
		}
	}


private:

	CTerrainObjectManager*	_pTerrainManager = nullptr;
	CSimpleTerrainRenderer*	_pTerrainRenderer = nullptr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	LogInit("TerrainViewer.log");

	LogEnable(true);

	CD3DApplication* pApplication = new CD3DApplication;


	if (!pApplication->Initialize(L"TerrainViewer", 1920, 1080, g_fWorldScale * 100.f, g_fWorldScale * 15000000.f, true))
	{
		delete pApplication;

		return -1;
	}	

	//@{ Prepare camera controller

	CPlanetCameraController* pPlanetCameraController = new CPlanetCameraController();
	pPlanetCameraController->CreateDebugTextBlock();

	pPlanetCameraController->SetWorldScale(g_fWorldScale);
	pPlanetCameraController->SetCoordinates(CPlanetCameraController::Coordinates(D2R*20, D2R*20, g_fWorldScale * 10000000.f, 0, 0));
	pPlanetCameraController->SetMaxHeight(20000000.0);

	pApplication->GetGraphicsContext()->GetScene()->GetMainCamera()->SetController(pPlanetCameraController);
	
	//@}


	//@{ Prepare terrain rendering pipeline

	/*CD3DStaticTerrainRenderer* pTerrainRenderer = new CD3DStaticTerrainRenderer;

	pTerrainRenderer->Init(pApplication->GetGraphicsContext()->GetSystem(), g_fWorldScale);

	pTerrainRenderer->LoadPlanet(L"PlanetViewerData//TestPlanet");

	pApplication->GetGraphicsContext()->GetScene()->RegisterRenderer(pTerrainRenderer);

	pApplication->GetGraphicsContext()->GetScene()->SetWorldRadius(g_fWorldScale * 100000000.f);
	pApplication->GetGraphicsContext()->GetScene()->SetMinCellSize(g_fWorldScale * 100.f);

	pApplication->GetGraphicsContext()->GetScene()->RegisterObjectManager(pTerrainRenderer);*/

	pApplication->GetGraphicsContext()->GetScene()->CreateDebugTextBlock();
	pApplication->GetGraphicsContext()->GetScene()->ShowDebugTextBlock(true);


	//CTerrainVisibilityManager* pTerrainVisibilityManager = new CTerrainVisibilityManager;

	//pTerrainVisibilityManager->Init(pTerrainRenderer, g_fWorldScale);

	//pApplication->GetGraphicsContext()->GetScene()->InstallVisibilityPlugin(pTerrainRenderer, pTerrainVisibilityManager);


	ID3D11Device* pDevice = pApplication->GetGraphicsContext()->GetSystem()->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pApplication->GetGraphicsContext()->GetSystem()->GetDeviceContext();
	
	CTerrainObjectManager* pTerrainObjectManager = new CTerrainObjectManager();
	pTerrainObjectManager->Init(pDevice, pDeviceContext, L"PlanetViewerData//TestPlanet", g_fWorldScale, 10000.0);


	CSimpleTerrainRenderer* pTerrainRenderer = new CSimpleTerrainRenderer();
	pTerrainRenderer->Init();


	CMyAppHandler* pAppHandler = new CMyAppHandler(pTerrainObjectManager, pTerrainRenderer);

	pApplication->InstallAppHandler(pAppHandler);


	//@}


	// Run the application
	pApplication->Run();

	// Shutdown application
	pApplication->Shutdown();



	// delete objects

	//delete pTerrainVisibilityManager;
	//delete pTerrainRenderer;
	delete pPlanetCameraController;
	delete pApplication;
	delete pAppHandler;

	return 0;
}
