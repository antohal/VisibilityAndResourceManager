#include "CDirect2DTextRenderer.h"

#include <iostream>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

//
// CDirect2DTextRenderer
//

CDirect2DTextRenderer::CDirect2DTextRenderer()
{

}

CDirect2DTextRenderer::~CDirect2DTextRenderer()
{
	for (CDirect2DTextBlock* pTextBlock : m_setTextBlocks)
	{
		delete pTextBlock;
	}

	m_setTextBlocks.clear();
}

void CDirect2DTextRenderer::Init(ID2D1Factory* in_pD2DFactory, IDXGISwapChain* in_pDXGISwapChain)
{
	m_ptrD2DFactory = in_pD2DFactory;
	m_ptrD2DSwapChain = in_pDXGISwapChain;

	auto result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown * *>(&m_ptrDwriteFactory));
	if (FAILED(result)) 
	{
		std::cout << "Failed to create DirectWrite Factory." << std::endl;
		std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}

	CreateResources();
}

void CDirect2DTextRenderer::Render()
{
	m_ptrRenderTarget->BeginDraw();

	for (CDirect2DTextBlock* pTextBlock : m_setTextBlocks)
	{
		pTextBlock->Render();
	}

	m_ptrRenderTarget->EndDraw();
}

ID2D1RenderTarget*	CDirect2DTextRenderer::GetRenderTarget() const
{
	return m_ptrRenderTarget;
}

IDWriteFactory* CDirect2DTextRenderer::GetDWriteFactory() const
{
	return m_ptrDwriteFactory;
}

CDirect2DTextBlock*	CDirect2DTextRenderer::CreateTextBlock()
{
	CDirect2DTextBlock* pNewTextBlock = new CDirect2DTextBlock(this);
	m_setTextBlocks.insert(pNewTextBlock);
	return pNewTextBlock;
}

void CDirect2DTextRenderer::DeleteTextBlock(CDirect2DTextBlock* in_pTextBlock)
{
	auto it = m_setTextBlocks.find(in_pTextBlock);
	if (it == m_setTextBlocks.end())
	{
		// TODO: error here
		return;
	}

	m_setTextBlocks.erase(it);

	delete in_pTextBlock;
}

void CDirect2DTextRenderer::ReleaseResources()
{
	m_ptrRenderTarget.Release();


	for (CDirect2DTextBlock* pTextBlock : m_setTextBlocks)
	{
		pTextBlock->ReleaseResources();
	}
}

void CDirect2DTextRenderer::CreateResources()
{
	if (!m_ptrD2DFactory)
	{
		//TODO: log error

		return;
	}

	CComPtr<IDXGISurface> backBufferSurface;

	// Get a DXGI surface for D2D use.
	auto result = m_ptrD2DSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferSurface));
	if (FAILED(result)) 
	{
		std::cout << "Failed to get DXGI surface for back buffer." << std::endl;
		std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}

	// Proper DPI support is very important. Most applications do stupid things like hard coding this, which is why you,
	// can't use proper DPI on most monitors in Windows yet.
	float dpiX;
	float dpiY;
	m_ptrD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

	// DXGI_FORMAT_UNKNOWN will cause it to use the same format as the back buffer (R8G8B8A8_UNORM)
	auto d2dRTProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

	// Wraps up our DXGI surface in a D2D render target.
	result = m_ptrD2DFactory->CreateDxgiSurfaceRenderTarget(backBufferSurface, &d2dRTProps, &m_ptrRenderTarget);
	if (FAILED(result)) 
	{
		std::cout << "Failed to create D2D DXGI Render Target." << std::endl;
		std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}

	for (CDirect2DTextBlock* pTextBlock : m_setTextBlocks)
	{
		pTextBlock->CreateResources();
	}
}


//
// CDirect2DTextBlock
//

CDirect2DTextBlock::CDirect2DTextBlock(CDirect2DTextRenderer* in_pOwner)
	: m_pOwner(in_pOwner)
{

}

CDirect2DTextBlock::~CDirect2DTextBlock()
{

}

void CDirect2DTextBlock::Init(const D2D1_COLOR_F& in_Color, const D2D1_RECT_F& in_rcPlacement, const std::wstring& in_wsFontName, DWRITE_FONT_WEIGHT in_FontWeight, float in_fFontSize)
{
	m_Color = in_Color;
	m_Rect = in_rcPlacement;
	m_FontName = in_wsFontName;
	m_FontWeight = in_FontWeight;
	m_fFontSize = in_fFontSize;

	m_bInited = true;

	CreateResources();
}

void CDirect2DTextBlock::ChangePlacement(const D2D1_RECT_F& in_rcPlacement)
{
	m_Rect = in_rcPlacement;
}

void CDirect2DTextBlock::AddTextLine(const std::wstring& in_wsTextLine)
{
	m_RenderedText += L"\n" + in_wsTextLine;
}

UINT CDirect2DTextBlock::AddParameter(const std::wstring& in_wsParamName)
{
	return 0;
}

void CDirect2DTextBlock::SetParameterValue(UINT in_paramHandle, float in_fParameterValue)
{

}

void CDirect2DTextBlock::ClearText()
{
	m_RenderedText.clear();
}

void CDirect2DTextBlock::ReleaseResources()
{
	m_ptrSolidBrush.Release();
	m_ptrTextFormat.Release();
}

void CDirect2DTextBlock::Render()
{
	if (!m_pOwner->GetRenderTarget() || !m_pOwner->GetDWriteFactory())
	{
		// TODO: log error here - not inited owner
		return;
	}

	if (!m_bInited)
	{
		return;
	}

	if (!m_ptrTextFormat || !m_ptrSolidBrush)
	{
		// TODO: log error here - not inited owner
		return;
	}

	m_pOwner->GetRenderTarget()->DrawText(m_RenderedText.c_str(), m_RenderedText.length(), m_ptrTextFormat, m_Rect, m_ptrSolidBrush);
}

void CDirect2DTextBlock::CreateResources()
{
	if (!m_pOwner->GetRenderTarget() || !m_pOwner->GetDWriteFactory())
	{
		// TODO: log error here - not inited owner
		return;
	}

	if (!m_bInited)
	{
		return;
	}

	// This is the brush we will be using to render our text, it does not need to be a solid color,
	// we could use any brush we wanted. In this case we chose a nice solid red brush.
	auto result = m_pOwner->GetRenderTarget()->CreateSolidColorBrush(m_Color, &m_ptrSolidBrush);
	if (FAILED(result)) 
	{
		std::cout << "Failed to create solid color brush." << std::endl;
		std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}


	result = m_pOwner->GetDWriteFactory()->CreateTextFormat(m_FontName.c_str(), nullptr, m_FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, m_fFontSize, L"", &m_ptrTextFormat);
	if (FAILED(result)) 
	{
		std::cout << "Failed to create DirectWrite text format." << std::endl;
		std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}
}
