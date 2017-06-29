#include "HeightfieldConverterPrivate.h"

#include "SoftwareConverter.h"
#include "DirectComputeConverter.h"

#include "Log.h"

#include <d3dx11tex.h>

#pragma comment(lib, "d3dx11.lib")

#define USE_ENGINE_SCALE

#ifdef USE_ENGINE_SCALE
const float g_fMasterScale = 100.f;
#else
const float g_fMasterScale = 1.f;
#endif

HeightfieldConverter::HeightfieldConverterPrivate::~HeightfieldConverterPrivate()
{
	if (_pAbstractConverter)
		delete _pAbstractConverter;
}

// инициализация
void HeightfieldConverter::HeightfieldConverterPrivate::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, const wchar_t* in_pcszComputeShaderFile)
{
	_ptrD3DDevice = in_pD3DDevice11;
	_ptrDeviceContext = in_pDeviceContext;

	//switch (in_Mode)
	{
	//case SOFTWARE_MODE:
	//	_pAbstractConverter = new SoftwareHeightfieldConverter(in_pD3DDevice11);
	//	break;

	//case DIRECT_COMPUTE_MODE:
		_pAbstractConverter = new DirectComputeHeightfieldConverter(in_pD3DDevice11, in_pDeviceContext, in_pcszComputeShaderFile, this);
//		break;
	}

}

// Создать триангуляцию немедленно и дождаться готовности
void HeightfieldConverter::HeightfieldConverterPrivate::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	if (_pAbstractConverter)
		_pAbstractConverter->CreateTriangulationImmediate(in_pHeightfield, out_pTriangulation);
}

// добавить/удалить listener
void HeightfieldConverter::HeightfieldConverterPrivate::RegisterListener(HeightfieldConverterListener* listener)
{
	if (_pAbstractConverter)
		_pAbstractConverter->RegisterListener(listener);
}

void HeightfieldConverter::HeightfieldConverterPrivate::UnregisterListener(HeightfieldConverterListener* listener)
{
	if (_pAbstractConverter)
		_pAbstractConverter->UnregisterListener(listener);
}

// добавить задачу на триангуляцию, которая будет выполняться асинхронно с помощью DirectCompute
void HeightfieldConverter::HeightfieldConverterPrivate::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
	if (_pAbstractConverter)
		_pAbstractConverter->AppendTriangulationTask(in_pHeightfield);
}

void HeightfieldConverter::HeightfieldConverterPrivate::UpdateTasks()
{
	if (_pAbstractConverter)
		_pAbstractConverter->UpdateTasks();
}

// Задать глобальный коэффициент масштаба.
// По умолчанию все расчеты ведуться в привязке к эллипсоиду Земли в системе координат WGS-84 в метрах
void HeightfieldConverter::HeightfieldConverterPrivate::SetWorldScale(float in_fScale)
{
	_fScale = in_fScale * g_fMasterScale;
}

float HeightfieldConverter::HeightfieldConverterPrivate::GetWorldScale() const
{
	return _fScale;
}

void HeightfieldConverter::HeightfieldConverterPrivate::SetHeightScale(float in_fHeightScale)
{
	_fHeightScale = in_fHeightScale ;
}

void HeightfieldConverter::HeightfieldConverterPrivate::ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords)
{
	if (_pAbstractConverter)
		_pAbstractConverter->ComputeTriangulationCoords(in_Coords, out_TriangulationCoords);
}

// Считать данные карты высот из текстуры
void HeightfieldConverter::HeightfieldConverterPrivate::ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield)
{
	// Load the texture in.
	HRESULT result = D3DX11CreateShaderResourceViewFromFileW(_ptrD3DDevice, in_pcwszTextureFileName, NULL, NULL, &out_Heightfield.pTextureSRV, NULL);
	if (FAILED(result))
	{
		LogMessage("HeightfieldConverter:ReadHeightfieldDataFromTexture error D3DX11CreateShaderResourceViewFromFileW");
		return;
	}

	ID3D11Resource* resource = NULL;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	if (out_Heightfield.pTextureSRV)
	{
		out_Heightfield.pTextureSRV->GetResource(&resource);
		out_Heightfield.pTextureSRV->GetDesc(&srvDesc);
	}
	else
		return;

	ID3D11Texture2D* tex = (ID3D11Texture2D*)resource;
	if (tex)
	{
		D3D11_TEXTURE2D_DESC description;
		tex->GetDesc(&description);
		
		out_Heightfield.Config.nCountX = description.Width;
		out_Heightfield.Config.nCountY = description.Height;
	}
}


// Освободить буферы триангуляции
void HeightfieldConverter::HeightfieldConverterPrivate::ReleaseTriangulation(STriangulation* triangulation)
{
	ULONG lref = 0;
	if (triangulation->pIndexBuffer)
		lref = triangulation->pIndexBuffer->Release();

	if (triangulation->pVertexBuffer)
		lref = triangulation->pVertexBuffer->Release();

	triangulation->pIndexBuffer = nullptr;
	triangulation->pVertexBuffer = nullptr;

	triangulation->nIndexCount = 0;
	triangulation->nVertexCount = 0;
}

void HeightfieldConverter::HeightfieldConverterPrivate::ReleaseHeightfield(SHeightfield* heightfield)
{
	if (heightfield->pTextureSRV)
		heightfield->pTextureSRV->Release();

	heightfield->pTextureSRV = nullptr;

	heightfield->Config.nCountX = 0;
	heightfield->Config.nCountY = 0;
}


//-------------------------------------------------------------------------------------- 
// Create a CPU accessible buffer and download the content of a GPU buffer into it 
// This function is very useful for debugging CS programs 
//--------------------------------------------------------------------------------------  
_Use_decl_annotations_
ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(pDevice->CreateBuffer(&desc, nullptr, &debugbuf)))
	{
		//#if defined(_DEBUG) || defined(PROFILE) 
		//debugbuf->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Debug") - 1, "Debug");
		//#endif 

		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
}


// Получить буферы вершин и индексов в памяти
void HeightfieldConverter::HeightfieldConverterPrivate::UnmapTriangulation(STriangulation* triangulation, SVertex* out_pVertexes, unsigned int* out_pIndices)
{
	// Read back the result from GPU, verify its correctness against result computed by CPU 
	if (triangulation->pVertexBuffer)
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(_ptrD3DDevice, _ptrDeviceContext, triangulation->pVertexBuffer);
		D3D11_MAPPED_SUBRESOURCE MappedResource;

		SVertex *p;
		_ptrDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

		// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS 
		// This is also a common trick to debug CS programs. 
		p = (SVertex*)MappedResource.pData;

		memcpy(out_pVertexes, p, triangulation->nVertexCount * sizeof(SVertex));

		_ptrDeviceContext->Unmap(debugbuf, 0);

		if (debugbuf)
			debugbuf->Release();
	}

	// Read back the result from GPU, verify its correctness against result computed by CPU 
	if (triangulation->pIndexBuffer)
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(_ptrD3DDevice, _ptrDeviceContext, triangulation->pIndexBuffer);
		D3D11_MAPPED_SUBRESOURCE MappedResource;

		unsigned int *p;
		_ptrDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

		// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS 
		// This is also a common trick to debug CS programs. 
		p = (unsigned int*)MappedResource.pData;

		memcpy(out_pIndices, p, triangulation->nIndexCount * 4);

		_ptrDeviceContext->Unmap(debugbuf, 0);

		if (debugbuf)
			debugbuf->Release();
	}
}
