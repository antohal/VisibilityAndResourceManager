#include <Windows.h>

#include "Application.h"
#include "TerrainDataManager.h"
#include "PlanetCameraController.h"
#include "GraphicsContext.h"

#include "SimpleTerrainRenderer.h"
#include "TerrainManager.h"

#include "Log.h"

const float g_fWorldScale = 0.000005f;


class CMyAppHandler : public CD3DAppHandler
{
public:

	CMyAppHandler(CTerrainManager* in_pTerrainObjectManager, CSimpleTerrainRenderer* in_pSimpleTerrainRenderer, CD3DCamera* in_pCamera, CD3DGraphicsContext* in_pContext)
		:_pTerrainManager(in_pTerrainObjectManager),  _pTerrainRenderer(in_pSimpleTerrainRenderer), _pCamera(in_pCamera), _pContext(in_pContext)
	{}
	

	virtual void OnKeyDown(unsigned int in_wKey) 
	{
		if (in_wKey == VK_F1)
		{
			bool bWireframe = _pContext->GetSystem()->IsWireframe();
			_pContext->GetSystem()->SetWireframe(!bWireframe);
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

	virtual void OnFrame(float in_fFrameTime) override
	{
		D3DXVECTOR3 vPos, vDir, vUp;

		vPos.x = (float)_pCamera->GetPos()[0];
		vPos.y = (float)_pCamera->GetPos()[1];
		vPos.z = (float)_pCamera->GetPos()[2];

		vDir.x = (float)_pCamera->GetDir()[0];
		vDir.y = (float)_pCamera->GetDir()[1];
		vDir.z = (float)_pCamera->GetDir()[2];

		vUp.x = (float)_pCamera->GetUp()[0];
		vUp.y = (float)_pCamera->GetUp()[1];
		vUp.z = (float)_pCamera->GetUp()[2];

		_pTerrainManager->SetViewProjection(&vPos, &vDir, &vUp, _pContext->GetSystem()->GetProjectionMatrix());

		_pTerrainManager->Update(in_fFrameTime);


		// добавляем новые объекты

		for (size_t iObj = 0; iObj < _pTerrainManager->GetNewObjectsCount(); iObj++)
		{
			TerrainObjectID newObjID = _pTerrainManager->GetNewObjectID(iObj);
			_pTerrainRenderer->CreateObject(newObjID);
		}


		// удаляем старые
		
		for (size_t iObj = 0; iObj < _pTerrainManager->GetObjectsToDeleteCount(); iObj++)
		{
			TerrainObjectID oldObjID = _pTerrainManager->GetObjectToDeleteID(iObj);
			_pTerrainRenderer->DeleteObject(oldObjID);
		}

		
		// добавляем на отрисовку видимые

		for (size_t iObj = 0; iObj < _pTerrainManager->GetVisibleObjectsCount(); iObj++)
		{
			TerrainObjectID visObjID = _pTerrainManager->GetVisibleObjectID(iObj);
			_pTerrainRenderer->AddObjectToRenderQueue(visObjID);
		}
	}


private:

	CTerrainManager*		_pTerrainManager = nullptr;
	CSimpleTerrainRenderer*	_pTerrainRenderer = nullptr;
	CD3DCamera*				_pCamera = nullptr;
	CD3DGraphicsContext*	_pContext = nullptr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	LogInit("TerrainViewer.log");

	LogEnable(true);

	CD3DApplication* pApplication = new CD3DApplication;


	if (!pApplication->Initialize(L"TerrainViewer", 1280, 960, g_fWorldScale * 10000.f, g_fWorldScale * 50000000.f * 100.f, false))
	{
		delete pApplication;

		return -1;
	}	

	//@{ Prepare camera controller

	CPlanetCameraController* pPlanetCameraController = new CPlanetCameraController();
	pPlanetCameraController->CreateDebugTextBlock();

	pPlanetCameraController->SetWorldScale(g_fWorldScale * 100.f);
	pPlanetCameraController->SetCoordinates(CPlanetCameraController::Coordinates(D2R*20, D2R*20, g_fWorldScale * 100.f * 10000000.f, 0, 0));
	pPlanetCameraController->SetMaxHeight(g_fWorldScale * 100.f * 20000000.0);

	pPlanetCameraController->SetScrollCoeff(0.5f);

	pApplication->GetGraphicsContext()->GetScene()->GetMainCamera()->SetController(pPlanetCameraController);

	pApplication->GetGraphicsContext()->GetScene()->CreateDebugTextBlock();
	pApplication->GetGraphicsContext()->GetScene()->ShowDebugTextBlock(true);

	//@}

	CTerrainManager* pTerrainManager = nullptr;
	CSimpleTerrainRenderer* pSimpleTerrainRenderer = nullptr;
	CMyAppHandler* pAppHandler = nullptr;

	//@{ Prepare terrain rendering pipeline

	

	ID3D11Device* pDevice = pApplication->GetGraphicsContext()->GetSystem()->GetDevice();
	ID3D11DeviceContext* pDeviceContext = pApplication->GetGraphicsContext()->GetSystem()->GetDeviceContext();

	pTerrainManager = new CTerrainManager();

	// Инициализируем террейн менеджер
	//pTerrainManager->Init(pDevice, pDeviceContext, L"PlanetViewerData\\TestPlanet", g_fWorldScale, g_fWorldScale * 100000000.f * 100.f);
	pTerrainManager->Init(pDevice, pDeviceContext, L"PlanetViewerData\\Earth_2", g_fWorldScale, g_fWorldScale * 100000000.f * 100.f, 1.517f, 2.034f);

	// Это если нужно сгенерить планету
	//pTerrainManager->InitGenerated(pDevice, pDeviceContext, L"PlanetViewerData\\RandomPlanet", 2, 2, 9, g_fWorldScale, g_fWorldScale * 100000000.f * 100.f);

	pTerrainManager->GetResourceManager()->EnableDebugTextRender(pApplication->GetGraphicsContext()->GetScene()->GetDebugTextBlock());

	// создаем простой рендерер
	pSimpleTerrainRenderer = new CSimpleTerrainRenderer();
	pSimpleTerrainRenderer->Init(pTerrainManager, g_fWorldScale);

	pApplication->GetGraphicsContext()->GetScene()->RegisterRenderer(pSimpleTerrainRenderer);


	pAppHandler = new CMyAppHandler(pTerrainManager, pSimpleTerrainRenderer, pApplication->GetGraphicsContext()->GetScene()->GetMainCamera(), pApplication->GetGraphicsContext());
	pApplication->InstallAppHandler(pAppHandler);


	//@}


	// Run the application
	pApplication->Run();


	if (pSimpleTerrainRenderer)
		delete pSimpleTerrainRenderer;

	// Shutdown application
	pApplication->Shutdown();



	// delete objects

	if (pPlanetCameraController)
		delete pPlanetCameraController;

	if (pApplication)
		delete pApplication;

	if (pAppHandler)
		delete pAppHandler;

	return 0;
}
