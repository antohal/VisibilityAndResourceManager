#include <Windows.h>

#include "ResourceManager.h"
#include "VisibilityManager.h"

#include <vector>
#include <map>

using namespace std;

namespace {
	CResourceManager* g_ResourceManager = nullptr;
}

CResourceManager* GetResourceManager()
{
	return g_ResourceManager;
}

struct CResourceManager::SResourceManagerPrivate
{
	SResourceManagerPrivate(CResourceManager* owner) : _owner(owner)
	{}

	CResourceManager*			_owner = nullptr;
	C3DBaseObjectManager*		_objectManager = nullptr;

	float						_invisibleUnloadTime = 10.f;
	float						_boundBoxExtensionSpeedMultiplier = 10.f;

	vector<CVisibilityManager*>		_visibilityManagers;
	map<C3DBaseObject*, float>		_objectVisibilityTimers;

	void incrementVisibilityRefCountRecursive(C3DBaseResource* resource)
	{
		if (!resource)
			return;

		if (resource->_visibleRefCount == 0)
			if (resource->GetManager())
				resource->GetManager()->RequestLoadResource(resource);

		resource->_visibleRefCount++;

		vector<C3DBaseResource*> vecChildResources;
		resource->GetChildResources(vecChildResources);

		for (C3DBaseResource* childResource : vecChildResources)
		{
			incrementVisibilityRefCountRecursive(childResource);
		}
	}

	void decrementVisibilityRefCountRecursive(C3DBaseResource* resource)
	{
		if (!resource)
			return;

		// TODO: insert assert here
		if (resource->_visibleRefCount > 0)
		{
			resource->_visibleRefCount--;

			if (resource->_visibleRefCount == 0)
			{
				if (resource->GetManager())
					resource->GetManager()->RequestUnloadResource(resource);
			}
		} 

		vector<C3DBaseResource*> vecChildResources;
		resource->GetChildResources(vecChildResources);

		for (C3DBaseResource* childResource : vecChildResources)
		{
			decrementVisibilityRefCountRecursive(childResource);
		}
	}
};

void C3DBaseObject::SetPotentiallyVisible()
{
	if (!GetResourceManager())
		return;

	map<C3DBaseObject*, float>& objectVisibilityTimers = GetResourceManager()->_private->_objectVisibilityTimers;

	// Если объект стал виден впервые, или после долгого перерыва, то обновить счетчики видимости
	if (objectVisibilityTimers.find(this) == objectVisibilityTimers.end())
	{
		GetResourceManager()->_private->incrementVisibilityRefCountRecursive(this);
	}

	GetResourceManager()->_private->_objectVisibilityTimers[this] = GetResourceManager()->_private->_invisibleUnloadTime;
}


CResourceManager::CResourceManager()
{
	_private = new SResourceManagerPrivate(this);

	g_ResourceManager = this;
}

CResourceManager::~CResourceManager()
{
	delete _private;
}

void CResourceManager::Init(C3DBaseObjectManager* objectManager)
{
	_private->_objectManager = objectManager;
}

void CResourceManager::SetInvisibleUnloadTime(float time)
{
	_private->_invisibleUnloadTime = time;
}

void CResourceManager::SetSpeedPotentialVisibilityMultiplier(float speedMultiplier)
{
	_private->_boundBoxExtensionSpeedMultiplier = speedMultiplier;
}

void CResourceManager::AddVisibilityManager(CVisibilityManager* visibilityManager)
{
	_private->_visibilityManagers.push_back(visibilityManager);
}

void CResourceManager::Update(float deltaTime)
{
	if (deltaTime > 1)
		deltaTime = 1;

	map<C3DBaseObject*, float>& objectVisibilityTimers = _private->_objectVisibilityTimers;

	for (auto it = objectVisibilityTimers.begin(); it != objectVisibilityTimers.end();)
	{
		float& timer = it->second;

		timer -= deltaTime;

		if (timer <= 0)
		{
			_private->decrementVisibilityRefCountRecursive(it->first);
			it = objectVisibilityTimers.erase(it);
		}
		else
			it++;
	}
}

float CResourceManager::GetTexturePriority(C3DBaseTexture* texture)
{
	float texturePriority = -2;

	for (CVisibilityManager* visibilityManager : _private->_visibilityManagers)
	{
		float localTexturePriority = visibilityManager->GetTexturePriority(texture);

		if (localTexturePriority > 0)
		{
			if (texturePriority < 0 || localTexturePriority < texturePriority)
				texturePriority = localTexturePriority;
		}
	}

	return texturePriority;
}

void CResourceManager::DumpToFile(const std::wstring& fileName)
{

}
