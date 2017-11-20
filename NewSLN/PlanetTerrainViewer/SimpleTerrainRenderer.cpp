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

//
// CSimpleTerrainRenderObject
//

CSimpleTerrainRenderObject::CSimpleTerrainRenderObject(CSimpleTerrainRenderer * in_pRenderer, const STerrainBlockParams* in_pParams, TerrainObjectID ID)
	: _owner(in_pRenderer), _ID(ID)
{
	// �������� ����� ������� �� TerrainManager:
	std::wstring wsTextureFileName = _owner->GetTerrainManager()->GetTextureFileName(ID);
	std::wstring wsHeightmapFileName = _owner->GetTerrainManager()->GetHeightmapFileName(ID);

	LogMessage("Loading texture '%ls'", wsTextureFileName.c_str());
	_wsTextureFileName = wsTextureFileName;

	_pLoadThread = new std::thread([this]() {

		_owner->LockLoadMutex();

		ID3D11ShaderResourceView* pResourceView = nullptr;

		// ��������� ��������
		HRESULT result = D3DX11CreateShaderResourceViewFromFileW(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), _wsTextureFileName.c_str(), NULL, NULL, &pResourceView, NULL);
		if (FAILED(result))
		{

			_owner->GetTerrainManager()->SetDataReady(_ID);

			//LogMessage("CD3DStaticTerrainMaterial::Load: Error loading texture %ls", _wsTextureFileName.c_str());

			_owner->UnlockLoadMutex();
			return;
		}


		//std::this_thread::sleep_for(1s);

		_pTextureSRV = pResourceView;
		_owner->GetTerrainManager()->SetDataReady(_ID);

		_owner->UnlockLoadMutex();
	});

}

CSimpleTerrainRenderObject::~CSimpleTerrainRenderObject()
{
	if (_pLoadThread)
	{
		_pLoadThread->detach();

		delete _pLoadThread;
	}

	// ����������� ��������
	if (_pTextureSRV)
	{
		_pTextureSRV->Release();
		_pTextureSRV = nullptr;
	}

	LogMessage("Unloading texture '%ls'", _wsTextureFileName.c_str());
}

void CSimpleTerrainRenderObject::SetIndexAndVertexBuffers(CD3DGraphicsContext* in_pContext)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(SVertex);
	unsigned int offset = 0;

	STriangulation* pTriangulation = nullptr;
	_owner->GetTerrainManager()->GetTerrainObjectTriangulation(_ID, &pTriangulation);

	if (!pTriangulation)
	{
		return;
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

	}
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
	if (_pTriangulationsThread)
	{

		_bTriangulationThreadFinished = true;

		if (_pTriangulationsThread->joinable() && _pTriangulationsThread->native_handle())
		{
			_pTriangulationsThread->join();
		}

		delete _pTriangulationsThread;

		_pTriangulationsThread = nullptr;
	}
}

CSimpleTerrainRenderer::~CSimpleTerrainRenderer()
{
	if (_pD3DXEffect)
		_pD3DXEffect->Release();


	if (_pTriangulationsThread)
	{

		_bTriangulationThreadFinished = true;

		if (_pTriangulationsThread->joinable() && _pTriangulationsThread->native_handle())
		{
			_pTriangulationsThread->join();
		}

		delete _pTriangulationsThread;
	}

	if (_pHeightfieldConverter)
		delete _pHeightfieldConverter;

	for (auto it = _mapTerrainRenderObjects.begin(); it != _mapTerrainRenderObjects.end(); it++)
	{
		delete it->second;
	}

	_mapTerrainRenderObjects.clear();

	FinalizeShader();
}

void CSimpleTerrainRenderer::Init(CTerrainManager* in_pTerrainManager, float in_fWorldScale)
{
	_pTerrainManager = in_pTerrainManager;

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
	_pHeightfieldConverter->SetHeightScale(30.f);
	//_pHeightfieldConverter->SetHeightScale(20.f);

	//_pHeightfieldConverter->SetHeightScale(0);

	//_pHeightfieldConverter->SetNormalDivisionAngles(1, 2);

	_pTerrainManager->SetHeightfieldConverter(_pHeightfieldConverter);

	//---------------------------------------------

	//ID3D11ShaderResourceView* pTextureSRV = nullptr;

	////ID3D11Texture2D* tex;

	//D3DX11_IMAGE_INFO iinfo;
	//D3DX11GetImageInfoFromFileW(L"E://H_00_00.dds", nullptr, &iinfo, nullptr);

	//D3DX11_IMAGE_LOAD_INFO info;

	//info.Width = 257;
	//info.Height = 257;
	//info.Depth = 1;
	//info.FirstMipLevel = 0;
	//info.MipLevels = 1;
	//info.Usage = D3D11_USAGE_STAGING;
	//info.BindFlags = 0;
	//info.Format = DXGI_FORMAT_R32_FLOAT;
	//info.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	//info.MiscFlags = 0;
	//info.Filter = D3DX11_FILTER_NONE;
	//info.MipFilter = D3DX11_FILTER_NONE;
	//info.pSrcInfo = &iinfo;


	//ID3D11Resource* res;
	//HRESULT result = D3DX11CreateTextureFromFileW(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), L"E://H_00_00.dds", &info, NULL, &res, NULL);
	//if (FAILED(result))
	//	return;

	///*SHeightfield hf;
	//_pHeightfieldConverter->ReadHeightfieldDataFromTexture(L"E://H_00_00.dds", hf, 1);

	//pTextureSRV = hf.pTextureSRV;*/

	////ID3D11Resource* resource = NULL;
	////D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	////if (pTextureSRV)
	////{
	////	pTextureSRV->GetResource(&resource);
	////	pTextureSRV->GetDesc(&srvDesc);
	////}
	////else
	////	return;

	////ID3D11Texture2D* tex = (ID3D11Texture2D*)resource;
	//ID3D11Texture2D* tex = (ID3D11Texture2D*)res;
	//if (tex)
	//{
	//	D3D11_TEXTURE2D_DESC description;
	//	tex->GetDesc(&description);


	//	//description.Usage = D3D11_USAGE_DYNAMIC;
	//	//description.MipLevels = 1;
	//	//description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;// | D3D11_CPU_ACCESS_WRITE;


	//	///ID3D11Texture2D* memTexture = nullptr;
	//	//GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice()->CreateTexture2D(&description, nullptr, &memTexture);
	//	//GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDeviceContext()->CopyResource(memTexture, tex);

	//	D3D11_MAPPED_SUBRESOURCE subresource;
	//	HRESULT hr = GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDeviceContext()->Map(tex, 0, D3D11_MAP_READ, 0, &subresource);

	//	if (hr == S_OK)
	//	{

	//		float* pfData = (float*)subresource.pData;

	//		FILE* fp = fopen("E://hmap.dat", "wt");

	//		unsigned int iData = 0;

	//		while (iData < subresource.DepthPitch)
	//		{
	//			unsigned int iRow = subresource.RowPitch;

	//			for (unsigned int i = 0; i < iRow / 4; i++)
	//			{
	//				fprintf(fp, "%f ", pfData[iData / 4 + i]);
	//			}

	//			fprintf(fp, "\n");

	//			iData += iRow;
	//		}

	//		fclose(fp);


	//		GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDeviceContext()->Unmap(tex, 0);
	//	}
	//}


	StartUpdateTriangulationsThread();
}

CSimpleTerrainRenderObject* CSimpleTerrainRenderer::CreateObject(TerrainObjectID ID)
{
	CSimpleTerrainRenderObject* pNewObject = new CSimpleTerrainRenderObject(this, _pTerrainManager->GetTerrainObjectParams(ID), ID);
	_mapTerrainRenderObjects[ID] = pNewObject;

	return pNewObject;
}

void CSimpleTerrainRenderer::DeleteObject(TerrainObjectID ID)
{
	auto it = _mapTerrainRenderObjects.find(ID);

	if (it != _mapTerrainRenderObjects.end())
	{
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

	_uiTriangulationsCountParam = _pTextBlock->AddParameter(L"���������� ������������ � ������");
	_uiHeightfieldsCountParam = _pTextBlock->AddParameter(L"���������� ���� ����� � ������");
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

	// render queued objects

	for (const TerrainObjectID& ID : _lstRenderQueue)
	{
		auto itObj = _mapTerrainRenderObjects.find(ID);

		if (itObj == _mapTerrainRenderObjects.end())
		{
			LogMessage("Cannot find such render object (TerrainObjectID = %d)", ID);
			continue;
		}

		CSimpleTerrainRenderObject* pObj = itObj->second;

		if (pObj->GetIndexCount() == 0)
			continue;

		pObj->SetIndexAndVertexBuffers(in_pContext);

		DrawIndexedByShader(in_pContext->GetSystem()->GetDeviceContext(), pObj->GetTextureResourceView(), pObj->GetIndexCount());

		numPrimitives += pObj->GetIndexCount() / 3;
	}

	_visibleObjsCount = _lstRenderQueue.size();


	_lstRenderQueue.clear();


	if (_pTextBlock)
	{
		_pTextBlock->SetParameterValue(_uiTriangulationsCountParam, _pTerrainManager->GetTriangulationsCount());
		_pTextBlock->SetParameterValue(_uiHeightfieldsCountParam, _pTerrainManager->GetHeightfieldsCount());
	}

	return numPrimitives;
}

int CSimpleTerrainRenderer::GetVisibleObjectsCount() const
{
	return _visibleObjsCount;
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

void CSimpleTerrainRenderer::LockDeviceContext()
{
	GetHeightfieldConverter()->LockDeviceContext();
}

void CSimpleTerrainRenderer::UnlockDeviceContext()
{
	GetHeightfieldConverter()->UnlockDeviceContext();
}

void CSimpleTerrainRenderer::LockLoadMutex()
{
	_loadMutex.lock();
}

void CSimpleTerrainRenderer::UnlockLoadMutex()
{
	_loadMutex.unlock();
}

void CSimpleTerrainRenderer::StartUpdateTriangulationsThread()
{
	_pTriangulationsThread = new std::thread([this]() {

		while (!_bTriangulationThreadFinished)
		{
			if (!_pTerrainManager->UpdateTriangulations())
			{
				std::this_thread::sleep_for(1ms);
			}
		}

	});
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