#include <Windows.h>

#include "ResourceManager.h"
#include "VisibilityManager.h"
#include "Log.h"

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
			{
				resource->GetManager()->RequestLoadResource(resource);
				LogMessage("CResourceManager: resource with type %d is became visible, requesting load", resource->GetType());
			}

		resource->_visibleRefCount++;
		LogMessage("CResourceManager: resource with type %d incremented refcounter %d", resource->GetType(), resource->_visibleRefCount);

		//vector<C3DBaseResource*> vecChildResources;
		//resource->GetChildResources(vecChildResources);

		//for (C3DBaseResource* childResource : vecChildResources)
		for (size_t iChildResource = 0; iChildResource < resource->GetChildResourceCount(); iChildResource++)
		{
			C3DBaseResource* childResource = resource->GetChildResourceById(iChildResource);
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
			LogMessage("CResourceManager: resource with type %d decremented refcounter %d", resource->GetType(), resource->_visibleRefCount);

			if (resource->_visibleRefCount == 0)
			{
				if (resource->GetManager())
				{
					resource->GetManager()->RequestUnloadResource(resource);
					LogMessage("CResourceManager: resource with type %d no longer referenced, unloading", resource->GetType());
				}
			}
		} 

		/*vector<C3DBaseResource*> vecChildResources;
		resource->GetChildResources(vecChildResources);

		for (C3DBaseResource* childResource : vecChildResources)*/
		for (size_t iChildResource = 0; iChildResource < resource->GetChildResourceCount(); iChildResource++)
		{
			C3DBaseResource* childResource = resource->GetChildResourceById(iChildResource);
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

	//LogMessage("CResourceManager: object %d is potentially visible", reinterpret_cast<UINT_PTR>(this));
}


CResourceManager::CResourceManager()
{
	LogMessage("CResourceManager: created.");

	_private = new SResourceManagerPrivate(this);

	g_ResourceManager = this;
}

CResourceManager::~CResourceManager()
{
	LogMessage("CResourceManager: destructed.");

	delete _private;
}

void CResourceManager::Init(C3DBaseObjectManager* objectManager)
{
	_private->_objectManager = objectManager;

	LogMessage("CResourceManager: inited OK.");
}

void CResourceManager::SetInvisibleUnloadTime(float time)
{
	LogMessage("CResourceManager: InvisibleUnloadTime set to %f.", time);

	_private->_invisibleUnloadTime = time;
}

void CResourceManager::SetSpeedPotentialVisibilityMultiplier(float speedMultiplier)
{
	LogMessage("CResourceManager: SpeedPotentialVisibilityMultiplier set to %f.", speedMultiplier);

	_private->_boundBoxExtensionSpeedMultiplier = speedMultiplier;
}

void CResourceManager::SetRotationRatePotentialVisibilityMultiplier(float rateMultiplier)
{
	LogMessage("CResourceManager: RotationRatePotentialVisibilityMultiplier set to %f.", rateMultiplier);
}

void CResourceManager::AddVisibilityManager(CVisibilityManager* visibilityManager)
{
	_private->_visibilityManagers.push_back(visibilityManager);

	LogMessage("CResourceManager: added visibility manager %d", reinterpret_cast<UINT_PTR>(visibilityManager));
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

void CResourceManager::EnableLeg(bool enable/* = true*/)
{
	LogEnable(enable);

	for (CVisibilityManager* visman : _private->_visibilityManagers)
	{
		visman->EnableLog(enable);
	}
}

// Выключить лог в файл
void CResourceManager::DisableLog()
{
	LogEnable(false);

	for (CVisibilityManager* visman : _private->_visibilityManagers)
	{
		visman->DisableLog();
	}
}
