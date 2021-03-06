#include "HeightfieldConverterPrivate.h"

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

// �������������
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


void HeightfieldConverter::HeightfieldConverterPrivate::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, ID3DX11Effect* in_pEffect)
{
	_ptrD3DDevice = in_pD3DDevice11;
	_ptrDeviceContext = in_pDeviceContext;

	_pAbstractConverter = new DirectComputeHeightfieldConverter(in_pD3DDevice11, in_pDeviceContext, in_pEffect, this);
}

// ������� ������������ ���������� � ��������� ����������
void HeightfieldConverter::HeightfieldConverterPrivate::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, STriangulation* out_pTriangulation, const SHeightfield** in_ppNeighbours)
{
	if (_pAbstractConverter)
		_pAbstractConverter->CreateTriangulationImmediate(in_pHeightfield, in_fLongitudeCutCoeff, in_fLattitudeCutCoeff, out_pTriangulation, in_ppNeighbours);
}

// �������� ������ �� ������������, ������� ����� ����������� ���������� � ������� DirectCompute
void HeightfieldConverter::HeightfieldConverterPrivate::AppendTriangulationTask(const SHeightfield* in_pHeightfield, float in_fLongitudeCutCoeff, float in_fLattitudeCutCoeff, void* param, TriangulationTaskCompleteCallback in_Callback)
{
	if (_pAbstractConverter)
		_pAbstractConverter->AppendTriangulationTask(in_pHeightfield, in_fLongitudeCutCoeff, in_fLattitudeCutCoeff, param, in_Callback);
}

void HeightfieldConverter::HeightfieldConverterPrivate::UpdateTasks()
{
	if (_pAbstractConverter)
		_pAbstractConverter->UpdateTasks();
}

// ������ ���������� ����������� ��������.
// �� ��������� ��� ������� �������� � �������� � ���������� ����� � ������� ��������� WGS-84 � ������
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

void HeightfieldConverter::HeightfieldConverterPrivate::SetNormalDivisionAngles(float in_fAngle1InDeg, float in_fAngle2InDeg)
{
	_fNormalDivisionAngle1 = in_fAngle1InDeg;
	_fNormalDivisionAngle2 = in_fAngle2InDeg;

	if (_fNormalDivisionAngle2 < _fNormalDivisionAngle1)
		_fNormalDivisionAngle2 = _fNormalDivisionAngle1;
}

void HeightfieldConverter::HeightfieldConverterPrivate::ComputeTriangulationCoords(const SHeightfield::SCoordinates& in_Coords, STriangulationCoordsInfo& out_TriangulationCoords)
{
	if (_pAbstractConverter)
		_pAbstractConverter->ComputeTriangulationCoords(in_Coords, out_TriangulationCoords);
}

// ������� ������ ����� ����� �� ��������
void HeightfieldConverter::HeightfieldConverterPrivate::ReadHeightfieldDataFromTexture(const wchar_t* in_pcwszTextureFileName, SHeightfield& out_Heightfield, unsigned short in_usCompressionRatio)
{
	// Load the texture in.
	HRESULT result = D3DX11CreateShaderResourceViewFromFileW(_ptrD3DDevice, in_pcwszTextureFileName, NULL, NULL, &out_Heightfield.pTextureSRV, NULL);

	if (FAILED(result))
	{
		LogMessage("HeightfieldConverter:ReadHeightfieldDataFromTexture error D3DX11CreateShaderResourceViewFromFileW");
		return;
	}

	/*ID3D11Resource* resource = NULL;
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
		
		out_Heightfield.Config.nCountX = description.Width / in_usCompressionRatio;
		out_Heightfield.Config.nCountY = description.Height / in_usCompressionRatio;
	}*/
}


// ���������� ������ ������������
void HeightfieldConverter::HeightfieldConverterPrivate::ReleaseTriangulation(STriangulation* triangulation)
{
	ULONG lref = 0;

	LockDeviceContext();

	if (triangulation->pIndexBuffer)
		lref = triangulation->pIndexBuffer->Release();

	if (triangulation->pVertexBuffer)
		lref = triangulation->pVertexBuffer->Release();

	UnlockDeviceContext();

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
		pd3dImmediateContext->CopyResource(debugbuf, pBuffer);
	}

	return debugbuf;
}


// �������� ������ ������ � �������� � ������
bool HeightfieldConverter::HeightfieldConverterPrivate::UnmapTriangulation(STriangulation* triangulation, SVertex* out_pVertexes, unsigned int* out_pIndices)
{
	LockDeviceContext();

	// Read back the result from GPU, verify its correctness against result computed by CPU 
	if (triangulation->pVertexBuffer && out_pVertexes)
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(_ptrD3DDevice, _ptrDeviceContext, triangulation->pVertexBuffer);

		if (!debugbuf)
		{
			UnlockDeviceContext();
			return false;
		}

		D3D11_MAPPED_SUBRESOURCE MappedResource;

		SVertex *p;
		if (_ptrDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource) == S_OK)
		{
			// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS 
			// This is also a common trick to debug CS programs. 
			p = (SVertex*)MappedResource.pData;

			memcpy(out_pVertexes, p, triangulation->nVertexCount * sizeof(SVertex));

			_ptrDeviceContext->Unmap(debugbuf, 0);
		}
		else
		{
			UnlockDeviceContext();
			return false;
		}

		if (debugbuf)
			debugbuf->Release();
	}

	// Read back the result from GPU, verify its correctness against result computed by CPU 
	if (triangulation->pIndexBuffer && out_pIndices)
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(_ptrD3DDevice, _ptrDeviceContext, triangulation->pIndexBuffer);
		D3D11_MAPPED_SUBRESOURCE MappedResource;

		if (!debugbuf)
		{
			UnlockDeviceContext();
			return false;
		}

		unsigned int *p;
		if (_ptrDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource) == S_OK)
		{
			// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS 
			// This is also a common trick to debug CS programs. 
			p = (unsigned int*)MappedResource.pData;

			memcpy(out_pIndices, p, triangulation->nIndexCount * 4);

			_ptrDeviceContext->Unmap(debugbuf, 0);
		}
		else
		{
			UnlockDeviceContext();
			return false;
		}

		if (debugbuf)
			debugbuf->Release();
	}

	UnlockDeviceContext();

	return true;
}

void HeightfieldConverter::HeightfieldConverterPrivate::LockDeviceContext()
{
	_contextMutex.lock();
}

void HeightfieldConverter::HeightfieldConverterPrivate::UnlockDeviceContext()
{
	_contextMutex.unlock();
}
