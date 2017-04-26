#include "Scene.h"

void CD3DScene::Update(float deltaTime)
{
	_mainCamera.Update(deltaTime);
}

void CD3DScene::Render(CD3DGraphicsContext* in_pContext)
{

}

CD3DCamera*  CD3DScene::GetMainCamera()
{
	return &_mainCamera;
}

const CD3DCamera*	 CD3DScene::GetMainCamera() const
{
	return &_mainCamera;
}
