#include "MonoTextRenderer.h"

CMonoTextRenderer::CMonoTextRenderer(ID2D1Factory* in_pD2DFactory, IDXGISwapChain* in_pDXGISwapChain)
	: _ptrD2DFactory(in_pD2DFactory), _ptrD2DSwapChain(in_pDXGISwapChain)
{
	auto result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown * *>(&_ptrDwriteFactory));
	if (FAILED(result))
	{
		//std::cout << "Failed to create DirectWrite Factory." << std::endl;
		//std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}
}

CMonoTextRenderer::~CMonoTextRenderer()
{
	if (_resourceCreated)
		ReleaseResources();

	_ptrD2DFactory.Release();
	_ptrD2DSwapChain.Release();
}

void CMonoTextRenderer::CreateResources()
{
	if (!_ptrD2DFactory || !_ptrD2DSwapChain)
	{
		//TODO: log error

		return;
	}

	CComPtr<IDXGISurface> backBufferSurface;

	HRESULT result = _ptrD2DSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferSurface));


	if (FAILED(result))
	{
		//std::cout << "Failed to get DXGI surface for back buffer." << std::endl;
		//std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}

	// Proper DPI support is very important. Most applications do stupid things like hard coding this, which is why you,
	// can't use proper DPI on most monitors in Windows yet.
	/*float dpiX;
	float dpiY;
	_ptrD2DFactory->GetDesktopDpi(&dpiX, &dpiY);*/

	// DXGI_FORMAT_UNKNOWN will cause it to use the same format as the back buffer (R8G8B8A8_UNORM)
	auto d2dRTProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)/*, dpiX, dpiY*/);

	// Wraps up our DXGI surface in a D2D render target.
	result = _ptrD2DFactory->CreateDxgiSurfaceRenderTarget(backBufferSurface, &d2dRTProps, &_ptrRenderTarget);
	if (FAILED(result))
	{
		//std::cout << "Failed to create D2D DXGI Render Target." << std::endl;
		//std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}

	_resourceCreated = true;
}

void CMonoTextRenderer::ReleaseResources()
{
	if (!_resourceCreated)
	{
		// TODO: log error
		return;
	}

	_ptrRenderTarget.Release();

	_resourceCreated = false;
}

void CMonoTextRenderer::CreateTextFormat(
	_In_z_ WCHAR const* fontFamilyName,
	_In_opt_ IDWriteFontCollection* fontCollection,
	DWRITE_FONT_WEIGHT fontWeight,
	DWRITE_FONT_STYLE fontStyle,
	DWRITE_FONT_STRETCH fontStretch,
	FLOAT fontSize,
	_In_z_ WCHAR const* localeName,
	CComPtr<IDWriteTextFormat>& out_ptrTextFormat)
{
	if (!_ptrDwriteFactory)
		return;

	_ptrDwriteFactory->CreateTextFormat(fontFamilyName, fontCollection, fontWeight, fontStyle, fontStretch, fontSize, localeName, &out_ptrTextFormat);
}

void CMonoTextRenderer::CreateSolidColorBrush(CONST D2D1_COLOR_F &color, CComPtr<ID2D1SolidColorBrush>& out_ptrSolidBrush)
{
	if (!_ptrRenderTarget)
	{
		//TODO: log
		return;
	}

	auto result = _ptrRenderTarget->CreateSolidColorBrush(color, &out_ptrSolidBrush);
	if (FAILED(result))
	{
		//std::cout << "Failed to create solid color brush." << std::endl;
		//std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}
}

void CMonoTextRenderer::RenderText(
	_In_reads_(stringLength) CONST WCHAR *string,
	UINT32 stringLength,
	_In_ IDWriteTextFormat *textFormat,
	_In_ CONST D2D1_RECT_F *layoutRect,
	_In_ ID2D1Brush *defaultForegroundBrush,
	D2D1_DRAW_TEXT_OPTIONS options/* = D2D1_DRAW_TEXT_OPTIONS_NONE*/,
	DWRITE_MEASURING_MODE measuringMode/* = DWRITE_MEASURING_MODE_NATURAL*/)
{
	if (!_ptrRenderTarget)
	{
		//TODO: log
		return;
	}

	_ptrRenderTarget->DrawText(string, stringLength, textFormat, layoutRect, defaultForegroundBrush, options, measuringMode);
}

void CMonoTextRenderer::FillRectangle(const D2D1_RECT_F &rect, ID2D1Brush  *brush)
{
	if (!_ptrRenderTarget)
	{
		//TODO: log
		return;
	}

	_ptrRenderTarget->FillRectangle(rect, brush);
}

void CMonoTextRenderer::DrawRectangle(const D2D1_RECT_F &rect, ID2D1Brush  *brush)
{
	if (!_ptrRenderTarget)
	{
		//TODO: log
		return;
	}

	_ptrRenderTarget->DrawRectangle(rect, brush);
}

void CMonoTextRenderer::BeginDraw(unsigned int target)
{
	if (target != 0)
		return;

	if (!_ptrRenderTarget)
	{
		//TODO: log
		return;
	}

	_ptrRenderTarget->BeginDraw();
}

void CMonoTextRenderer::EndDraw(unsigned int target)
{
	if (target != 0)
		return;

	if (!_ptrRenderTarget)
	{
		//TODO: log
		return;
	}

	HRESULT hr = _ptrRenderTarget->EndDraw();

	if (hr != S_OK)
	{
		// TODO: log here

	}
}
