#include "SimpleTerrainRenderer.h"

#include "GraphicsContext.h"
#include "Application.h"

#include "TerrainDataManager.h"

#include <d3dx11async.h>
#include <fstream>
#include "Log.h"

#include <string>
#include <chrono>

#include <d3dcompiler.h>

#include "CommonStates.h"

//
// CSimpleTerrainRenderObject
//

CSimpleTerrainRenderObject::CSimpleTerrainRenderObject(CSimpleTerrainRenderer * in_pRenderer, const STerrainBlockParams* in_pParams, TerrainObjectID ID)
	: _owner(in_pRenderer), _ID(ID)
{
}

CSimpleTerrainRenderObject::~CSimpleTerrainRenderObject()
{
	// освобождаем текстуру
	if (_pTextureSRV)
	{
		_pTextureSRV->Release();
		_pTextureSRV = nullptr;
	}


	LogMessage("Removing terrain object '%d'", _ID);
}

bool CSimpleTerrainRenderObject::SetIndexAndVertexBuffers(CD3DGraphicsContext* in_pContext)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(SVertex);
	unsigned int offset = 0;

	STriangulation* pTriangulation = nullptr;
	_owner->GetTerrainManager()->GetTerrainObjectTriangulation(_ID, &pTriangulation);

	if (!pTriangulation)
	{
		return false;
	}

	ID3D11Buffer* pVertexBuffer = pTriangulation->pVertexBuffer;
	ID3D11Buffer* pIndexBuffer = pTriangulation->pIndexBuffer;

	if (pVertexBuffer && pIndexBuffer)
	{

		_owner->GetHeightfieldConverter()->LockDeviceContext();

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		in_pContext->GetSystem()->GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		in_pContext->GetSystem()->GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


		_owner->GetHeightfieldConverter()->UnlockDeviceContext();

		return true;
	}

	return false;
}

unsigned int CSimpleTerrainRenderObject::GetIndexCount() const
{
	STriangulation* pTriangulation = nullptr;
	_owner->GetTerrainManager()->GetTerrainObjectTriangulation(_ID, &pTriangulation);

	if (!pTriangulation)
		return 0;

	if (pTriangulation->pIndexBuffer && pTriangulation->pVertexBuffer)
		return pTriangulation->nIndexCount;

	return 0;
}

//
// CSimpleTerrainRenderer
//

void CSimpleTerrainRenderer::Stop()
{
	delete _pTexturesQueue;
	delete _pHeightmapsQueue;
}

CSimpleTerrainRenderer::~CSimpleTerrainRenderer()
{
	if (_pD3DXEffect)
		_pD3DXEffect->Release();


	if (_pHeightfieldConverter)
		delete _pHeightfieldConverter;

	for (auto it = _mapTerrainRenderObjects.begin(); it != _mapTerrainRenderObjects.end(); it++)
	{
		if (it->second)
			delete it->second;
	}

	_mapTerrainRenderObjects.clear();

	FinalizeShader();
}

std::wstring CSimpleTerrainRenderer::GetTextureFileName(TerrainObjectID ID) const
{
	STerrainBlockParams params;
	_pTerrainManager->GetTerrainObjectParams(ID, &params);

	std::wstring wsTextureName = _pTerrainManager->GetRootDirectory();

	for (unsigned int i = 0; i <= params.uiDepth; i++)
	{
		std::wstring wsXX = std::to_wstring(params.aTreePosition[i].ucLattitudeIndex);
		std::wstring wsYY = std::to_wstring(params.aTreePosition[i].ucLongitudeIndex);

		if (wsXX.size() == 1)
			wsXX = L"0" + wsXX;

		if (wsYY.size() == 1)
			wsYY = L"0" + wsYY;

		if (i < params.uiDepth)
			wsTextureName += L"\\" + wsXX + L"_" + wsYY;
		else
			wsTextureName += L"\\T_" + wsXX + L"_" + wsYY + L".dds";
	}

	return wsTextureName;
}

std::wstring CSimpleTerrainRenderer::GetHeighmapFileName(TerrainObjectID ID) const
{
	STerrainBlockParams params;
	_pTerrainManager->GetTerrainObjectParams(ID, &params);

	std::wstring wsTextureName = _pTerrainManager->GetRootDirectory();

	for (unsigned int i = 0; i <= params.uiDepth; i++)
	{
		std::wstring wsXX = std::to_wstring(params.aTreePosition[i].ucLattitudeIndex);
		std::wstring wsYY = std::to_wstring(params.aTreePosition[i].ucLongitudeIndex);

		if (wsXX.size() == 1)
			wsXX = L"0" + wsXX;

		if (wsYY.size() == 1)
			wsYY = L"0" + wsYY;

		if (i < params.uiDepth)
			wsTextureName += L"\\" + wsXX + L"_" + wsYY;
		else
			wsTextureName += L"\\H_" + wsXX + L"_" + wsYY + L".dds";
	}

	return wsTextureName;
}


void CSimpleTerrainRenderer::Init(CTerrainManager* in_pTerrainManager, float in_fWorldScale)
{
	_pTerrainManager = in_pTerrainManager;
	_fWorldScale = in_fWorldScale * 100;

	InitializeShader(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), L"PlanetTerrainViewerShaders\\SimpleTerrain.vs", L"PlanetTerrainViewerShaders\\SimpleTerrain.ps");

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DX11CompileEffectFromFile(L"ComputeShaders\\HeightfieldConverter.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, flags, 0, GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(),
		&_pD3DXEffect, &errorBlob);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (_pD3DXEffect)
			_pD3DXEffect->Release();

		_pD3DXEffect = nullptr;
	}

	_pHeightfieldConverter = new HeightfieldConverter;

	if (_pD3DXEffect)
		_pHeightfieldConverter->Init(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDeviceContext(), _pD3DXEffect);
	else
		_pHeightfieldConverter->Init(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDeviceContext(), L"ComputeShaders\\HeightfieldConverter.hlsl");

	_pHeightfieldConverter->SetWorldScale(in_fWorldScale);
	_pHeightfieldConverter->SetHeightScale(10.f);
	//_pHeightfieldConverter->SetHeightScale(0);

	//_pHeightfieldConverter->SetNormalDivisionAngles(1, 2);

	_pTerrainManager->SetHeightfieldConverter(_pHeightfieldConverter);

	_sortTexturesLoadQueueFunc = [this](size_t ID) -> float {
		return TextureSortValue(ID);
	};

	_sortHeightmapsLoadQueueFunc = [this](size_t ID) -> float {
		return HeightmapSortValue(ID);
	};

	_pTexturesQueue = new CTextureLoadQueue(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), [this](size_t ID, ID3D11ShaderResourceView* tex) {
		TextureLoadFinished(ID, tex);
	}, _sortTexturesLoadQueueFunc);

	_pHeightmapsQueue = new CTextureLoadQueue(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), [this](size_t ID, ID3D11ShaderResourceView* tex) {
		HeightmapLoadFinished(ID, tex);
	}, _sortHeightmapsLoadQueueFunc);

	InitDebugRenderer();
}

float CSimpleTerrainRenderer::GetTerrainObjectCosToCameraDir(TerrainObjectID ID)
{
	CD3DCamera* pCamera = GetApplicationHandle()->GetGraphicsContext()->GetScene()->GetMainCamera();
	
	D3DXVECTOR3 vObjPos;
	_pTerrainManager->GetTerrainObjectCenter(ID, &vObjPos);

	return vm::dot_prod(vm::normalize(vm::Vector3df(vObjPos.x, vObjPos.y, vObjPos.z) - pCamera->GetPos()), pCamera->GetDir());
}

float CSimpleTerrainRenderer::TextureSortValue(TerrainObjectID ID)
{
	if (_setNotReadyObjectsInFrustum.find(ID) != _setNotReadyObjectsInFrustum.end())
		return 1.f;

	return GetTerrainObjectCosToCameraDir(ID);
}

float CSimpleTerrainRenderer::HeightmapSortValue(TerrainObjectID ID)
{
	if (_setNotReadyObjectsInFrustum.find(ID) != _setNotReadyObjectsInFrustum.end())
		return 1.f;

	TerrainObjectID neighbours[8];
	_pTerrainManager->GetTerrainObjectNeighbours(ID, neighbours);

	for (int i = 0; i < 8; i++)
	{
		if (_setNotReadyObjectsInFrustum.find(neighbours[i]) != _setNotReadyObjectsInFrustum.end())
		{
			return 1.f;
		}
	}

	return GetTerrainObjectCosToCameraDir(ID);
}

void CSimpleTerrainRenderer::TextureLoadFinished(TerrainObjectID ID, ID3D11ShaderResourceView* tex)
{
	if (CSimpleTerrainRenderObject* obj = _mapTerrainRenderObjects[ID])
	{
		if (tex)
		{
			obj->_pTextureSRV = tex;
			tex->AddRef();
		}

	}

	GetTerrainManager()->SetTextureReady(ID);
}

void CSimpleTerrainRenderer::ProcessLoadedTextures()
{
	_pHeightmapsQueue->Process();
	_pTexturesQueue->Process();
}

void CSimpleTerrainRenderer::SortLoadQueue()
{
	_setNotReadyObjectsInFrustum.clear();

	for (size_t i = 0; i < _pTerrainManager->GetNotReadyObjectsInFrustumCount(); i++)
	{
		_setNotReadyObjectsInFrustum.insert(_pTerrainManager->GetNotReadyObjectInFrustumID(i));
	}


	_pHeightmapsQueue->Sort();
	_pTexturesQueue->Sort();
}

void CSimpleTerrainRenderer::HeightmapLoadFinished(TerrainObjectID ID, ID3D11ShaderResourceView* tex)
{
	GetTerrainManager()->SetHeightmapReady(ID, tex);
}

CSimpleTerrainRenderObject* CSimpleTerrainRenderer::CreateObject(TerrainObjectID ID)
{
	std::lock_guard<std::mutex> hfLock(_objMutex);

	STerrainBlockParams params;
	_pTerrainManager->GetTerrainObjectParams(ID, &params);

	CSimpleTerrainRenderObject* pNewObject = new CSimpleTerrainRenderObject(this, &params, ID);
	_mapTerrainRenderObjects[ID] = pNewObject;

	return pNewObject;
}

void CSimpleTerrainRenderer::DeleteObject(TerrainObjectID ID)
{
	std::lock_guard<std::mutex> hfLock(_objMutex);

	auto it = _mapTerrainRenderObjects.find(ID);

	if (it != _mapTerrainRenderObjects.end())
	{
		if (it->second)
			delete it->second;

		_mapTerrainRenderObjects.erase(it);
	}
}

void CSimpleTerrainRenderer::AddObjectToRenderQueue(TerrainObjectID objID)
{
	_lstRenderQueue.push_back(objID);
}

void CSimpleTerrainRenderer::SetDebugTextBlock(CDirect2DTextBlock* block)
{
	_pTextBlock = block;

	_uiTriangulationsCountParam = _pTextBlock->AddParameter(L"Триангуляций");
	_uiPotentiallyVisibleCount = _pTextBlock->AddParameter(L"Потенциально видимых объектов");
	_uiHeightfieldsCountParam = _pTextBlock->AddParameter(L"Карт высот");

	_uiTexturesQueueParam =		_pTextBlock->AddParameter(L"Текстур в очереди");
	_uiHeightmapsQueueParam =	_pTextBlock->AddParameter(L"Карт высот в очереди");

	_uiBoundBoxCalculatingParam = _pTextBlock->AddParameter(L"Баундбоксов считается");

	_uiNotReadyInFrustumCountParam = _pTextBlock->AddParameter(L"Неготово видимых");
	_uiClosestDistParam = _pTextBlock->AddParameter(L"Минимальное расстояние до поверхности");
}

int CSimpleTerrainRenderer::Render(CD3DGraphicsContext * in_pContext)
{
	ID3D11DeviceContext* deviceContext = in_pContext->GetSystem()->GetDeviceContext();

	D3DXMATRIX mViewMatrix;
	in_pContext->GetScene()->GetMainCamera()->GetViewMatrix(mViewMatrix);

	SetShaderParameters(in_pContext, mViewMatrix, *in_pContext->GetSystem()->GetProjectionMatrix());


	GetHeightfieldConverter()->LockDeviceContext();

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_pInputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(_pVertexShader, NULL, 0);
	deviceContext->PSSetShader(_pPixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &_pSampleState);

	GetHeightfieldConverter()->UnlockDeviceContext();

	int numPrimitives = 0;


	vm::Vector3df vCamPos = GetApplicationHandle()->GetGraphicsContext()->GetScene()->GetMainCamera()->GetPos();
	D3DXVECTOR3 cameraPosition(vCamPos[0], vCamPos[1], vCamPos[2]);
	D3DXVECTOR3 objectProjection, objectNormal;

	// render queued objects

	for (const TerrainObjectID& ID : _lstRenderQueue)
	{
		std::lock_guard<std::mutex> hfLock(_objMutex);

		auto itObj = _mapTerrainRenderObjects.find(ID);

		if (itObj == _mapTerrainRenderObjects.end())
		{
			LogMessage("Cannot find such render object (TerrainObjectID = %d)", ID);
			continue;
		}

		CSimpleTerrainRenderObject* pObj = itObj->second;

		if (!pObj || pObj->GetIndexCount() == 0)
			continue;

		if (pObj->SetIndexAndVertexBuffers(in_pContext))
		{
			DrawIndexedByShader(in_pContext->GetSystem()->GetDeviceContext(), pObj->GetTextureResourceView(), pObj->GetIndexCount());

			numPrimitives += pObj->GetIndexCount() / 3;
		}



		// calculate closest point
		if (_pTerrainManager->GetTerrainObjectProjection(ID, &cameraPosition, &objectProjection, &objectNormal))
		{
			_pTerrainManager->GetTerrainObjectClosestPoint(ID, &cameraPosition, &_vClosestPoint, &objectNormal);
			_fClosestDist = D3DXVec3Length(&(_vClosestPoint - cameraPosition));
		}

	}

	_visibleObjsCount = _lstRenderQueue.size();




	if (_pTextBlock)
	{
		int texturesCountToLoad = _pTexturesQueue->CountInQueue();
		int heightmapsCountToLoad = _pHeightmapsQueue->CountInQueue();

		_pTextBlock->SetParameterValue(_uiTriangulationsCountParam, _pTerrainManager->GetTriangulationsCount());
		_pTextBlock->SetParameterValue(_uiPotentiallyVisibleCount, _pTerrainManager->GetPotentiallyVisibleObjectsCount());
		_pTextBlock->SetParameterValue(_uiHeightfieldsCountParam, _pTerrainManager->GetHeightfieldsCount());

		_pTextBlock->SetParameterValue(_uiTexturesQueueParam, texturesCountToLoad);
		_pTextBlock->SetParameterValue(_uiHeightmapsQueueParam, heightmapsCountToLoad);

		_pTextBlock->SetParameterValue(_uiBoundBoxCalculatingParam, _pTerrainManager->GetBoundBoxToBeCalculatedCount());

		_pTextBlock->SetParameterValue(_uiNotReadyInFrustumCountParam, _pTerrainManager->GetNotReadyObjectsInFrustumCount());
		_pTextBlock->SetParameterValue(_uiClosestDistParam, _fClosestDist);
	}

	if (_bDebugRenderEnabled)
		RenderDebug();


	_lstRenderQueue.clear();

	return numPrimitives;
}

int CSimpleTerrainRenderer::GetVisibleObjectsCount() const
{
	return _visibleObjsCount;
}

void CSimpleTerrainRenderer::SwitchDebugRender()
{
	_bDebugRenderEnabled = !_bDebugRenderEnabled;
}

void CSimpleTerrainRenderer::SetLightParameters(const vm::Vector3df & in_vDirection, const vm::Vector3df & in_vDiffuse)
{
	_vLightDirection = in_vDirection;
	_vLightDiffuse = in_vDiffuse;
}

bool CSimpleTerrainRenderer::SetShaderParameters(CD3DGraphicsContext* in_pContext, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;

	D3DXVECTOR3 lightDirection = D3DXVECTOR3((float)_vLightDirection[0], (float)_vLightDirection[1], (float)_vLightDirection[2]);
	D3DXVECTOR4 diffuseColor = D3DXVECTOR4((float)_vLightDiffuse[0], (float)_vLightDiffuse[1], (float)_vLightDiffuse[2], 1);


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	if (!_pMatrixBuffer)
		return false;

	GetHeightfieldConverter()->LockDeviceContext();

	// Lock the constant buffer so it can be written to.
	result = in_pContext->GetSystem()->GetDeviceContext()->Map(_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	GetHeightfieldConverter()->UnlockDeviceContext();

	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;


	GetHeightfieldConverter()->LockDeviceContext();

	// Unlock the constant buffer.
	in_pContext->GetSystem()->GetDeviceContext()->Unmap(_pMatrixBuffer, 0);

	GetHeightfieldConverter()->UnlockDeviceContext();

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	GetHeightfieldConverter()->LockDeviceContext();

	// Now set the constant buffer in the vertex shader with the updated values.
	in_pContext->GetSystem()->GetDeviceContext()->VSSetConstantBuffers(bufferNumber, 1, &_pMatrixBuffer);

	GetHeightfieldConverter()->UnlockDeviceContext();

	if (!_pLightBuffer)
		return false;

	GetHeightfieldConverter()->LockDeviceContext();

	// Lock the light constant buffer so it can be written to.
	result = in_pContext->GetSystem()->GetDeviceContext()->Map(_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	GetHeightfieldConverter()->UnlockDeviceContext();

	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->mode = _RenderingMode;

	GetHeightfieldConverter()->LockDeviceContext();

	// Unlock the constant buffer.
	in_pContext->GetSystem()->GetDeviceContext()->Unmap(_pLightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	in_pContext->GetSystem()->GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &_pLightBuffer);

	GetHeightfieldConverter()->UnlockDeviceContext();

	return true;
}

void CSimpleTerrainRenderer::DrawIndexedByShader(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, unsigned int indexCount)
{
	// Set shader texture resource in the pixel shader.

	GetHeightfieldConverter()->LockDeviceContext();

	ID3D11ShaderResourceView* resourceViews[1] = { texture };
	deviceContext->PSSetShaderResources(0, 1, resourceViews);


	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);


	GetHeightfieldConverter()->UnlockDeviceContext();

	return;
}

void CSimpleTerrainRenderer::InitDebugRenderer()
{
	_primitiveBatch = new PrimitiveBatch<VertexPositionColor>(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDeviceContext());

	_debugEffect.reset(new BasicEffect(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice()));
	
	_debugEffect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	_debugEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice()->
		CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, shaderByteCode, byteCodeLength, &_debugInputLayout);
}

void CSimpleTerrainRenderer::RenderDebug()
{
	ID3D11DeviceContext* deviceContext = GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDeviceContext();

	CommonStates states(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice());
//	deviceContext->OMSetBlendState(states.Opaque(), nullptr, 0xFFFFFFFF);
//	deviceContext->OMSetDepthStencilState(states.DepthNone(), 0);
//	deviceContext->RSSetState(states.CullCounterClockwise());

	deviceContext->RSSetState(states.Wireframe());

	DirectX::XMMATRIX matProj;
	memcpy(&matProj, GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetProjectionMatrix(), sizeof(DirectX::XMMATRIX));


	D3DXMATRIX d3dView;
	GetApplicationHandle()->GetGraphicsContext()->GetScene()->GetMainCamera()->GetViewMatrix(d3dView);

	vm::Vector3df vCamPos = GetApplicationHandle()->GetGraphicsContext()->GetScene()->GetMainCamera()->GetPos();
	D3DXVECTOR3 cameraPosition(vCamPos[0], vCamPos[1], vCamPos[2]);

	DirectX::XMMATRIX matView;
	memcpy(&matView, d3dView, sizeof(matView));


	_debugEffect->SetProjection(matProj);
	_debugEffect->SetView(matView);

	_debugEffect->Apply(deviceContext);
	deviceContext->IASetInputLayout(_debugInputLayout);

	_primitiveBatch->Begin();
//	_primitiveBatch->DrawLine(VertexPositionColor(XMFLOAT3(0, 0, 0), XMFLOAT4(1, 0,0,1)), VertexPositionColor(XMFLOAT3(_fWorldScale * 7000000, 0, 0), XMFLOAT4(1, 0, 0, 1)));

	// Draw closest point
	{
		const float fCrossWidth = 0.05 * D3DXVec3Length(&(_vClosestPoint - cameraPosition));
		_primitiveBatch->DrawLine(
			VertexPositionColor(XMFLOAT3(_vClosestPoint.x - fCrossWidth, _vClosestPoint.y, _vClosestPoint.z), XMFLOAT4(1, 1, 1, 1)),
			VertexPositionColor(XMFLOAT3(_vClosestPoint.x + fCrossWidth, _vClosestPoint.y, _vClosestPoint.z), XMFLOAT4(1, 1, 1, 1))
		);

		_primitiveBatch->DrawLine(
			VertexPositionColor(XMFLOAT3(_vClosestPoint.x, _vClosestPoint.y - fCrossWidth, _vClosestPoint.z), XMFLOAT4(1, 1, 1, 1)),
			VertexPositionColor(XMFLOAT3(_vClosestPoint.x, _vClosestPoint.y + fCrossWidth, _vClosestPoint.z), XMFLOAT4(1, 1, 1, 1))
		);

		_primitiveBatch->DrawLine(
			VertexPositionColor(XMFLOAT3(_vClosestPoint.x, _vClosestPoint.y, _vClosestPoint.z - fCrossWidth), XMFLOAT4(1, 1, 1, 1)),
			VertexPositionColor(XMFLOAT3(_vClosestPoint.x, _vClosestPoint.y, _vClosestPoint.z + fCrossWidth), XMFLOAT4(1, 1, 1, 1))
		);
	}

	for (const TerrainObjectID& ID : _lstRenderQueue)
	{
		D3DXVECTOR3 corners[8];
		_pTerrainManager->GetTerrainObjectBoundBoxCorners(ID, corners);

		D3DXVECTOR3 objectProjection;
		D3DXVECTOR3 objectNormal;
		//if (_pTerrainManager->GetTerrainObjectProjection(ID, &cameraPosition, &objectProjection))
		_pTerrainManager->GetTerrainObjectProjection(ID, &cameraPosition, &objectProjection, &objectNormal);
		{
			const float fCrossWidth = 0.05 * D3DXVec3Length(&(objectProjection - cameraPosition));
			const float fNormalLength = 0.15 * D3DXVec3Length(&(objectProjection - cameraPosition));

			//@{ cross in point
			_primitiveBatch->DrawLine(
				VertexPositionColor(XMFLOAT3(objectProjection.x - fCrossWidth, objectProjection.y, objectProjection.z), XMFLOAT4(1, 0, 0, 1)),
				VertexPositionColor(XMFLOAT3(objectProjection.x + fCrossWidth, objectProjection.y, objectProjection.z), XMFLOAT4(1, 0, 0, 1))
			);

			_primitiveBatch->DrawLine(
				VertexPositionColor(XMFLOAT3(objectProjection.x, objectProjection.y - fCrossWidth, objectProjection.z), XMFLOAT4(0, 1, 0, 1)),
				VertexPositionColor(XMFLOAT3(objectProjection.x, objectProjection.y + fCrossWidth, objectProjection.z), XMFLOAT4(0, 1, 0, 1))
			);

			_primitiveBatch->DrawLine(
				VertexPositionColor(XMFLOAT3(objectProjection.x, objectProjection.y, objectProjection.z - fCrossWidth), XMFLOAT4(0, 0, 0.7, 1)),
				VertexPositionColor(XMFLOAT3(objectProjection.x, objectProjection.y, objectProjection.z + fCrossWidth), XMFLOAT4(0, 0, 0.7, 1))
			);
			//@}

			// normal
			_primitiveBatch->DrawLine(
				VertexPositionColor(XMFLOAT3(objectProjection.x, objectProjection.y, objectProjection.z), XMFLOAT4(0, 0, 0.7, 1)),
				VertexPositionColor(XMFLOAT3(objectProjection.x + fNormalLength*objectNormal.x, objectProjection.y + fNormalLength*objectNormal.y, objectProjection.z + fNormalLength*objectNormal.z), XMFLOAT4(1, 1, 0.7, 1))
			);
		}

		D3DXVECTOR3 vObjPos;
		_pTerrainManager->GetTerrainObjectCenter(ID, &vObjPos);
		{
			const float fCrossWidth = 0.05 * D3DXVec3Length(&(vObjPos - cameraPosition));

			//@{ cross in point
			_primitiveBatch->DrawLine(
				VertexPositionColor(XMFLOAT3(vObjPos.x - fCrossWidth, vObjPos.y, vObjPos.z), XMFLOAT4(1, 1, 0, 1)),
				VertexPositionColor(XMFLOAT3(vObjPos.x + fCrossWidth, vObjPos.y, vObjPos.z), XMFLOAT4(1, 1, 0, 1))
			);

			_primitiveBatch->DrawLine(
				VertexPositionColor(XMFLOAT3(vObjPos.x, vObjPos.y - fCrossWidth, vObjPos.z), XMFLOAT4(1, 1, 0, 1)),
				VertexPositionColor(XMFLOAT3(vObjPos.x, vObjPos.y + fCrossWidth, vObjPos.z), XMFLOAT4(1, 1, 0, 1))
			);

			_primitiveBatch->DrawLine(
				VertexPositionColor(XMFLOAT3(vObjPos.x, vObjPos.y, vObjPos.z - fCrossWidth), XMFLOAT4(1, 1, 0, 1)),
				VertexPositionColor(XMFLOAT3(vObjPos.x, vObjPos.y, vObjPos.z + fCrossWidth), XMFLOAT4(1, 1, 0, 1))
			);
			//@}
		}


		XMFLOAT3 vCorners[8];
		for (int i = 0; i < 8; i++)
			memcpy(&vCorners[i], &corners[i], sizeof(XMFLOAT3));

		
		XMFLOAT4 pvColors[10] = {

			XMFLOAT4(0.5, 1, 0, 1),
			XMFLOAT4(1, 0.5, 1, 1),
			XMFLOAT4(1, 1, 0.5, 1),
			XMFLOAT4(1, 0.5, 0.5, 1),
			XMFLOAT4(1, 0, 0, 1),
			XMFLOAT4(0, 0, 1, 1),
			XMFLOAT4(0, 1, 0, 1),
			XMFLOAT4(1, 1, 1, 1),
			XMFLOAT4(1, 1, 0, 1),
			XMFLOAT4(1, 0, 1, 1)

		};
		
		
		XMFLOAT4 vColor = pvColors[ID % 10];

		_primitiveBatch->DrawQuad(
			VertexPositionColor(vCorners[0], vColor),
			VertexPositionColor(vCorners[1], vColor),
			VertexPositionColor(vCorners[2], vColor),
			VertexPositionColor(vCorners[3], vColor));

		_primitiveBatch->DrawQuad(
			VertexPositionColor(vCorners[4], vColor),
			VertexPositionColor(vCorners[5], vColor),
			VertexPositionColor(vCorners[6], vColor),
			VertexPositionColor(vCorners[7], vColor));

		_primitiveBatch->DrawQuad(
			VertexPositionColor(vCorners[0], vColor),
			VertexPositionColor(vCorners[1], vColor),
			VertexPositionColor(vCorners[5], vColor),
			VertexPositionColor(vCorners[4], vColor));

		_primitiveBatch->DrawQuad(
			VertexPositionColor(vCorners[3], vColor),
			VertexPositionColor(vCorners[2], vColor),
			VertexPositionColor(vCorners[6], vColor),
			VertexPositionColor(vCorners[7], vColor));


		_primitiveBatch->DrawQuad(
			VertexPositionColor(vCorners[1], vColor),
			VertexPositionColor(vCorners[2], vColor),
			VertexPositionColor(vCorners[6], vColor),
			VertexPositionColor(vCorners[5], vColor));

		_primitiveBatch->DrawQuad(
			VertexPositionColor(vCorners[0], vColor),
			VertexPositionColor(vCorners[3], vColor),
			VertexPositionColor(vCorners[7], vColor),
			VertexPositionColor(vCorners[4], vColor));

	}

	_primitiveBatch->End();
}

void CSimpleTerrainRenderer::LockDeviceContext()
{
	GetHeightfieldConverter()->LockDeviceContext();
}

void CSimpleTerrainRenderer::UnlockDeviceContext()
{
	GetHeightfieldConverter()->UnlockDeviceContext();
}

void CSimpleTerrainRenderer::AppendTextureToLoad(TerrainObjectID ID)
{
	_pTexturesQueue->AddToLoad(ID, GetTextureFileName(ID));
}

void CSimpleTerrainRenderer::AppendHeightmapToLoad(TerrainObjectID ID)
{
	_pHeightmapsQueue->AddToLoad(ID, GetHeighmapFileName(ID));
}

void CSimpleTerrainRenderer::RemoveTextureFromLoad(TerrainObjectID ID)
{
	_pTexturesQueue->Remove(ID);
}

void CSimpleTerrainRenderer::RemoveHeightmapFromLoad(TerrainObjectID ID)
{
	_pHeightmapsQueue->Remove(ID);
}


bool CSimpleTerrainRenderer::InitializeShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
	unsigned int numElements;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			LogMessage("Missing Shader File %ls", vsFilename);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			LogMessage("Missing Shader File %ls", psFilename);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &_pVertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &_pPixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;



	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = 12; // D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;


	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = 24; // D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = 32; // D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;


	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&_pInputLayout);

	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &_pSampleState);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &_pMatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &_pLightBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void CSimpleTerrainRenderer::OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = (unsigned long)errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	LogMessage("Error compiling shader %ls.  Check shader-error.txt for message.", shaderFilename);

	return;
}

void CSimpleTerrainRenderer::FinalizeShader()
{
	// Release the light constant buffer.
	if (_pLightBuffer)
	{
		_pLightBuffer->Release();
		_pLightBuffer = 0;
	}

	// Release the matrix constant buffer.
	if (_pMatrixBuffer)
	{
		_pMatrixBuffer->Release();
		_pMatrixBuffer = 0;
	}

	// Release the sampler state.
	if (_pSampleState)
	{
		_pSampleState->Release();
		_pSampleState = 0;
	}

	// Release the layout.
	if (_pInputLayout)
	{
		_pInputLayout->Release();
		_pInputLayout = 0;
	}

	if (_debugInputLayout)
	{
		_debugInputLayout->Release();
		_debugInputLayout = 0;
	}

	// Release the pixel shader.
	if (_pPixelShader)
	{
		_pPixelShader->Release();
		_pPixelShader = 0;
	}

	// Release the vertex shader.
	if (_pVertexShader)
	{
		_pVertexShader->Release();
		_pVertexShader = 0;
	}
}