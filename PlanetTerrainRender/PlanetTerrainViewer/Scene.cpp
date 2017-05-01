#include "Scene.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"

#include "Log.h"

CD3DScene::CD3DScene()
{

}

CD3DScene::~CD3DScene()
{
	for (auto it = _mapRenderers.begin(); it != _mapRenderers.end(); it++)
		delete it->second;
}

void CD3DScene::Update(float deltaTime)
{
	_mainCamera.Update(deltaTime);
}

void CD3DScene::Render(CD3DGraphicsContext* in_pContext)
{
	//for (CD3DSceneRenderer* pRenderer : _setRenderers)
		//pRenderer->Render(in_pContext);

	for (auto it = _mapRenderers.begin(); it != _mapRenderers.end(); it++)
	{


		it->first->Render(in_pContext);
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
	//_setRenderers.insert(in_pRenderer);

	if (_mapRenderers.find(in_pRenderer) != _mapRenderers.end())
	{
		LogMessage("Error adding renderer");
	}

	CVisibilityManager* pVisibilityManager = new CVisibilityManager(in_pRenderer, in_pRenderer->GetWorldRadius(), in_pRenderer->GetMinCellSize());

	_mapRenderers[in_pRenderer] = pVisibilityManager;
}

void CD3DScene::UnregisterRenderer(CD3DSceneRenderer * in_pRenderer)
{
	//_setRenderers.erase(in_pRenderer);

	if (_mapRenderers.find(in_pRenderer) == _mapRenderers.end())
	{
		LogMessage("Error removing renderer: no such renderer");
	}

	CVisibilityManager* pVisibilityManager = _mapRenderers[in_pRenderer];

	delete pVisibilityManager;
}
