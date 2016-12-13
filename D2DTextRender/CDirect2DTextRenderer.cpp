#include "CDirect2DTextRenderer.h"

#include <iostream>

#include <set>
#include <atlbase.h>

#include <list>
#include <map>
#include <sstream>

#include "string_cast.hpp"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

//
// CDirect2DTextRenderer
//

struct CDirect2DTextRenderer::TextRendererPrivate
{
	~TextRendererPrivate()
	{
		for (CDirect2DTextBlock* pTextBlock : _setTextBlocks)
		{
			delete pTextBlock;
		}

		_setTextBlocks.clear();
	}

	ID2D1RenderTarget*	GetRenderTarget() const
	{
		return _ptrRenderTarget;
	}

	IDWriteFactory*		GetDWriteFactory() const
	{
		return _ptrDwriteFactory;
	}

	void Render()
	{
		_ptrRenderTarget->BeginDraw();

		for (CDirect2DTextBlock* pTextBlock : _setTextBlocks)
		{
			pTextBlock->Render();
		}

		_ptrRenderTarget->EndDraw();
	}

	void ReleaseResources()
	{
		_ptrRenderTarget.Release();
		
		for (CDirect2DTextBlock* pTextBlock : _setTextBlocks)
		{
			pTextBlock->ReleaseResources();
		}
	}

	void CreateResources()
	{
		if (!_ptrD2DFactory)
		{
			//TODO: log error

			return;
		}

		CComPtr<IDXGISurface> backBufferSurface;

		// Get a DXGI surface for D2D use.
		auto result = _ptrD2DSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferSurface));
		if (FAILED(result))
		{
			//std::cout << "Failed to get DXGI surface for back buffer." << std::endl;
			//std::cout << "Error was: " << std::hex << result << std::endl;
			return;
		}

		// Proper DPI support is very important. Most applications do stupid things like hard coding this, which is why you,
		// can't use proper DPI on most monitors in Windows yet.
		float dpiX;
		float dpiY;
		_ptrD2DFactory->GetDesktopDpi(&dpiX, &dpiY);

		// DXGI_FORMAT_UNKNOWN will cause it to use the same format as the back buffer (R8G8B8A8_UNORM)
		auto d2dRTProps = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpiX, dpiY);

		// Wraps up our DXGI surface in a D2D render target.
		result = _ptrD2DFactory->CreateDxgiSurfaceRenderTarget(backBufferSurface, &d2dRTProps, &_ptrRenderTarget);
		if (FAILED(result))
		{
			std::cout << "Failed to create D2D DXGI Render Target." << std::endl;
			std::cout << "Error was: " << std::hex << result << std::endl;
			return;
		}

		for (CDirect2DTextBlock* pTextBlock : _setTextBlocks)
		{
			pTextBlock->CreateResources();
		}
	}

	std::set<CDirect2DTextBlock*>	_setTextBlocks;

	CComPtr<ID2D1Factory>			_ptrD2DFactory;
	CComPtr<IDWriteFactory>			_ptrDwriteFactory;
	CComPtr<IDXGISwapChain>			_ptrD2DSwapChain;

	CComPtr<ID2D1RenderTarget>		_ptrRenderTarget;
};

CDirect2DTextRenderer::CDirect2DTextRenderer()
{
	_private = new TextRendererPrivate;
}

CDirect2DTextRenderer::~CDirect2DTextRenderer()
{
	delete _private;
}

void CDirect2DTextRenderer::Init(ID2D1Factory* in_pD2DFactory, IDXGISwapChain* in_pDXGISwapChain)
{
	_private->_ptrD2DFactory = in_pD2DFactory;
	_private->_ptrD2DSwapChain = in_pDXGISwapChain;

	auto result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown * *>(&_private->_ptrDwriteFactory));
	if (FAILED(result)) 
	{
		//std::cout << "Failed to create DirectWrite Factory." << std::endl;
		//std::cout << "Error was: " << std::hex << result << std::endl;
		return;
	}

	CreateResources();
}

void CDirect2DTextRenderer::Render()
{
	_private->Render();
}

CDirect2DTextBlock*	CDirect2DTextRenderer::CreateTextBlock()
{
	CDirect2DTextBlock* pNewTextBlock = new CDirect2DTextBlock(this);
	_private->_setTextBlocks.insert(pNewTextBlock);
	return pNewTextBlock;
}

void CDirect2DTextRenderer::DeleteTextBlock(CDirect2DTextBlock* in_pTextBlock)
{
	auto it = _private->_setTextBlocks.find(in_pTextBlock);
	if (it == _private->_setTextBlocks.end())
	{
		// TODO: error here
		return;
	}

	_private->_setTextBlocks.erase(it);

	delete in_pTextBlock;
}

void CDirect2DTextRenderer::ReleaseResources()
{
	_private->ReleaseResources();
}

void CDirect2DTextRenderer::CreateResources()
{
	_private->CreateResources();
}


//
// CDirect2DTextBlock
//

struct CDirect2DTextBlock::TextBlockPrivate
{
	void Render()
	{
		if (!_pOwner->_private->GetRenderTarget() || !_pOwner->_private->GetDWriteFactory())
		{
			// TODO: log error here - not inited owner
			return;
		}

		if (!_bInited)
		{
			return;
		}

		if (!_ptrTextFormat || !_ptrSolidBrush)
		{
			// TODO: log error here - not inited owner
			return;
		}

		_pOwner->_private->GetRenderTarget()->DrawText(_RenderedText.c_str(), static_cast<UINT32>(_RenderedText.length()), _ptrTextFormat, _Rect, _ptrSolidBrush);
	}

	void ReleaseResources()
	{
		_ptrSolidBrush.Release();
		_ptrTextFormat.Release();
	}

	void CreateResources()
	{
		if (!_pOwner->_private->GetRenderTarget() || !_pOwner->_private->GetDWriteFactory())
		{
			// TODO: log error here - not inited owner
			return;
		}

		if (!_bInited)
		{
			return;
		}

		// This is the brush we will be using to render our text, it does not need to be a solid color,
		// we could use any brush we wanted. In this case we chose a nice solid red brush.
		auto result = _pOwner->_private->GetRenderTarget()->CreateSolidColorBrush(_Color, &_ptrSolidBrush);
		if (FAILED(result))
		{
			//std::cout << "Failed to create solid color brush." << std::endl;
			//std::cout << "Error was: " << std::hex << result << std::endl;
			return;
		}


		result = _pOwner->_private->GetDWriteFactory()->CreateTextFormat(_FontName.c_str(), nullptr, _FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, _fFontSize, L"", &_ptrTextFormat);
		if (FAILED(result))
		{
			//std::cout << "Failed to create DirectWrite text format." << std::endl;
			//std::cout << "Error was: " << std::hex << result << std::endl;
			return;
		}
	}


	void AddTextLine(const std::wstring& in_wsTextLine)
	{
		_lstTextLines.push_back(TextLine());

		TextLine& line = _lstTextLines.back();
		line._wsText = in_wsTextLine;

		UpdateRenderedText();
	}

	UINT AddParameter(const std::wstring& in_wsParamName)
	{
		_lstTextLines.push_back(TextLine());

		TextLine& line = _lstTextLines.back();
		line._wsText = in_wsParamName + L" : ";
		line._uiParamHandle = static_cast<UINT>(_mapParameters.size());

		_mapParameters[line._uiParamHandle] = &line;

		UpdateRenderedText();

		return line._uiParamHandle;
	}

	void SetParameterValue(UINT in_paramHandle, float in_fParameterValue)
	{
		auto it = _mapParameters.find(in_paramHandle);

		if (it == _mapParameters.end())
		{
			// TODO: error here
			return;
		}

		it->second->_fParamValue = in_fParameterValue;

		UpdateRenderedText();
	}

	void UpdateRenderedText()
	{
		std::wostringstream woss;
		for (const TextLine& line : _lstTextLines)
		{
			woss << line._wsText;

			if (line._uiParamHandle != UINT(-1))
			{
				woss << line._fParamValue;
			}

			woss << std::endl;
		}

		_RenderedText = woss.str();
	}

	void ClearText()
	{
		_RenderedText.clear();
		_lstTextLines.clear();
		_mapParameters.clear();
	}

	struct TextLine
	{
		std::wstring	_wsText;
		UINT			_uiParamHandle = -1;
		float			_fParamValue = 0;
	};

	std::list<TextLine>				_lstTextLines;
	std::map<UINT, TextLine*>		_mapParameters;

	//@{ params

	D2D1_COLOR_F					_Color;
	D2D1_RECT_F						_Rect;
	std::wstring					_FontName;
	DWRITE_FONT_WEIGHT				_FontWeight;
	float							_fFontSize;
	bool							_bInited = false;

	//@}

	std::wstring					_RenderedText;

	CComPtr<ID2D1SolidColorBrush>   _ptrSolidBrush;
	CComPtr<IDWriteTextFormat>		_ptrTextFormat;

	CDirect2DTextRenderer*			_pOwner = nullptr;
};

CDirect2DTextBlock::CDirect2DTextBlock(CDirect2DTextRenderer* in_pOwner)
{
	_private = new TextBlockPrivate;
	_private->_pOwner = in_pOwner;
}

CDirect2DTextBlock::~CDirect2DTextBlock()
{
	delete _private;
}

void CDirect2DTextBlock::Init(const D2D1_COLOR_F& in_Color, const D2D1_RECT_F& in_rcPlacement, const char* in_pcszFontName, DWRITE_FONT_WEIGHT in_FontWeight, float in_fFontSize)
{
	_private->_Color = in_Color;
	_private->_Rect = in_rcPlacement;
	_private->_FontName = string_cast<std::wstring>(std::string(in_pcszFontName));
	_private->_FontWeight = in_FontWeight;
	_private->_fFontSize = in_fFontSize;

	_private->_bInited = true;

	_private->CreateResources();
}

void CDirect2DTextBlock::ChangePlacement(const D2D1_RECT_F& in_rcPlacement)
{
	_private->_Rect = in_rcPlacement;
}
// Добавить простую текстовую строку (версия ANSI)
void CDirect2DTextBlock::AddTextLine(const char* in_pcszTextLine)
{
	_private->AddTextLine(string_cast<std::wstring>(std::string(in_pcszTextLine)));
}

// Добавить простую текстовую строку (версия UNICODE)
void CDirect2DTextBlock::AddTextLine(const wchar_t* in_pcwszTextLine)
{
	_private->AddTextLine(std::wstring(in_pcwszTextLine));
}

// Добавить параметр (возвращает хэндл параметра, по которому можно изменять значение) (версия ANSI)
UINT CDirect2DTextBlock::AddParameter(const char* in_pcszParamName)
{
	return _private->AddParameter(string_cast<std::wstring>(std::string(in_pcszParamName)));
}

// Добавить параметр (возвращает хэндл параметра, по которому можно изменять значение) (версия UNICODE)
UINT CDirect2DTextBlock::AddParameter(const wchar_t* in_pcwszParamName)
{
	return _private->AddParameter(std::wstring(in_pcwszParamName));
}

void CDirect2DTextBlock::SetParameterValue(UINT in_uiParamHandle, float in_fParameterValue)
{
	_private->SetParameterValue(in_uiParamHandle, in_fParameterValue);
}

void CDirect2DTextBlock::ClearText()
{
	_private->ClearText();
}

void CDirect2DTextBlock::CreateResources()
{
	_private->CreateResources();
}

void CDirect2DTextBlock::ReleaseResources()
{
	_private->ReleaseResources();
}

void CDirect2DTextBlock::Render()
{
	_private->Render();
}
