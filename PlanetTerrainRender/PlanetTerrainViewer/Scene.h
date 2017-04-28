#pragma once

#include "Camera.h"

#include "C3DBaseObject.h"

#include <set>
#include <list>

class CD3DGraphicsContext;

class CD3DSceneRenderer
{
public:

	virtual void					GetObjects(std::list<C3DBaseObject*>& out_lstObjects) const = 0;
	virtual void					Render(CD3DGraphicsContext* in_pContext) = 0;

};

class CD3DScene
{
public:

	void							Update(float deltaTime);
	void							Render(CD3DGraphicsContext* in_pContext);

	CD3DCamera*						GetMainCamera();
	const CD3DCamera*				GetMainCamera() const;

	void							RegisterRenderer(CD3DSceneRenderer* in_pRenderer);
	void							UnregisterRenderer(CD3DSceneRenderer* in_pRenderer);

private:

	CD3DCamera						_mainCamera;
	std::set<CD3DSceneRenderer*>	_setRenderers;

};
