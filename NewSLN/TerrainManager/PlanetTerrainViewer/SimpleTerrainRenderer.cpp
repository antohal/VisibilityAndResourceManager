#include "SimpleTerrainRenderer.h"

void CSimpleTerrainRenderer::Init(CTerrainObjectManager* in_pTerrainObjectManager)
{
	_pTerrainObjectManager = in_pTerrainObjectManager;
}

CSimpleTerrainRenderObject* CSimpleTerrainRenderer::CreateObject(TerrainObjectID ID)
{

}

void CSimpleTerrainRenderer::DeleteObject(TerrainObjectID ID)
{

}

int CSimpleTerrainRenderer::Render(CD3DGraphicsContext * in_pContext)
{
	return 0;
}

void CSimpleTerrainRenderer::SetLightParameters(const vm::Vector3df & in_vDirection, const vm::Vector3df & in_vDiffuse)
{
	_vLightDirection = in_vDirection;
	_vLightDiffuse = in_vDiffuse;
}
