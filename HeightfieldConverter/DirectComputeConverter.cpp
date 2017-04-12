#include "DirectComputeConverter.h"

#include <d3dcompiler.h>

#include <chrono>
#include "vecmath.h"

using namespace std::chrono_literals;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

HRESULT CreateBufferSRV(_In_ ID3D11Device* pDevice, _In_ ID3D11Buffer* pBuffer, _Outptr_ ID3D11ShaderResourceView** ppSRVOut);
HRESULT CreateBufferUAV(_In_ ID3D11Device* pDevice, _In_ ID3D11Buffer* pBuffer, _Outptr_ ID3D11UnorderedAccessView** pUAVOut);


//-------------------------------------------------------------------------------------- 
// Create Shader Resource View for Structured or Raw Buffers 
//-------------------------------------------------------------------------------------- 
_Use_decl_annotations_
HRESULT CreateBufferSRV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer 

		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.BufferEx.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer 

			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}
	}

	return pDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

//-------------------------------------------------------------------------------------- 
// Create Unordered Access View for Structured or Raw Buffers 
//--------------------------------------------------------------------------------------  
_Use_decl_annotations_
HRESULT CreateBufferUAV(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS)
	{
		// This is a Raw Buffer 

		desc.Format = DXGI_FORMAT_R32_TYPELESS; // Format must be DXGI_FORMAT_R32_TYPELESS, when creating Raw Unordered Access View 
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
		desc.Buffer.NumElements = descBuf.ByteWidth / 4;
	}
	else
	{
		if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED)
		{
			// This is a Structured Buffer 

			desc.Format = DXGI_FORMAT_UNKNOWN;      // Format must be must be DXGI_FORMAT_UNKNOWN, when creating a View of a Structured Buffer 
			desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
		}
		else
		{
			return E_INVALIDARG;
		}
	}

	return pDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

void RunComputeShader(_In_ ID3D11DeviceContext* pd3dImmediateContext,
	_In_ ID3D11ComputeShader* pComputeShader,
	_In_ UINT nNumViews, _In_reads_(nNumViews) ID3D11ShaderResourceView** pShaderResourceViews,
	_In_opt_ ID3D11Buffer* pCBCS, _In_reads_opt_(dwNumDataBytes) void* pCSData, _In_ DWORD dwNumDataBytes,
	_In_ UINT nNumUAVs, _In_ ID3D11UnorderedAccessView** ppUnorderedAccessViews,
	_In_ UINT X, _In_ UINT Y, _In_ UINT Z);


// ‘ункци€ компил€ции compute shader
HRESULT CompileComputeShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ ID3D11Device* device, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !device || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	// We generally prefer to use the higher CS shader profile when possible as CS 5.0 is better performance on 11-class hardware
	LPCSTR profile = (device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : "cs_4_0";

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, profile, flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}


//-------------------------------------------------------------------------------------- 
// Run CS 
//--------------------------------------------------------------------------------------  
_Use_decl_annotations_
void RunComputeShader(ID3D11DeviceContext* pd3dImmediateContext,
	ID3D11ComputeShader* pComputeShader,
	UINT nNumViews, ID3D11ShaderResourceView** ppShaderResourceViews,
	ID3D11Buffer* pCBCS, void* pCSData, DWORD dwNumDataBytes,
	UINT nNumUAVs, ID3D11UnorderedAccessView** ppUnorderedAccessViews,
	UINT X, UINT Y, UINT Z)
{
	pd3dImmediateContext->CSSetShader(pComputeShader, nullptr, 0);
	pd3dImmediateContext->CSSetShaderResources(0, nNumViews, ppShaderResourceViews);
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, nNumUAVs, ppUnorderedAccessViews, nullptr);
	if (pCBCS && pCSData)
	{
		D3D11_MAPPED_SUBRESOURCE MappedResource;
		pd3dImmediateContext->Map(pCBCS, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		memcpy(MappedResource.pData, pCSData, dwNumDataBytes);
		pd3dImmediateContext->Unmap(pCBCS, 0);
		ID3D11Buffer* ppCB[1] = { pCBCS };
		pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCB);
	}


	pd3dImmediateContext->Dispatch(X, Y, Z);

	pd3dImmediateContext->CSSetShader(nullptr, nullptr, 0);

	ID3D11UnorderedAccessView* ppUAViewnullptr[2] = { nullptr, nullptr };
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 2, ppUAViewnullptr, nullptr);

	ID3D11ShaderResourceView* ppSRVnullptr[2] = { nullptr, nullptr };
	pd3dImmediateContext->CSSetShaderResources(0, 2, ppSRVnullptr);

	ID3D11Buffer* ppCBnullptr[1] = { nullptr };
	pd3dImmediateContext->CSSetConstantBuffers(0, 1, ppCBnullptr);
}


//-------------------------------------------------------------------------------------- 
// Create Raw Buffer 
//-------------------------------------------------------------------------------------- 
_Use_decl_annotations_
HRESULT CreateRawBuffer(ID3D11Device* pDevice, UINT uSize, void* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = uSize;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, nullptr, ppBufOut);
}

//-------------------------------------------------------------------------------------- 
// Create Structured Buffer 
//-------------------------------------------------------------------------------------- 
_Use_decl_annotations_
HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut)
{
	*ppBufOut = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;

	if (pInitData)
	{
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return pDevice->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else
		return pDevice->CreateBuffer(&desc, nullptr, ppBufOut);
}

DirectComputeHeightfieldConverter::DirectComputeHeightfieldConverter(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, HeightfieldConverter::HeightfieldConverterPrivate* in_pOwner)
{
	_owner = in_pOwner;

	_ptrD3DDevice = in_pD3DDevice11;
	_ptrDeviceContext = in_pDeviceContext;

	// Verify compute shader is supported
	if (_ptrD3DDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts = { 0 };
		_ptrD3DDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));

		if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
		{
			//printf("DirectCompute is not supported by this device\n");
			// TODO: log here
			return;
		}
	}


	// Compile shader
	ID3DBlob *csBlob = nullptr;
	HRESULT hr = CompileComputeShader(L"ComputeShaders\\HeightfieldConverter.hlsl", "CSMain", _ptrD3DDevice, &csBlob);
	if (FAILED(hr))
	{
		// TODO: log error here
		//printf("Failed compiling shader %08X\n", hr);
		return;
	}

	// Create shader
	hr = _ptrD3DDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &_ptrComputeShader);
	if (FAILED(hr))
	{
		// TODO: log error here
		return;
	}

}

DirectComputeHeightfieldConverter::~DirectComputeHeightfieldConverter()
{

}

void DirectComputeHeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	STriangulationTask task(this, *in_pHeightfield);
	task.createTriangulation();
	*out_pTriangulation = task._triangulation;
}

void DirectComputeHeightfieldConverter::RegisterListener(HeightfieldConverterListener* in_pListener)
{
	_setListeners.insert(in_pListener);
}

void DirectComputeHeightfieldConverter::UnregisterListener(HeightfieldConverterListener* in_pListener)
{
	_setListeners.erase(in_pListener);
}

void DirectComputeHeightfieldConverter::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
	std::lock_guard<std::mutex> lock(_tasksMutex);
	_qTriangulationTasks.push(new STriangulationTask(this, *in_pHeightfield));
}

// обработать поставленные задачи
void DirectComputeHeightfieldConverter::UpdateTasks()
{
	std::lock_guard<std::mutex> lock(_tasksMutex);

	while (!_qTriangulationTasks.empty())
	{
		STriangulationTask* task = _qTriangulationTasks.front();
		_qTriangulationTasks.pop();

		task->createTriangulation();

		for (HeightfieldConverterListener* listener : _setListeners)
			listener->TriangulationCreated(&task->_triangulation);

		delete task;
	}
}

void DirectComputeHeightfieldConverter::STriangulationTask::createTriangulation()
{
	_triangulation.ID = _heightfield.ID;

	_triangulation.nVertexCount = _heightfield.Config.nCountX * _heightfield.Config.nCountY;
	_triangulation.nIndexCount = (_heightfield.Config.nCountX - 1) * (_heightfield.Config.nCountY - 1) * 2 * 3;

	computeBasis();

	createOutputBuffers();
	createInputBuffers();

	ID3D11ShaderResourceView* aRViews[1] = { _heightfield.pTextureSRV };
	ID3D11UnorderedAccessView* aUAViews[2] = { _ptrVertexBufferUAV, _ptrIndexBufferUAV };

	ConstantBufferData constantData;

	constantData.Config = _heightfield.Config;
	constantData.fWorldScale = _owner->_owner->GetWorldScale();

	memcpy(constantData.vCenter, _triangulation.vPosition, 3 * sizeof(double));
	memcpy(constantData.vXAxis, _triangulation.vXAxis, 3 * sizeof(double));
	memcpy(constantData.vYAxis, _triangulation.vYAxis, 3 * sizeof(double));
	memcpy(constantData.vZAxis, _triangulation.vZAxis, 3 * sizeof(double));

	RunComputeShader(_owner->_ptrDeviceContext, _owner->_ptrComputeShader, 1, aRViews, _ptrConstantBuffer, &constantData, sizeof(ConstantBufferData), 2, aUAViews,
		_heightfield.Config.nCountX, 
		_heightfield.Config.nCountY,
		1);
}

vm::Vector3df GetWGS84SurfacePoint(double longitude, double lattitude)
{
	const double Rmin = 6356752.3142;
	const double Rmax = 6378137;

	double cosB = cos(lattitude);
	double sinB = sin(lattitude);

	double cosA = cos(longitude);
	double sinA = sin(longitude);

	double R = sqrt( Rmax*Rmax*Rmin*Rmin / (Rmin*Rmin*cosB*cosB + Rmax*Rmax*sinB*sinB) );

	return vm::Vector3df(
			R*cosA*cosB,
			R*sinA*cosB,
			R*sinB
		);
}

vm::Vector3df GetWGS84SurfaceNormal(const vm::Vector3df& in_vSurfacePoint)
{
	const double Rmin = 6356752.3142;
	const double Rmax = 6378137;

	vm::Vector3df vUnnormalizedNormal = vm::Vector3df(
			2 * in_vSurfacePoint[0] / (Rmax*Rmax),
			2 * in_vSurfacePoint[1] / (Rmax*Rmax),
			2 * in_vSurfacePoint[2] / (Rmin*Rmin)
		);

	return normalize(vUnnormalizedNormal);
}

vm::Vector3df GetWGS84SurfaceNormal(double longitude, double lattitude)
{
	return GetWGS84SurfaceNormal(GetWGS84SurfacePoint(longitude, lattitude));
}

void DirectComputeHeightfieldConverter::STriangulationTask::computeBasis()
{
	double middleLattitude = (_heightfield.Config.fMinLattitude + _heightfield.Config.fMaxLattitude)*0.5;
	double middleLongitude = (_heightfield.Config.fMinLongitude + _heightfield.Config.fMaxLongitude)*0.5;

	vm::Vector3df vMiddlePoint = GetWGS84SurfacePoint(middleLongitude, middleLattitude);
	vm::Vector3df vNormal = GetWGS84SurfaceNormal(vMiddlePoint);
	vm::Vector3df vEast = vm::normalize(vm::cross(vNormal, vm::Vector3df(0, 0, 1)));
	vm::Vector3df vNorth = vm::normalize(vm::cross(vNormal, vEast));

	memcpy(_triangulation.vPosition, &vMiddlePoint[0], 3*sizeof(double));
	memcpy(_triangulation.vXAxis, &vNorth[0], 3 * sizeof(double));
	memcpy(_triangulation.vYAxis, &vNormal[0], 3 * sizeof(double));
	memcpy(_triangulation.vZAxis, &vEast[0], 3 * sizeof(double));

	vm::BoundBox<double> vBoundBox(vMiddlePoint);

	vBoundBox.update(vMiddlePoint + vNormal*_heightfield.Config.fMinHeight);
	vBoundBox.update(vMiddlePoint + vNormal*_heightfield.Config.fMaxHeight);

	vBoundBox.update(
		GetWGS84SurfacePoint(_heightfield.Config.fMinLongitude, _heightfield.Config.fMinLattitude) 
		+ GetWGS84SurfaceNormal(_heightfield.Config.fMinLongitude, _heightfield.Config.fMinLattitude)*_heightfield.Config.fMinHeight);
	vBoundBox.update(
		GetWGS84SurfacePoint(_heightfield.Config.fMinLongitude, _heightfield.Config.fMinLattitude) 
		+ GetWGS84SurfaceNormal(_heightfield.Config.fMinLongitude, _heightfield.Config.fMinLattitude)*_heightfield.Config.fMaxHeight);

	vBoundBox.update(
		GetWGS84SurfacePoint(_heightfield.Config.fMaxLongitude, _heightfield.Config.fMinLattitude) 
		+ GetWGS84SurfaceNormal(_heightfield.Config.fMaxLongitude, _heightfield.Config.fMinLattitude)*_heightfield.Config.fMinHeight);
	vBoundBox.update(GetWGS84SurfacePoint(_heightfield.Config.fMaxLongitude, _heightfield.Config.fMinLattitude) 
		+ GetWGS84SurfaceNormal(_heightfield.Config.fMaxLongitude, _heightfield.Config.fMinLattitude)*_heightfield.Config.fMaxHeight);

	vBoundBox.update(
		GetWGS84SurfacePoint(_heightfield.Config.fMinLongitude, _heightfield.Config.fMaxLattitude) 
		+ GetWGS84SurfaceNormal(_heightfield.Config.fMinLongitude, _heightfield.Config.fMaxLattitude)*_heightfield.Config.fMinHeight);
	vBoundBox.update(
		GetWGS84SurfacePoint(_heightfield.Config.fMinLongitude, _heightfield.Config.fMaxLattitude) 
		+ GetWGS84SurfaceNormal(_heightfield.Config.fMinLongitude, _heightfield.Config.fMaxLattitude)*_heightfield.Config.fMaxHeight);

	vBoundBox.update(GetWGS84SurfacePoint(_heightfield.Config.fMaxLongitude, _heightfield.Config.fMaxLattitude) 
		+ GetWGS84SurfaceNormal(_heightfield.Config.fMaxLongitude, _heightfield.Config.fMaxLattitude) *_heightfield.Config.fMinHeight);
	vBoundBox.update(GetWGS84SurfacePoint(_heightfield.Config.fMaxLongitude, _heightfield.Config.fMaxLattitude) 
		+ GetWGS84SurfaceNormal(_heightfield.Config.fMaxLongitude, _heightfield.Config.fMaxLattitude)*_heightfield.Config.fMaxHeight);

	memcpy(_triangulation.vBoundBoxMinimum, &vBoundBox._vMin[0], 3 * sizeof(double));
	memcpy(_triangulation.vBoundBoxMaximum, &vBoundBox._vMax[0], 3 * sizeof(double));

	for (unsigned int i = 0; i < 3; i++)
	{
		_triangulation.vPosition[i] *= _owner->_owner->GetWorldScale();
		_triangulation.vBoundBoxMinimum[i] *= _owner->_owner->GetWorldScale();
		_triangulation.vBoundBoxMaximum[i] *= _owner->_owner->GetWorldScale();
	}
}

void DirectComputeHeightfieldConverter::STriangulationTask::createInputBuffers()
{
	D3D11_BUFFER_DESC constantBufferDesc;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(ConstantBufferData);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	ID3D11Buffer* pBuf = nullptr;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT hr = _owner->_ptrD3DDevice->CreateBuffer(&constantBufferDesc, NULL, &pBuf);
	if (FAILED(hr))
	{
		//TODO: log
		return;
	}

	_ptrConstantBuffer = pBuf;
}

void DirectComputeHeightfieldConverter::STriangulationTask::createOutputBuffers()
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;

	HRESULT result;

	if (_triangulation.nIndexCount == 0)
		return;

	if (_triangulation.nVertexCount == 0)
		return;


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SVertex) * _triangulation.nVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	vertexBufferDesc.StructureByteStride = 0;

	// Now create the vertex buffer.
	result = _owner->_ptrD3DDevice->CreateBuffer(&vertexBufferDesc, nullptr, &_triangulation.pVertexBuffer);
	if (FAILED(result))
	{
		return;
	}


	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _triangulation.nIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	indexBufferDesc.StructureByteStride = 0;

	// Create the index buffer.
	result = _owner->_ptrD3DDevice->CreateBuffer(&indexBufferDesc, nullptr, &_triangulation.pIndexBuffer);
	if (FAILED(result))
	{
		return;
	}

	CreateBufferUAV(_owner->_ptrD3DDevice, _triangulation.pVertexBuffer, &_ptrVertexBufferUAV);
	CreateBufferUAV(_owner->_ptrD3DDevice, _triangulation.pIndexBuffer, &_ptrIndexBufferUAV);
}