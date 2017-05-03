#pragma once

#include "Camera.h"

#include "C3DBaseObject.h"
#include "VisibilityManager.h"
#include "ResourceManager.h"

#include <set>
#include <list>
#include <map>

class CD3DGraphicsContext;

class CD3DSceneRenderer
{
public:

	virtual void					Render(CD3DGraphicsContext* in_pContext) = 0;
};

class CD3DScene
{
public:

	CD3DScene();
	~CD3DScene();

	void							Update(CD3DGraphicsContext* in_pContext, float deltaTime);
	void							Render(CD3DGraphicsContext* in_pContext);

	CD3DCamera*						GetMainCamera();
	const CD3DCamera*				GetMainCamera() const;

	void							RegisterObjectManager(C3DBaseObjectManager* in_pManager);
	void							UnregisterObjectManager(C3DBaseObjectManager* in_pManager);

	void							RegisterRenderer(CD3DSceneRenderer* in_pRenderer);
	void							UnregisterRenderer(CD3DSceneRenderer* in_pRenderer);

	void							CreateDebugTextBlock();
	void							ShowDebugTextBlock(bool in_bShow);

	float							GetWorldRadius() const;
	void							SetWorldRadius(float in_fRadius);

	void							SetMinCellSize(float in_fMinCellSize);
	float							GetMinCellSize() const;

private:

	CD3DCamera						_mainCamera;

	CResourceManager*				_pResourceManager = nullptr;
	CDirect2DTextBlock*				_pTextBlock = nullptr;

	float							_fWorldRadius = 1000000;
	float							_fMinCellSize = 100;

	std::map<C3DBaseObjectManager*, CVisibilityManager*>	_mapManagers;
	std::set<CD3DSceneRenderer*>	_setRenderers;
};
