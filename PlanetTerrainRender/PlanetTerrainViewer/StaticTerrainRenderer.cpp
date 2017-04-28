#include "StaticTerrainRenderer.h"
#include "Log.h"

//
// CD3DStaticTerrainObject
//

CD3DStaticTerrainObject::CD3DStaticTerrainObject()
{

}

C3DBaseFaceSet* CD3DStaticTerrainObject::GetFaceSetById(size_t id) const 
{ 
	if (id == 0) 
		return _pFaceset; 

	return nullptr;
}

void CD3DStaticTerrainObject::SetFaceset(CD3DStaticTerrainFaceset* in_pFaceset)
{
	_pFaceset = in_pFaceset;
}

void CD3DStaticTerrainObject::GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax)
{
	if (!_pFaceset)
		return;
}

D3DXMATRIX* CD3DStaticTerrainObject::GetWorldTransform()
{
	if (!_pFaceset)
		return nullptr;

	return nullptr;
}

//
// CD3DStaticTerrainFaceset
//

CD3DStaticTerrainFaceset::CD3DStaticTerrainFaceset(CD3DStaticTerrainObject* in_pObject, CD3DStaticTerrainMaterial * in_pMaterial, const CTerrainBlockData* in_pTerrainBlockData)
	: _pTerrainObject(in_pObject), _pMaterialRef(in_pMaterial), _pTerrainBlockData(in_pTerrainBlockData)
{
}

C3DBaseMaterial * CD3DStaticTerrainFaceset::GetMaterialRef()
{
	return _pMaterialRef;
}

C3DBaseManager* CD3DStaticTerrainFaceset::GetManager() const 
{

	if (!_pMaterialRef)
		return nullptr;

	return _pMaterialRef->GetManager();
}

void CD3DStaticTerrainFaceset::Load()
{

}

void CD3DStaticTerrainFaceset::Unload()
{

}


//
// CD3DStaticTerrainMaterial
//

CD3DStaticTerrainMaterial::CD3DStaticTerrainMaterial(CD3DStaticTerrainRenderer * in_pOwner, const std::wstring& in_wsFileName) : _pOwner(in_pOwner), _wsTextureFileName(in_wsFileName)
{
}

void CD3DStaticTerrainMaterial::Render(CD3DGraphicsContext * in_pContext)
{
	// TODO: render visible facesets

	_setVisibleFacesets.clear();
}

C3DBaseManager* CD3DStaticTerrainMaterial::GetManager() const
{
	return static_cast<C3DBaseManager*>(_pOwner);
}

void CD3DStaticTerrainMaterial::AddVisibleFaceSet(C3DBaseFaceSet * in_pFaceSet , unsigned int in_uiEyeId)
{
	_setVisibleFacesets.insert(static_cast<CD3DStaticTerrainFaceset*>(in_pFaceSet));

	_pOwner->AddVisibleMaterial(this);
}

void CD3DStaticTerrainMaterial::Load()
{

}

void CD3DStaticTerrainMaterial::Unload()
{

}

//
// CD3DStaticTerrainRenderer
//

CD3DStaticTerrainRenderer::CD3DStaticTerrainRenderer()
{
	_pTerrainDataManager = new CTerrainDataManager();

}

CD3DStaticTerrainRenderer::~CD3DStaticTerrainRenderer()
{
	LogMessage("Deleting objects");

	for (CD3DStaticTerrainObject* pObject : _lstTerrainObjects)
	{
		delete pObject;
	}

	for (CD3DStaticTerrainMaterial* pMaterial : _lstMaterials)
	{
		delete pMaterial;
	}

	for (CD3DStaticTerrainFaceset* pFaceset : _lstFacesets)
	{
		delete pFaceset;
	}

	LogMessage("Releasing planet terrain info");

	if (_pPlanetTerrainData)
		_pTerrainDataManager->ReleaseTerrainDataInfo(_pPlanetTerrainData);

	delete _pTerrainDataManager;
}

void CD3DStaticTerrainRenderer::LoadPlanet(const wchar_t * in_pcwszDirectory)
{
	LogMessage("Loading planet terrain info");

	_pTerrainDataManager->LoadTerrainDataInfo(in_pcwszDirectory, &_pPlanetTerrainData);

	CreateObjects();
}

void CD3DStaticTerrainRenderer::AddVisibleMaterial(CD3DStaticTerrainMaterial * in_pMaterial)
{
	_setVisibleMaterials.insert(in_pMaterial);
}

void CD3DStaticTerrainRenderer::GetObjects(std::list<C3DBaseObject*>& out_lstObjects) const
{
	out_lstObjects.clear();

	for (CD3DStaticTerrainObject* pObject : _lstTerrainObjects)
	{
		out_lstObjects.push_back(pObject);
	}
}

void CD3DStaticTerrainRenderer::Render(CD3DGraphicsContext * in_pContext)
{

	// TODO: begin render, setup shaders

	for (CD3DStaticTerrainMaterial* pMaterial : _setVisibleMaterials)
	{
		pMaterial->Render(in_pContext);
	}

	// TODO: end render, cleanup shaders

	_setVisibleMaterials.clear();
}

void CD3DStaticTerrainRenderer::RequestLoadResource(C3DBaseResource * in_pResource)
{
	switch (in_pResource->GetType())
	{
	case C3DRESOURCE_FACESET:
		{
			if (CD3DStaticTerrainFaceset* pFaceset = dynamic_cast<CD3DStaticTerrainFaceset*>(in_pResource))
			{
				pFaceset->Load();
			}
		}
		break;

	case C3DRESOURCE_MATERIAL:
		{
			if (CD3DStaticTerrainMaterial* pMaterial = dynamic_cast<CD3DStaticTerrainMaterial*>(in_pResource))
			{
				pMaterial->Load();
			}
		}
		break;
	}

	LogMessage("CD3DStaticTerrainRenderer::RequestLoadResource: Error, resource has unknown type");
}

void CD3DStaticTerrainRenderer::RequestUnloadResource(C3DBaseResource * in_pResource)
{
	switch (in_pResource->GetType())
	{
	case C3DRESOURCE_FACESET:
		{
			if (CD3DStaticTerrainFaceset* pFaceset = dynamic_cast<CD3DStaticTerrainFaceset*>(in_pResource))
			{
				pFaceset->Load();
			}
		}
		break;

	case C3DRESOURCE_MATERIAL:
		{
			if (CD3DStaticTerrainMaterial* pMaterial = dynamic_cast<CD3DStaticTerrainMaterial*>(in_pResource))
			{
				pMaterial->Load();
			}
		}
		break;
	}

	LogMessage("CD3DStaticTerrainRenderer::RequestUnloadResource: Error, resource has unknown type");
}

void CD3DStaticTerrainRenderer::CreateObjects()
{
	if (!_pPlanetTerrainData)
	{
		LogMessage("Terrain data not loaded. Error.");
		return;
	}

	LogMessage("Terrain data loaded. Creating objects.");

	CreateObjectsRecursive(_pPlanetTerrainData);

	LogMessage("%d Objects created.", _lstTerrainObjects.size());
}

void CD3DStaticTerrainRenderer::CreateObjectsRecursive(const CTerrainBlockData* in_pData)
{
	if (in_pData->GetParentBlockData())
	{
		CreateObject(in_pData);
	}

	for (unsigned int i = 0; i < in_pData->GetChildBlockDataCount(); i++)
	{
		CreateObjectsRecursive(in_pData->GetChildBlockData(i));
	}
}

void CD3DStaticTerrainRenderer::CreateObject(const CTerrainBlockData* in_pData)
{
	CD3DStaticTerrainObject* pObject = new CD3DStaticTerrainObject;

	CD3DStaticTerrainMaterial* pMaterial = new CD3DStaticTerrainMaterial(this, in_pData->GetTextureFileName());

	CD3DStaticTerrainFaceset* pFaceset = new CD3DStaticTerrainFaceset(pObject, pMaterial, in_pData);

	pObject->SetFaceset(pFaceset);

	_lstMaterials.push_back(pMaterial);
	_lstFacesets.push_back(pFaceset);
	_lstTerrainObjects.push_back(pObject);
}
