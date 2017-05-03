#include "Scene.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"
#include "GraphicsContext.h"
#include "Application.h"

#include "Log.h"

CD3DScene::CD3DScene()
{
	_pResourceManager = new CResourceManager();
}

CD3DScene::~CD3DScene()
{
	for (auto it = _mapManagers.begin(); it != _mapManagers.end(); it++)
		delete it->second;

	delete _pResourceManager;
}

void CD3DScene::Update(CD3DGraphicsContext* in_pContext, float deltaTime)
{
	_mainCamera.Update(deltaTime);

	Vector3 vPos, vDir, vUp;

	vPos.x = (float)_mainCamera.GetPos()[0];
	vPos.y = (float)_mainCamera.GetPos()[1];
	vPos.z = (float)_mainCamera.GetPos()[2];

	vDir.x = (float)_mainCamera.GetDir()[0];
	vDir.y = (float)_mainCamera.GetDir()[1];
	vDir.z = (float)_mainCamera.GetDir()[2];

	vUp.x = (float)_mainCamera.GetUp()[0];
	vUp.y = (float)_mainCamera.GetUp()[1];
	vUp.z = (float)_mainCamera.GetUp()[2];

	_pResourceManager->SetViewProjection(vPos, vDir, vUp, in_pContext->GetSystem()->GetProjectionMatrix());

	_pResourceManager->Update(deltaTime);

	for (auto it = _mapManagers.begin(); it != _mapManagers.end(); it++)
	{
		CVisibilityManager* pVisMan = it->second;

		pVisMan->SetViewProjection(vPos, vDir, vUp, in_pContext->GetSystem()->GetProjectionMatrix());

		pVisMan->UpdateVisibleObjectsSet();
	}
}

void CD3DScene::Render(CD3DGraphicsContext* in_pContext)
{
	for (auto it = _setRenderers.begin(); it != _setRenderers.end(); it++)
	{
		(*it)->Render(in_pContext);
	}
}

CD3DCamera*  CD3DScene::GetMainCamera()
{
	return &_mainCamera;
}

const CD3DCamera*	 CD3DScene::GetMainCamera() const
{
	return &_mainCamera;
}

void CD3DScene::RegisterRenderer(CD3DSceneRenderer * in_pRenderer)
{
	_setRenderers.insert(in_pRenderer);
}

void CD3DScene::UnregisterRenderer(CD3DSceneRenderer * in_pRenderer)
{
	if (_setRenderers.find(in_pRenderer) == _setRenderers.end())
	{
		LogMessage("Error removing renderer: no such renderer");
	}

	_setRenderers.erase(in_pRenderer);
}

void CD3DScene::RegisterObjectManager(C3DBaseObjectManager* in_pManager)
{
	if (_mapManagers.find(in_pManager) != _mapManagers.end())
	{
		LogMessage("Error adding renderer");
	}

	CVisibilityManager* pVisibilityManager = new CVisibilityManager(in_pManager, GetWorldRadius(), GetMinCellSize());

	_pResourceManager->AddVisibilityManager(pVisibilityManager);

	_mapManagers[in_pManager] = pVisibilityManager;
}

void CD3DScene::UnregisterObjectManager(C3DBaseObjectManager* in_pManager)
{
	
	CVisibilityManager* pVisibilityManager = _mapManagers[in_pManager];

	_pResourceManager->RemoveVisibilityManager(pVisibilityManager);

	delete pVisibilityManager;
}

void CD3DScene::CreateDebugTextBlock()
{
	_pTextBlock = GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetTextRenderer()->CreateTextBlock();

	_pTextBlock->Init(D2D1::ColorF(D2D1::ColorF::LimeGreen), D2D1::RectF(750, 250, 1000, 700), D2D1::ColorF(0.1f, 0.2f, 0.6f, 0.2f), D2D1::ColorF(D2D1::ColorF::Red), 4, 4,
		"Consolas", DWRITE_FONT_WEIGHT_NORMAL, 12.f);

	_pResourceManager->EnableDebugTextRender(_pTextBlock);
}

void CD3DScene::ShowDebugTextBlock(bool in_bShow)
{
	if (!_pTextBlock)
	{
		LogMessage("CD3DScene::ShowDebugTextBlock: Error, text block is not created");
		return;
	}

	_pTextBlock->SetVisible(in_bShow);
}

float CD3DScene::GetWorldRadius() const
{
	return _fWorldRadius;
}

void CD3DScene::SetWorldRadius(float in_fRadius)
{
	_fWorldRadius = in_fRadius;
}

void CD3DScene::SetMinCellSize(float in_fMinCellSize)
{
	_fMinCellSize = in_fMinCellSize;
}

float CD3DScene::GetMinCellSize() const
{
	return _fMinCellSize;
}