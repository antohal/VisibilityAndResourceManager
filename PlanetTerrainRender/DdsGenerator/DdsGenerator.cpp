#include "DdsGenerator.h"
#include "D3DX11tex.h"

#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib,"d3d11.lib")

DDSGENERATOR_API bool SaveDataToDDS(ID3D11Device* in_pDevice, ID3D11DeviceContext* in_pDeviceContext, void* in_pData, unsigned int in_nWidth,
	unsigned int in_nHeight,  unsigned int in_nElementSize, unsigned int in_nDxgiFormat, const char* in_pcszFileName)
{
	ID3D11Texture2D* pTextureInMemory = NULL;

	const UINT uiWidth = 512;
	const UINT uiHeight = 512;

	D3D11_TEXTURE2D_DESC textureInCPUMemoryDesc;

	textureInCPUMemoryDesc.Width = uiWidth;
	textureInCPUMemoryDesc.Height = uiHeight;
	textureInCPUMemoryDesc.MipLevels = 1;
	textureInCPUMemoryDesc.ArraySize = 1;
	//textureInCPUMemoryDesc.Format = DXGI_FORMAT_R32_FLOAT;
	textureInCPUMemoryDesc.Format = (DXGI_FORMAT)in_nDxgiFormat;
	textureInCPUMemoryDesc.SampleDesc.Count = 1;
	textureInCPUMemoryDesc.SampleDesc.Quality = 0;
	textureInCPUMemoryDesc.Usage = D3D11_USAGE_DEFAULT;
	textureInCPUMemoryDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureInCPUMemoryDesc.CPUAccessFlags = 0;
	textureInCPUMemoryDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initialData;

	// назначаем данные

	initialData.pSysMem = in_pData;
	initialData.SysMemPitch = uiWidth * in_nElementSize;
	initialData.SysMemSlicePitch = uiHeight * uiWidth * in_nElementSize;


	HRESULT hr = in_pDevice->CreateTexture2D(&textureInCPUMemoryDesc, &initialData, &pTextureInMemory);

	if (hr == S_OK)
	{
		hr = D3DX11SaveTextureToFileA(in_pDeviceContext, pTextureInMemory, D3DX11_IFF_DDS, in_pcszFileName);

		// освобождаем текстуру.
		pTextureInMemory->Release();
		pTextureInMemory = NULL;

		return hr == S_OK;
	}

	return false;
}
