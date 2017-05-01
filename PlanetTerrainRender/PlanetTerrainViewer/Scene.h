#pragma once

#include "Camera.h"

#include "C3DBaseObject.h"
#include "VisibilityManager.h"

#include <set>
#include <list>
#include <map>

class CD3DGraphicsContext;

class CD3DSceneRenderer : public C3DBaseObjectManager
{
public:

	virtual void					Render(CD3DGraphicsContext* in_pContext) = 0;
	virtual float					GetWorldRadius() const = 0;
	virtual float					GetMinCellSize() const = 0;
};

class CD3DScene
{
public:

	CD3DScene();
	~CD3DScene();

	void							Update(float deltaTime);
	void							Render(CD3DGraphicsContext* in_pContext);

	CD3DCamera*						GetMainCamera();
	const CD3DCamera*				GetMainCamera() const;

	void							RegisterRenderer(CD3DSceneRenderer* in_pRenderer);
	void							UnregisterRenderer(CD3DSceneRenderer* in_pRenderer);

private:

	CD3DCamera						_mainCamera;
	std::map<CD3DSceneRenderer*, CVisibilityManager*>	_mapRenderers;

};
