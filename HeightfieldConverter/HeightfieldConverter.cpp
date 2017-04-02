#include "HeightfieldConverter.h"
#include "HeightfieldConverterPrivate.h"

HeightfieldConverter::HeightfieldConverter()
{
	_private = new HeightfieldConverterPrivate;
}

HeightfieldConverter::~HeightfieldConverter()
{
	delete _private;
}

// инициализация
void HeightfieldConverter::Init(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext, EHeightfieldConverterMode in_Mode)
{
	_private->Init(in_pD3DDevice11, in_pDeviceContext, in_Mode);
}

// Создать триангуляцию немедленно и дождаться готовности
void HeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	_private->CreateTriangulationImmediate(in_pHeightfield, out_pTriangulation);
}

// добавить/удалить listener
void HeightfieldConverter::RegisterListener(HeightfieldConverterListener* in_pListener)
{
	_private->RegisterListener(in_pListener);
}

void HeightfieldConverter::UnregisterListener(HeightfieldConverterListener* in_pListener)
{
	_private->UnregisterListener(in_pListener);
}

// добавить задачу на триангуляцию, которая будет выполняться асинхронно с помощью DirectCompute
void HeightfieldConverter::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
	_private->AppendTriangulationTask(in_pHeightfield);
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

void STriangulation::UnmapBuffers(ID3D11Device* in_pD3D11Device, ID3D11DeviceContext* in_pDeviceContext, SVertex* out_pVertexes, unsigned int* out_pIndices)
{
	// Read back the result from GPU, verify its correctness against result computed by CPU 
	if (pVertexBuffer)
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(in_pD3D11Device, in_pDeviceContext, pVertexBuffer);
		D3D11_MAPPED_SUBRESOURCE MappedResource;

		SVertex *p;
		in_pDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

		// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS 
		// This is also a common trick to debug CS programs. 
		p = (SVertex*) MappedResource.pData;

		memcpy(out_pVertexes, p, nVertexCount * sizeof(SVertex));

		in_pDeviceContext->Unmap(debugbuf, 0);

		if (debugbuf)
			debugbuf->Release();
	}

	// Read back the result from GPU, verify its correctness against result computed by CPU 
	if (pIndexBuffer)
	{
		ID3D11Buffer* debugbuf = CreateAndCopyToDebugBuf(in_pD3D11Device, in_pDeviceContext, pIndexBuffer);
		D3D11_MAPPED_SUBRESOURCE MappedResource;

		unsigned int *p;
		in_pDeviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &MappedResource);

		// Set a break point here and put down the expression "p, 1024" in your watch window to see what has been written out by our CS 
		// This is also a common trick to debug CS programs. 
		p = (unsigned int*) MappedResource.pData;

		memcpy(out_pIndices, p, nIndexCount * 4);

		in_pDeviceContext->Unmap(debugbuf, 0);

		if (debugbuf)
			debugbuf->Release();
	}
}

void STriangulation::ReleaseBuffers()
{
	ULONG lref = 0;
	if (pIndexBuffer)
		lref = pIndexBuffer->Release();

	if (pVertexBuffer)
		lref = pVertexBuffer->Release();

	pIndexBuffer = nullptr;
	pVertexBuffer = nullptr;
}
