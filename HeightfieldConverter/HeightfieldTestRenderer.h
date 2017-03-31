#pragma once

#include <d3d11.h>
#include <atlbase.h>
#include <atlwin.h>

class HeightfieldTestRenderer
{
public:

	HeightfieldTestRenderer(ID3D11Device* in_pD3DDevice11, ID3D11DeviceContext* in_pDeviceContext);


private:

	CComPtr<ID3D11Device>           m_device;
	CComPtr<ID3D11DeviceContext>    m_deviceContext;


};