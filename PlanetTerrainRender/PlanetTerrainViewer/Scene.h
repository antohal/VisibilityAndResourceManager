#pragma once

#include "Camera.h"

class CD3DGraphicsContext;

class CD3DScene
{
public:

	void				Update(float deltaTime);
	void				Render(CD3DGraphicsContext* in_pContext);

	CD3DCamera*			GetMainCamera();
	const CD3DCamera*	GetMainCamera() const;

private:

	CD3DCamera			_mainCamera;

};