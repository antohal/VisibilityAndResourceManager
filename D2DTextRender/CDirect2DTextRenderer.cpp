#include "CDirect2DTextRenderer.h"
#include "IAbstractTextRenderer.h"

#include "MonoTextRenderer.h"
#include "StereoTextRenderer.h"

#include <iostream>

#include <set>
#include <atlbase.h>

#include <list>
#include <map>
#include <sstream>

#include <stdarg.h>

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
		// Release для всех ресурсов вызовется автоматически, поскольку они в смарт-поинтерах
		for (CDirect2DTextBlock* pTextBlock : _setTextBlocks)
		{
			delete pTextBlock;
		}


		if (_textRenderer)
			delete _textRenderer;

		_setTextBlocks.clear();
	}

	void Render()
	{
		if (!_textRenderer)
			return;

		unsigned int targetsCount = _textRenderer->GetTargetsCount();

		for (unsigned int iTarget = 0; iTarget < targetsCount; iTarget++)
		{
			_textRenderer->BeginDraw(iTarget);

			for (CDirect2DTextBlock* pTextBlock : _setTextBlocks)
			{
				pTextBlock->Render();
			}

			_textRenderer->EndDraw(iTarget);
		}
	}

	void ReleaseResources()
	{
		if (_textRenderer)
			_textRenderer->ReleaseResources();
		
		for (CDirect2DTextBlock* pTextBlock : _setTextBlocks)
		{
			pTextBlock->ReleaseResources();
		}
	}

	void CreateResources()
	{
		if (_textRenderer)
			_textRenderer->CreateResources();

		for (CDirect2DTextBlock* pTextBlock : _setTextBlocks)
		{
			pTextBlock->CreateResources();
		}
	}

	IAbstractTextRenderer*			GetAbstractTextRenderer() const
	{
		return _textRenderer;
	}

	std::set<CDirect2DTextBlock*>	_setTextBlocks;

	IAbstractTextRenderer*			_textRenderer = nullptr;
};

CDirect2DTextRenderer::CDirect2DTextRenderer()
{
	_private = new TextRendererPrivate;
}

CDirect2DTextRenderer::~CDirect2DTextRenderer()
{
	delete _private;
}

void CDirect2DTextRenderer::InitMono(ID2D1Factory* in_pD2DFactory, IDXGISwapChain* in_pDXGISwapChain)
{
	_private->_textRenderer = new CMonoTextRenderer(in_pD2DFactory, in_pDXGISwapChain);
}

void CDirect2DTextRenderer::InitStereo(ID2D1Factory2* in_d2dFactory, ID2D1Device1* in_d2dDevice, ID2D1DeviceContext1* in_d2dContext, IDXGISwapChain1* in_dxgiSwapChain)
{
	_private->_textRenderer = new CStereoTextRenderer(in_d2dFactory, in_d2dDevice, in_d2dContext, in_dxgiSwapChain);
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
		if (!_pOwner->_private->GetAbstractTextRenderer())
		{
			// TODO: log
			return;
		}

		if (!_bInited)
		{
			return;
		}

		if (!_ptrTextFormat || !_ptrTextBrush)
		{
			// TODO: log error here - not inited owner
			return;
		}

		if (!_visible)
			return;

		if (_ptrRectangleFillBrush)
			_pOwner->_private->GetAbstractTextRenderer()->FillRectangle(_Rect, _ptrRectangleFillBrush);

		if (_ptrRectangleBorderBrush)
			_pOwner->_private->GetAbstractTextRenderer()->DrawRectangle(_Rect, _ptrRectangleBorderBrush);

		_pOwner->_private->GetAbstractTextRenderer()->RenderText(_RenderedText.c_str(), static_cast<UINT32>(_RenderedText.size()), _ptrTextFormat, &_TextRect, _ptrTextBrush);

	}

	void ReleaseResources()
	{
		_ptrTextBrush.Release();
		_ptrTextFormat.Release();
		_ptrRectangleFillBrush.Release();
		_ptrRectangleBorderBrush.Release();
	}

	void CreateResources()
	{
		if (!_pOwner->_private->GetAbstractTextRenderer())
		{
			// TODO: log error here - not inited owner
			return;
		}

		if (!_bInited)
		{
			return;
		}

	
		_pOwner->_private->GetAbstractTextRenderer()->CreateSolidColorBrush(_textColor, _ptrTextBrush);
		_pOwner->_private->GetAbstractTextRenderer()->CreateSolidColorBrush(_rectangleFillColor, _ptrRectangleFillBrush);
		_pOwner->_private->GetAbstractTextRenderer()->CreateSolidColorBrush(_rectangleBorderColor, _ptrRectangleBorderBrush);

		_pOwner->_private->GetAbstractTextRenderer()->CreateTextFormat(_FontName.c_str(), nullptr, _FontWeight, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, _fFontSize, L"", _ptrTextFormat);
	}


	void AddTextLine(const std::wstring& in_wsTextLine)
	{
		_lstTextLines.push_back(TextLine());

		TextLine& line = _lstTextLines.back();
		line._wsText = in_wsTextLine;


		if (_maxTextLines != (unsigned int)(-1))
		{
			if (_lstTextLines.size() > _maxTextLines)
				_lstTextLines.pop_front();
		}

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

			if (line._wsFormat.empty() && line._uiParamHandle != UINT(-1))
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

	void SetMaximumTextLines(unsigned int in_nMaxLinesCount)
	{
		_maxTextLines = in_nMaxLinesCount;
	}

	UINT AddFormattedTextLine(const wchar_t* in_pcwszFormat)
	{
		_lstTextLines.push_back(TextLine());

		TextLine& line = _lstTextLines.back();
		
		line._wsFormat = in_pcwszFormat;
		line._uiParamHandle = static_cast<UINT>(_mapParameters.size());

		_mapParameters[line._uiParamHandle] = &line;

		UpdateRenderedText();

		return line._uiParamHandle;
	}

	// Изменить параметры форматированной строки
	void UpdateFormattedTextLine(UINT in_uiFormattedLineId, va_list args)
	{
		auto it = _mapParameters.find(in_uiFormattedLineId);

		if (it == _mapParameters.end())
		{
			// TODO: error here
			return;
		}

		static wchar_t buf[2048];
		int size = vswprintf_s(buf, 2048, it->second->_wsFormat.c_str(), args);

		if (size > 0)
		{
			it->second->_wsText = std::wstring(buf, size);
			if (it->second->_wsText.back() == L'\0')
				it->second->_wsText.pop_back();
		}

		UpdateRenderedText();
	}


	struct TextLine
	{
		std::wstring	_wsText;
		std::wstring	_wsFormat;
		UINT			_uiParamHandle = -1;
		float			_fParamValue = 0;
	};

	std::list<TextLine>				_lstTextLines;
	std::map<UINT, TextLine*>		_mapParameters;

	//@{ params

	D2D1_COLOR_F					_textColor;
	D2D1_COLOR_F					_rectangleFillColor;
	D2D1_COLOR_F					_rectangleBorderColor;
	D2D1_RECT_F						_Rect;
	D2D1_RECT_F						_TextRect;
	std::wstring					_FontName;
	DWRITE_FONT_WEIGHT				_FontWeight;
	float							_fFontSize;
	bool							_bInited = false;

	float							_fHorizontalTextOffset = 0;
	float							_fVerticalTextOffset = 0;

	//@}

	std::wstring					_RenderedText;

	CComPtr<ID2D1SolidColorBrush>   _ptrTextBrush;
	CComPtr<ID2D1SolidColorBrush>   _ptrRectangleFillBrush;
	CComPtr<ID2D1SolidColorBrush>   _ptrRectangleBorderBrush;

	CComPtr<IDWriteTextFormat>		_ptrTextFormat;

	CDirect2DTextRenderer*			_pOwner = nullptr;

	bool							_visible = true;

	unsigned int					_maxTextLines = -1;
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

// Добавить форматированную строку, по идентификатору которой можно изменять параметры, например "vector = [%f, %f, %f]"
UINT CDirect2DTextBlock::AddFormattedTextLine(const wchar_t* in_pcwszFormat)
{
	return _private->AddFormattedTextLine(in_pcwszFormat);
}

// Изменить параметры форматированной строки
void CDirect2DTextBlock::UpdateFormattedTextLine(UINT in_uiFormattedLineId, ...)
{
	va_list args;
	va_start(args, in_uiFormattedLineId);
	_private->UpdateFormattedTextLine(in_uiFormattedLineId, args);
	va_end(args);
}

void CDirect2DTextBlock::Init(
	const D2D1_COLOR_F& in_textColor,				// цвет текста
	const D2D1_RECT_F& in_rcPlacement,				// прямоугольник, в котором выводится текст
	const D2D1_COLOR_F& in_rectangleFillColor,		// цвет, которым заполняется прямоугольник
	const D2D1_COLOR_F& in_rectangleBorderColor,	// цвет границы прямоугольника
	float in_fHorizontalTextOffset,					// отступ текста от границ прямоугольника по горизонтали
	float in_fVerticalTextOffset,					// отступ текста от границ прямоугольника по вертикали
	const char* in_pcszFontName,					// имя шрифта (например Arial)
	DWRITE_FONT_WEIGHT in_FontWeight,				// тип шрифта (например DWRITE_FONT_WEIGHT_NORMAL или DWRITE_FONT_WEIGHT_BOLD)
	float in_fFontSize								// размер шрифта 
)
{
	_private->_textColor = in_textColor;
	_private->_rectangleFillColor = in_rectangleFillColor;
	_private->_rectangleBorderColor = in_rectangleBorderColor;

	_private->_Rect = in_rcPlacement;
	_private->_TextRect = in_rcPlacement;

	_private->_fHorizontalTextOffset = in_fHorizontalTextOffset;
	_private->_fVerticalTextOffset	 = in_fVerticalTextOffset;

	_private->_TextRect.left += in_fHorizontalTextOffset;
	_private->_TextRect.right -= in_fHorizontalTextOffset;
	_private->_TextRect.top += in_fVerticalTextOffset;
	_private->_TextRect.bottom -= in_fVerticalTextOffset;

	_private->_FontName = string_cast<std::wstring>(std::string(in_pcszFontName));
	_private->_FontWeight = in_FontWeight;
	_private->_fFontSize = in_fFontSize;

	_private->_bInited = true;
}

void CDirect2DTextBlock::ChangePlacement(const D2D1_RECT_F& in_rcPlacement)
{
	_private->_Rect = in_rcPlacement;
	_private->_TextRect = in_rcPlacement;

	_private->_TextRect.left += _private->_fHorizontalTextOffset;
	_private->_TextRect.right -= _private->_fHorizontalTextOffset;
	_private->_TextRect.top += _private->_fVerticalTextOffset;
	_private->_TextRect.bottom -= _private->_fVerticalTextOffset;
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

void CDirect2DTextBlock::SetMaximumTextLines(unsigned int in_nMaxLinesCount)
{
	_private->SetMaximumTextLines(in_nMaxLinesCount);
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

// Установить видимость текстового блока
void CDirect2DTextBlock::SetVisible(bool visible)
{
	_private->_visible = visible;
}

// Функция возвращает признак видимости текстового блока
bool CDirect2DTextBlock::IsVisible() const
{
	return _private->_visible;
}
