#include "Scene.h"

#include "VisibilityManager.h"
#include "ResourceManager.h"

void CD3DScene::Update(float deltaTime)
{
	_mainCamera.Update(deltaTime);
}

void CD3DScene::Render(CD3DGraphicsContext* in_pContext)
{
	for (CD3DSceneRenderer* pRenderer : _setRenderers)
		pRenderer->Render(in_pContext);
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
	_setRenderers.erase(in_pRenderer);
}
