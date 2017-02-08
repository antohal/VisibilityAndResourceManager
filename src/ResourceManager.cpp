#include <Windows.h>

#include "ResourceManager.h"
#include "VisibilityManager.h"
#include "Log.h"

#include <vector>
#include <map>

#include "D2DBaseTypes.h"
#include "CDirect2DTextRenderer.h"

#define USE_DEBUG_INFO


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

	CDirect2DTextBlock*			_textBlock = nullptr;

	UINT						_paramPotentiallyVisibleResources = -1;
	UINT						_paramPotentiallyVisibleObjects = -1;
	UINT						_paramPotentiallyVisibleMeshes = -1;
	UINT						_paramPotentiallyVisibleFacesets = -1;
	UINT						_paramPotentiallyVisibleMaterials = -1;
	UINT						_paramPotentiallyVisibleTextures = -1;

	unsigned int				_potentiallyVisibleResources = 0;

#ifdef USE_DEBUG_INFO
	std::map<E3DResourceType, int>	_mapPotentiallyVisibleByType;
#endif

	void incrementVisibilityRefCountRecursive(C3DBaseResource* resource)
	{
		if (!resource)
			return;

		if (resource->_visibleRefCount == 0)
		{
			_potentiallyVisibleResources++;

#ifdef USE_DEBUG_INFO
			_mapPotentiallyVisibleByType[resource->GetType()] ++;
#endif

			if (resource->GetManager())
			{
				resource->GetManager()->RequestLoadResource(resource);
				LogMessage("CResourceManager: resource with type %d is became visible, requesting load", resource->GetType());
			}
		}

		resource->_visibleRefCount++;
		//LogMessage("CResourceManager: resource with type %d incremented refcounter %d", resource->GetType(), resource->_visibleRefCount);

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
			//LogMessage("CResourceManager: resource with type %d decremented refcounter %d", resource->GetType(), resource->_visibleRefCount);

			if (resource->_visibleRefCount == 0)
			{
				_potentiallyVisibleResources--;

#ifdef USE_DEBUG_INFO
				_mapPotentiallyVisibleByType[resource->GetType()] --;
#endif

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

	if (_private->_textBlock)
	{
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleResources, (float) _private->_potentiallyVisibleResources);

#ifdef USE_DEBUG_INFO
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleObjects, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_OBJECT]);
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleMeshes, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_MESH]);
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleFacesets, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_FACESET]);
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleMaterials, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_MATERIAL]);
		_private->_textBlock->SetParameterValue(_private->_paramPotentiallyVisibleTextures, (float)_private->_mapPotentiallyVisibleByType[C3DRESOURCE_TEXTURE]);

#endif

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

// Включить рендеринг отладочной информации в текстовый блок
void CResourceManager::EnableDebugTextRender(CDirect2DTextBlock* textBlock)
{
	if (_private->_textBlock)
	{
		LogMessage("CResourceManager::EnableDebugTextRender: text block already assigned, disable first!");
		return;
	}

	_private->_textBlock = textBlock;

	_private->_paramPotentiallyVisibleResources =  textBlock->AddParameter(L"Количество потенциально видимых ресурсов");

	_private->_paramPotentiallyVisibleObjects = textBlock->AddParameter(L"Объектов");
	_private->_paramPotentiallyVisibleMeshes = textBlock->AddParameter(L"Мешей");
	_private->_paramPotentiallyVisibleFacesets = textBlock->AddParameter(L"Фейссетов");
	_private->_paramPotentiallyVisibleMaterials = textBlock->AddParameter(L"Материалов");
	_private->_paramPotentiallyVisibleTextures = textBlock->AddParameter(L"Текстур");
}

// Выключить рендеринг отладочной информации в текстовый блок
void CResourceManager::DisableDebugTextRender()
{
	_private->_textBlock = nullptr;
}
