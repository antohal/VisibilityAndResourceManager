#include "Log.h"
#include "Direct3DSystem.h"

#include "StaticTerrainRenderer.h"
#include "GraphicsContext.h"
#include "Application.h"

#include "vecmath.h"

#include <d3dx11async.h>
#include <fstream>
#include <d3dx10math.h>

//
// CD3DStaticTerrainObject
//

CD3DStaticTerrainObject::CD3DStaticTerrainObject(CD3DStaticTerrainRenderer* in_pOwner) : _owner(in_pOwner)
{

}

// Функция вычисления расстояния до объекта
float CD3DStaticTerrainObject::GetDistance(const D3DXVECTOR3* in_pvPointFrom) const
{
	return 0;
}

// Получить минимальное расстояние видимости до объекта
float CD3DStaticTerrainObject::GetMinimalVisibleDistance() const
{
	return 0;
}

// Получить максимальное расстояние видимости до объекта
float CD3DStaticTerrainObject::GetMaximalVisibleDistance() const
{
	return FLT_MAX;
}

// Получить родительский объект-лод
C3DBaseObject*	CD3DStaticTerrainObject::GetParentLODObject()
{
	return nullptr;
}

// Получить количество дочерних объектов-лодов
unsigned int	CD3DStaticTerrainObject::GetNumChildLODObjects()
{
	return 0;
}

// Получить дочерний лод-объект
C3DBaseObject*	CD3DStaticTerrainObject::GetChildLODObject(unsigned int id)
{
	return nullptr;
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
	_pFaceset->ComputeTriangulationCoords();

	STriangulationCoordsInfo info = _pFaceset->GetTriangulation().Info;

	_vBBoxMin = D3DXVECTOR3(
		(float)info.vBoundBoxMinimum[0],
		(float)info.vBoundBoxMinimum[1],
		(float)info.vBoundBoxMinimum[2]
		);

	_vBBoxMax = D3DXVECTOR3(
		(float)info.vBoundBoxMaximum[0],
		(float)info.vBoundBoxMaximum[1],
		(float)info.vBoundBoxMaximum[2]
		);

	/*vm::Vector3df vPos(info.vPosition);
	vm::Vector3df vXAxis(info.vXAxis);
	vm::Vector3df vYAxis(info.vYAxis);
	vm::Vector3df vZAxis(info.vZAxis);*/

	/*_mTransform.m[0][0] = (float)vXAxis[0];	_mTransform.m[0][1] = (float)vYAxis[0];	_mTransform.m[0][2] = (float)vZAxis[0];	_mTransform.m[0][3] = 0;
	_mTransform.m[1][0] = (float)vXAxis[1];	_mTransform.m[1][1] = (float)vYAxis[1];	_mTransform.m[1][2] = (float)vZAxis[1];	_mTransform.m[1][3] = 0;
	_mTransform.m[2][0] = (float)vXAxis[2];	_mTransform.m[2][1] = (float)vYAxis[2];	_mTransform.m[2][2] = (float)vZAxis[2];	_mTransform.m[2][3] = 0;

	_mTransform.m[3][0] = -(float)vm::dot(vXAxis, vPos);
	_mTransform.m[3][1] = -(float)vm::dot(vYAxis, vPos);
	_mTransform.m[3][2] = -(float)vm::dot(vZAxis, vPos);
	_mTransform.m[3][3] = 1;*/

	D3DXMatrixIdentity(&_mTransform);
}

void CD3DStaticTerrainObject::GetBoundBox(D3DXVECTOR3** ppBBMin, D3DXVECTOR3** ppBBMax)
{
	*ppBBMin = &_vBBoxMin;
	*ppBBMax = &_vBBoxMax;
}

D3DXMATRIX* CD3DStaticTerrainObject::GetWorldTransform()
{
	return &_mTransform;
}


//
// CD3DStaticTerrainFaceset
//

CD3DStaticTerrainFaceset::CD3DStaticTerrainFaceset(CD3DStaticTerrainRenderer* in_pOwner, CD3DStaticTerrainObject* in_pObject, CD3DStaticTerrainMaterial * in_pMaterial, const CTerrainBlockData* in_pTerrainBlockData)
	: _owner(in_pOwner), _pTerrainObject(in_pObject), _pMaterialRef(in_pMaterial), _pTerrainBlockData(in_pTerrainBlockData)
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
	if (!_pTerrainBlockData || !_pTerrainObject)
	{
		LogMessage("Error: null terrain block data or no terrain object");
	}

	LogMessage("Loading faceset. Triangulating heightmap '%ls'", _pTerrainBlockData->GetHeightmapFileName());

	_heightfield.ID = 0;

	_owner->GetHeightfieldConverter()->ReadHeightfieldDataFromTexture(_pTerrainBlockData->GetHeightmapFileName(), _heightfield);
	_owner->GetHeightfieldConverter()->CreateTriangulationImmediate(&_heightfield, &_triangulation);
	_owner->GetHeightfieldConverter()->ReleaseHeightfield(&_heightfield);
}

void CD3DStaticTerrainFaceset::Unload()
{
	if (!_pTerrainBlockData || !_pTerrainObject)
	{
		LogMessage("Error: null terrain block data or no terrain object");
	}

	LogMessage("UnLoading faceset.");

	_owner->GetHeightfieldConverter()->ReleaseTriangulation(&_triangulation);
}

void CD3DStaticTerrainFaceset::ComputeTriangulationCoords()
{
	if (!_pTerrainBlockData || !_pTerrainObject)
	{
		LogMessage("Error: null terrain block data or no terrain object");
	}

	_heightfield.Config.Coords.fMinLattitude = _pTerrainBlockData->GetMinimumLattitude();
	_heightfield.Config.Coords.fMaxLattitude = _pTerrainBlockData->GetMaximumLattitude();
	_heightfield.Config.Coords.fMinLongitude = _pTerrainBlockData->GetMinimumLongitude();
	_heightfield.Config.Coords.fMaxLongitude = _pTerrainBlockData->GetMaximumLongitude();

	_owner->GetHeightfieldConverter()->ComputeTriangulationCoords(_heightfield.Config.Coords, _triangulation.Info);
}

const STriangulation& CD3DStaticTerrainFaceset::GetTriangulation() const
{
	return _triangulation;
}

STriangulation& CD3DStaticTerrainFaceset::GetTriangulation()
{
	return _triangulation;
}

const CTerrainBlockData* CD3DStaticTerrainFaceset::GetTerrainBlockData() const
{
	return _pTerrainBlockData;
}

void CD3DStaticTerrainFaceset::SetIndexAndVertexBuffers(CD3DGraphicsContext* in_pContext)
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

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	in_pContext->GetSystem()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

unsigned int CD3DStaticTerrainFaceset::GetIndexCount() const
{
	if (_triangulation.pIndexBuffer && _triangulation.pVertexBuffer)
		return _triangulation.nIndexCount;

	return 0;
}

//
// CD3DStaticTerrainMaterial
//

CD3DStaticTerrainMaterial::CD3DStaticTerrainMaterial(CD3DStaticTerrainRenderer * in_pOwner, const std::wstring& in_wsFileName) : _pOwner(in_pOwner), _wsTextureFileName(in_wsFileName)
{
}

int CD3DStaticTerrainMaterial::Render(CD3DGraphicsContext * in_pContext)
{
	int numPrimitives = 0;

	for (CD3DStaticTerrainFaceset* faceset : _setVisibleFacesets)
	{
		if (faceset->GetIndexCount() == 0)
			continue;

		faceset->SetIndexAndVertexBuffers(in_pContext);
		_pOwner->DrawIndexedByShader(in_pContext->GetSystem()->GetDeviceContext(), _pTextureSRV, faceset->GetIndexCount());

		numPrimitives += faceset->GetIndexCount() / 3;
	}

	_setVisibleFacesets.clear();

	return numPrimitives;
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
	if (_wsTextureFileName.empty())
		return;

	LogMessage("Loading texture '%ls'", _wsTextureFileName.c_str());

	HRESULT result = D3DX11CreateShaderResourceViewFromFileW(GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetDevice(), _wsTextureFileName.c_str(), NULL, NULL, &_pTextureSRV, NULL);
	if (FAILED(result))
	{
		LogMessage("CD3DStaticTerrainMaterial::Load: Error loading texture %ls", _wsTextureFileName.c_str());
		return;
	}
}

void CD3DStaticTerrainMaterial::Unload()
{
	if (_pTextureSRV)
	{
		_pTextureSRV->Release();
		_pTextureSRV = nullptr;
	}

	LogMessage("Unloading texture '%ls'", _wsTextureFileName.c_str());
}

//
// CD3DStaticTerrainRenderer
//

CD3DStaticTerrainRenderer::CD3DStaticTerrainRenderer()
{
	_pTerrainDataManager = new CTerrainDataManager();
	_pHeightfieldConverter = new HeightfieldConverter();
}

void CD3DStaticTerrainRenderer::Init(CDirect3DSystem* in_pSystem, float in_fWorldScale)
{
	_pHeightfieldConverter->Init(in_pSystem->GetDevice(), in_pSystem->GetDeviceContext());

	_pHeightfieldConverter->SetHeightScale(20000.0f);
	_pHeightfieldConverter->SetWorldScale(in_fWorldScale);

	InitializeShader(in_pSystem->GetDevice(), L"PlanetTerrainViewerShaders\\SimpleTerrain.vs", L"PlanetTerrainViewerShaders\\SimpleTerrain.ps");
}

CD3DStaticTerrainRenderer::~CD3DStaticTerrainRenderer()
{
	FinalizeShader();

	LogMessage("Deleting objects");

	for (CD3DStaticTerrainObject* pObject : _vecTerrainObjects)
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
	delete _pHeightfieldConverter;
}

void CD3DStaticTerrainRenderer::LoadPlanet(const wchar_t * in_pcwszDirectory)
{
	LogMessage("Loading planet terrain info");

	_pTerrainDataManager->LoadTerrainDataInfo(in_pcwszDirectory, &_pPlanetTerrainData);

	CreateObjects();
}

void CD3DStaticTerrainRenderer::SetLightParameters(const vm::Vector3df& in_vDirection, const vm::Vector3df& in_vDiffuse)
{
	_vLightDirection = in_vDirection;
	_vLightDiffuse = in_vDiffuse;
}

void CD3DStaticTerrainRenderer::AddVisibleMaterial(CD3DStaticTerrainMaterial * in_pMaterial)
{
	_setVisibleMaterials.insert(in_pMaterial);
}

const CTerrainBlockData* CD3DStaticTerrainRenderer::GetTerrainDataForObject(C3DBaseObject* pObject) const
{
	auto it = _mapTerrainDataBlocks.find(pObject);

	if (it == _mapTerrainDataBlocks.end())
		return nullptr;

	return it->second;
}

const CTerrainBlockData* CD3DStaticTerrainRenderer::GetRootTerrainData() const
{
	return _pPlanetTerrainData;
}

int CD3DStaticTerrainRenderer::Render(CD3DGraphicsContext * in_pContext)
{
	D3DXMATRIX mViewMatrix;

	in_pContext->GetScene()->GetMainCamera()->GetViewMatrix(mViewMatrix);

	SetShaderParameters(in_pContext, mViewMatrix, *in_pContext->GetSystem()->GetProjectionMatrix());

	int numPrims = 0;

	for (CD3DStaticTerrainMaterial* pMaterial : _setVisibleMaterials)
	{
		numPrims += pMaterial->Render(in_pContext);
	}

	// TODO: end render, cleanup shaders

	_setVisibleMaterials.clear();

	return numPrims;
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
		return;

	case C3DRESOURCE_MATERIAL:
		{
			if (CD3DStaticTerrainMaterial* pMaterial = dynamic_cast<CD3DStaticTerrainMaterial*>(in_pResource))
			{
				pMaterial->Load();
			}
		}
		return;
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
				pFaceset->Unload();
			}
		}
		return;

	case C3DRESOURCE_MATERIAL:
		{
			if (CD3DStaticTerrainMaterial* pMaterial = dynamic_cast<CD3DStaticTerrainMaterial*>(in_pResource))
			{
				pMaterial->Unload();
			}
		}
		return;
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

	LogMessage("%d Objects created.", _vecTerrainObjects.size());
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
	CD3DStaticTerrainObject* pObject = new CD3DStaticTerrainObject(this);

	CD3DStaticTerrainMaterial* pMaterial = new CD3DStaticTerrainMaterial(this, in_pData->GetTextureFileName());

	CD3DStaticTerrainFaceset* pFaceset = new CD3DStaticTerrainFaceset(this, pObject, pMaterial, in_pData);

	pObject->SetFaceset(pFaceset);

	_mapTerrainDataBlocks[pObject] = in_pData;

	_lstMaterials.push_back(pMaterial);
	_lstFacesets.push_back(pFaceset);
	_vecTerrainObjects.push_back(pObject);
}

size_t CD3DStaticTerrainRenderer::GetObjectsCount() const
{
	return _vecTerrainObjects.size();
}

C3DBaseObject*	CD3DStaticTerrainRenderer::GetObjectByIndex(size_t id) const
{
	return _vecTerrainObjects[id];
}

float CD3DStaticTerrainRenderer::GetWorldRadius() const
{
	return 20000000.f;
}

float CD3DStaticTerrainRenderer::GetMinCellSize() const
{
	return 1000.f;
}

void CD3DStaticTerrainRenderer::OutputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
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

bool CD3DStaticTerrainRenderer::InitializeShader(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename)
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

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "BINORMAL";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
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

void CD3DStaticTerrainRenderer::FinalizeShader()
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

bool CD3DStaticTerrainRenderer::SetShaderParameters(CD3DGraphicsContext* in_pContext, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
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

	vm::Vector3df p = in_pContext->GetScene()->GetMainCamera()->GetPos();
	dataPtr->vCamPos = vm::Vector4df(p[0], p[1], p[2], 1.0);

	vm::Vector3df d = in_pContext->GetScene()->GetMainCamera()->GetDir();
	dataPtr->vAxisX = vm::Vector4df(p[0], p[1], p[2], 1.0);

	vm::Vector3df u = in_pContext->GetScene()->GetMainCamera()->GetUp();
	dataPtr->vAxisZ = vm::Vector4df(u[0], u[1], u[2], 1.0);

	vm::Vector3df l = vm::cross(u, d);
	dataPtr->vAxisY = vm::Vector4df(l[0], l[1], l[2], 1.0);

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


void CD3DStaticTerrainRenderer::DrawIndexedByShader(ID3D11DeviceContext* deviceContext, ID3D11ShaderResourceView* texture, unsigned int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(_pInputLayout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(_pVertexShader, NULL, 0);
	deviceContext->PSSetShader(_pPixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &_pSampleState);

	// Set shader texture resource in the pixel shader.

	ID3D11ShaderResourceView* resourceViews[1] = { texture };
	deviceContext->PSSetShaderResources(0, 1, resourceViews);


	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}