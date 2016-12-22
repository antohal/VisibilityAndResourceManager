#include "StereoTextRenderer.h"

#include "D2d1_1helper.h"

CStereoTextRenderer::CStereoTextRenderer(ID2D1Factory2* in_d2dFactory, ID2D1Device1* in_d2dDevice, ID2D1DeviceContext1* in_d2dContext, IDXGISwapChain1* in_dxgiSwapChain)
: _d2dFactory(in_d2dFactory)
, _d2dDevice(in_d2dDevice)
, _d2dContext(in_d2dContext)
, _swapChain(in_dxgiSwapChain)
{
	auto result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown * *>(&_ptrDwriteFactory));
	if (FAILED(result))
	{
		//std::cout << "Failed to create DirectWrite Factory." << std::endl;
		//std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}
}

CStereoTextRenderer::~CStereoTextRenderer()
{
	if (_resourceCreated)
		ReleaseResources();
}

void CStereoTextRenderer::CreateResources()
{

	// Proper DPI support is very important. Most applications do stupid things like hard coding this, which is why you,
	// can't use proper DPI on most monitors in Windows yet.
	//float dpiX;
	//float dpiY;
	//_d2dFactory->GetDesktopDpi(&dpiX, &dpiY);

	// Now we set up the Direct2D render target bitmap linked to the swapchain.
	// Whenever we render to this bitmap, it will be directly rendered to the
	// swapchain associated with the window.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)/*,
			dpiX,
			dpiY*/
		);

	// Direct2D needs the dxgi version of the backbuffer surface pointer.
	CComPtr<IDXGIResource1> dxgiBackBuffer;
	HRESULT hr = _swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

	if (hr != S_OK)
	{
		// TODO: log
		return;
	}

	CComPtr<IDXGISurface2> dxgiSurface;
	hr = dxgiBackBuffer->CreateSubresourceSurface(0, &dxgiSurface);

	if (hr != S_OK)
	{
		// TODO: log
		return;
	}

	// Get a D2D surface from the DXGI back buffer to use as the D2D render target.
	hr = _d2dContext->CreateBitmapFromDxgiSurface(
			dxgiSurface,
			&bitmapProperties,
			&_d2dTargetBitmap);

	if (hr != S_OK)
	{
		// TODO: log
		return;
	}

	dxgiSurface.Release();

	// Stereo swapchains have an arrayed resource, so create a second Target Bitmap
	// for the right eye buffer.
	
	hr = dxgiBackBuffer->CreateSubresourceSurface(1, &dxgiSurface);

	if (hr != S_OK)
	{
		// TODO: log
		return;
	}

	hr = _d2dContext->CreateBitmapFromDxgiSurface(
			dxgiSurface,
			&bitmapProperties,
			&_d2dTargetBitmapRight);
	
	if (hr != S_OK)
	{
		// TODO: log
		return;
	}

	_d2dContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	_resourceCreated = true;
}

void CStereoTextRenderer::ReleaseResources()
{
	if (!_resourceCreated)
		return;

	_d2dTargetBitmap.Release();
	_d2dTargetBitmapRight.Release();

	_resourceCreated = false;
}

IDWriteTextFormat* CStereoTextRenderer::CreateTextFormat(
	_In_z_ WCHAR const* fontFamilyName,
	_In_opt_ IDWriteFontCollection* fontCollection,
	DWRITE_FONT_WEIGHT fontWeight,
	DWRITE_FONT_STYLE fontStyle,
	DWRITE_FONT_STRETCH fontStretch,
	FLOAT fontSize,
	_In_z_ WCHAR const* localeName)
{
	if (!_ptrDwriteFactory)
		return nullptr;

	IDWriteTextFormat* textFormat = nullptr;

	_ptrDwriteFactory->CreateTextFormat(fontFamilyName, fontCollection, fontWeight, fontStyle, fontStretch, fontSize, localeName, &textFormat);

	return textFormat;
}

ID2D1SolidColorBrush* CStereoTextRenderer::CreateSolidColorBrush(CONST D2D1_COLOR_F &color)
{
	if (!_d2dContext)
	{
		// TODO: log
		return nullptr;
	}

	ID2D1SolidColorBrush* solidBrush = nullptr;

	auto result = _d2dContext->CreateSolidColorBrush(color, &solidBrush);
	if (FAILED(result))
	{
		//std::cout << "Failed to create solid color brush." << std::endl;
		//std::cout << "Error was: " << std::hex << result << std::endl;
		return nullptr;
	}

	return solidBrush;
}

void CStereoTextRenderer::RenderText(
	_In_reads_(stringLength) CONST WCHAR *string,
	UINT32 stringLength,
	_In_ IDWriteTextFormat *textFormat,
	_In_ CONST D2D1_RECT_F *layoutRect,
	_In_ ID2D1Brush *defaultForegroundBrush,
	D2D1_DRAW_TEXT_OPTIONS options/* = D2D1_DRAW_TEXT_OPTIONS_NONE*/,
	DWRITE_MEASURING_MODE measuringMode/* = DWRITE_MEASURING_MODE_NATURAL*/)
{
	_d2dContext->DrawText(string, stringLength, textFormat, layoutRect, defaultForegroundBrush, options, measuringMode);
}

unsigned int CStereoTextRenderer::GetTargetsCount() const
{
	if (_d2dTargetBitmap && _d2dTargetBitmapRight)
		return 2;

	if (_d2dTargetBitmap)
		return 1;

	return 0;
}

ID2D1Bitmap1* CStereoTextRenderer::GetTarget(unsigned int target) const
{
	ID2D1Bitmap1* currentTarget = nullptr;

	if (target == 0)
		currentTarget = _d2dTargetBitmap;

	if (target == 1)
		currentTarget = _d2dTargetBitmapRight;

	return currentTarget;
}

void CStereoTextRenderer::BeginDraw(unsigned int target)
{
	ID2D1Bitmap1* currentTarget = GetTarget(target);

	if (!currentTarget)
	{
		return;
	}

	_d2dContext->SetTarget(currentTarget);

	_d2dContext->BeginDraw();
}

void CStereoTextRenderer::EndDraw(unsigned int target)
{
	ID2D1Bitmap1* currentTarget = GetTarget(target);

	if (!currentTarget)
	{
		return;
	}

	_d2dContext->EndDraw();
}
