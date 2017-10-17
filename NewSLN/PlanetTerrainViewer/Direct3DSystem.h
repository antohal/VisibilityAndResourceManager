#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")


//////////////
// INCLUDES //
//////////////
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d2d1.h>

#include <atlbase.h>

#include <string>

#include "CDirect2DTextRenderer.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: CDirect3DSystem
////////////////////////////////////////////////////////////////////////////////
class CDirect3DSystem
{
public:
	CDirect3DSystem();
	~CDirect3DSystem();

	bool								Initialize(unsigned int screenWidth, unsigned int screenHeight, bool vsync, HWND hwnd, bool fullscreen,
		float screenDepth, float screenNear);

	void								Shutdown();
	
	void								BeginScene(float, float, float, float);
	void								EndScene();

	void								CreateResources();
	void								ReleaseResources();

	void								SetWireframe(bool in_bWireframe);
	bool								IsWireframe() const { return _bWireframe; }

	ID3D11Device*						GetDevice();
	ID3D11DeviceContext*				GetDeviceContext();

	CDirect2DTextRenderer*				GetTextRenderer();

	void								GetVideoCardInfo(std::string& out_strDesc, unsigned int& out_uiMemory);

	D3DXMATRIX*							GetProjectionMatrix();
	const D3DXMATRIX*					GetProjectionMatrix() const;

private:

	bool								_bVsyncEnabled = false;
	unsigned int						_uiVideoCardMemory = 0;
	char								_pcszVideoCardDescription[128];

	bool								_bWireframe = false;
	
	CComPtr<IDXGISwapChain>				_ptrSwapChain;
	CComPtr<ID3D11Device>				_ptrDevice;
	CComPtr<ID3D11DeviceContext>		_ptrDeviceContext;
	CComPtr<ID3D11RenderTargetView>		_ptrRenderTargetView;
	CComPtr<ID3D11Texture2D>			_ptrDepthStencilBuffer;
	CComPtr<ID3D11DepthStencilState>	_ptrDepthStencilState;
	CComPtr<ID3D11DepthStencilView>		_ptrDepthStencilView;
	
	CComPtr<ID3D11RasterizerState>		_ptrRasterStateSolid;
	CComPtr<ID3D11RasterizerState>		_ptrRasterStateWire;

	CComPtr<ID2D1Factory>				_ptrD2dFactory;

	CDirect2DTextRenderer*				_pTextRenderer = nullptr;

	D3DXMATRIX							_mProjectionMatrix;
};
