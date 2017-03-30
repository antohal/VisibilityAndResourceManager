#include "DirectComputeConverter.h"

#include <d3dcompiler.h>

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

// Функция компиляции compute shader
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

DirectComputeHeightfieldConverter::DirectComputeHeightfieldConverter(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext)
{
	m_ptrD3DDevice = in_pD3DDevice11;

	// Verify compute shader is supported
	if (m_ptrD3DDevice->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwopts = { 0 };
		m_ptrD3DDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwopts, sizeof(hwopts));

		if (!hwopts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
		{
			//printf("DirectCompute is not supported by this device\n");
			// TODO: log here
			return;
		}
	}


	// Compile shader
	ID3DBlob *csBlob = nullptr;
	HRESULT hr = CompileComputeShader(L"ComputeShaders\\HeightfieldConverter.hlsl", "CSMain", m_ptrD3DDevice, &csBlob);
	if (FAILED(hr))
	{
		// TODO: log error here
		//printf("Failed compiling shader %08X\n", hr);
		return;
	}

	// Create shader
	hr = m_ptrD3DDevice->CreateComputeShader(csBlob->GetBufferPointer(), csBlob->GetBufferSize(), nullptr, &m_ptrComputeShader);
	if (FAILED(hr))
	{
		// TODO: log error here
		return;
	}
}

void DirectComputeHeightfieldConverter::CreateTriangulationImmediate(const SHeightfield* in_pHeightfield, STriangulation* out_pTriangulation)
{
	
}

void DirectComputeHeightfieldConverter::RegisterListener(HeightfieldConverterListener* in_pListener)
{
}

void DirectComputeHeightfieldConverter::UnregisterListener(HeightfieldConverterListener* in_pListener)
{
}

void DirectComputeHeightfieldConverter::AppendTriangulationTask(const SHeightfield* in_pHeightfield)
{
}
