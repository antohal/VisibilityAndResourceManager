#include "SimpleTerrainRenderer.h"

#include "GraphicsContext.h"
#include "Application.h"

#include "TerrainDataManager.h"

#include <d3dx11async.h>
#include <fstream>
#include "Log.h"

#include <string>

//
// CSimpleTerrainRenderObject
//

CSimpleTerrainRenderObject::CSimpleTerrainRenderObject(CSimpleTerrainRenderer * in_pRenderer, const STerrainBlockParams* in_pParams, TerrainObjectID ID)
	: _owner(in_pRenderer)
{

	// получаем имена текстур из TerrainManager:
	std::wstring wsTextureFileName = _owner->GetTerrainManager()->GetTextureFileName(ID);
	std::wstring wsHeightmapFileName = _owner->GetTerrainManager()->GetHeightmapFileName(ID);

	// получаем конвертер карт высот для дальнейших операций с ним
	HeightfieldConverter* pHeightfieldConverter = _owner->GetHeightfieldConverter();

	// создадим объект в котором будет храниться карта высот и ее параметры
	SHeightfield heightfield;

	// считаем данные карты высот из файла
	pHeightfieldConverter->ReadHeightfieldDataFromTexture(wsHeightmapFileName.c_str(), heightfield);

	LogMessage("Loading faceset. Triangulating heightmap '%ls'", wsHeightmapFileName.c_str());

	// заполним граничные данные
	heightfield.Config.Coords.fMinLattitude = in_pParams->fMinLattitude;
	heightfield.Config.Coords.fMaxLattitude = in_pParams->fMaxLattitude;
	heightfield.Config.Coords.fMinLongitude = in_pParams->fMinLongitude;
	heightfield.Config.Coords.fMaxLongitude = in_pParams->fMaxLongitude;

	// Создадим триангуляцию с помощью ComputeShader. В объекте _triangulation лежат индексные и вертексные буферы
	pHeightfieldConverter->CreateTriangulationImmediate(&heightfield, &_triangulation);

	// карта высот нам больше не нужна, освобождаем ее
	pHeightfieldConverter->ReleaseHeightfield(&heightfield);

	LogMessage("Loading texture '%ls'", wsTextureFileName.c_str());
	_wsTextureFileName = wsTextureFileName;

	// Загружаем текстуру
	HRESULT result = D3DX11CreateShaderResourceViewFromFileW(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), wsTextureFileName.c_str(), NULL, NULL, &_pTextureSRV, NULL);
	if (FAILED(result))
	{
		LogMessage("CD3DStaticTerrainMaterial::Load: Error loading texture %ls", wsTextureFileName.c_str());
		return;
	}

}

CSimpleTerrainRenderObject::~CSimpleTerrainRenderObject()
{
	// освобождаем триангуляцию вместе с буферами вершин и индексов
	_owner->GetHeightfieldConverter()->ReleaseTriangulation(&_triangulation);

	// освобождаем текстуру
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


	ID3D11Buffer* pVertexBuffer = _triangulation.pVertexBuffer;
	ID3D11Buffer* pIndexBuffer = _triangulation.pIndexBuffer;

	if (pVertexBuffer && pIndexBuffer)
	{
		// Set the vertex buffer to active in the input assembler so it can be rendered.
		in_pContext->GetSystem()->GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		in_pContext->GetSystem()->GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	}
}

unsigned int CSimpleTerrainRenderObject::GetIndexCount() const
{
	if (_triangulation.pIndexBuffer && _triangulation.pVertexBuffer)
		return _triangulation.nIndexCount;

	return 0;
}

//
// CSimpleTerrainRenderer
//

CSimpleTerrainRenderer::~CSimpleTerrainRenderer()
{
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


	_pHeightfieldConverter = new HeightfieldConverter;

	_pHeightfieldConverter->Init(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDeviceContext(), L"ComputeShaders\\HeightfieldConverter.hlsl");

	_pHeightfieldConverter->SetWorldScale(in_fWorldScale);
	//_pHeightfieldConverter->SetHeightScale(1000.f);
	_pHeightfieldConverter->SetHeightScale(10.f);
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

int CSimpleTerrainRenderer::Render(CD3DGraphicsContext * in_pContext)
{
	ID3D11DeviceContext* deviceContext = in_pContext->GetSystem()->GetDeviceContext();

	D3DXMATRIX mViewMatrix;
	in_pContext->GetScene()->GetMainCamera()->GetViewMatrix(mViewMatrix);

	SetShaderParameters(in_pContext, mViewMatrix, *in_pContext->GetSystem()->GetProjectionMatrix());

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_pInputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(_pVertexShader, NULL, 0);
	deviceContext->PSSetShader(_pPixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &_pSampleState);

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

	// Lock the constant buffer so it can be written to.
	result = in_pContext->GetSystem()->GetDeviceContext()->Map(_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	/*vm::Vector3df p = in_pContext->GetScene()->GetMainCamera()->GetPos();
	dataPtr->vCamPos = vm::Vector4df(p[0], p[1], p[2], 1.0);

	vm::Vector3df d = in_pContext->GetScene()->GetMainCamera()->GetDir();
	dataPtr->vAxisX = vm::Vector4df(p[0], p[1], p[2], 1.0);

	vm::Vector3df u = in_pContext->GetScene()->GetMainCamera()->GetUp();
	dataPtr->vAxisZ = vm::Vector4df(u[0], u[1], u[2], 1.0);

	vm::Vector3df l = vm::cross(u, d);
	dataPtr->vAxisY = vm::Vector4df(l[0], l[1], l[2], 1.0);*/

	// Unlock the constant buffer.
	in_pContext->GetSystem()->GetDeviceContext()->Unmap(_pMatrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	in_pContext->GetSystem()->GetDeviceContext()->VSSetConstantBuffers(bufferNumber, 1, &_pMatrixBuffer);

	if (!_pLightBuffer)
		return false;

	// Lock the light constant buffer so it can be written to.
	result = in_pContext->GetSystem()->GetDeviceContext()->Map(_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
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

	// Unlock the constant buffer.
	in_pContext->GetSystem()->GetDeviceContext()->Unmap(_pLightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	in_pContext->GetSystem()->GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &_pLightBuffer);

	return true;
}

void CSimpleTerrainRenderer::DrawIndexedByShader(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, unsigned int indexCount)
{
	// Set shader texture resource in the pixel shader.

	ID3D11ShaderResourceView* resourceViews[1] = { texture };
	deviceContext->PSSetShaderResources(0, 1, resourceViews);


	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
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