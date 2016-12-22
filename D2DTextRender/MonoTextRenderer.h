#pragma once

#include "IAbstractTextRenderer.h"

#include <atlbase.h>

#include <dxgi.h>
#include <dxgi1_2.h>

class CMonoTextRenderer : public IAbstractTextRenderer
{
public:

	CMonoTextRenderer(ID2D1Factory* in_pD2DFactory, IDXGISwapChain* in_pDXGISwapChain);
	~CMonoTextRenderer();

	//@{ IAbstractTextRenderer

	virtual void			CreateResources() override;
	virtual void			ReleaseResources() override;

	virtual IDWriteTextFormat*	CreateTextFormat(
		_In_z_ WCHAR const* fontFamilyName,
		_In_opt_ IDWriteFontCollection* fontCollection,
		DWRITE_FONT_WEIGHT fontWeight,
		DWRITE_FONT_STYLE fontStyle,
		DWRITE_FONT_STRETCH fontStretch,
		FLOAT fontSize,
		_In_z_ WCHAR const* localeName
	) override;

	virtual ID2D1SolidColorBrush*	CreateSolidColorBrush(CONST D2D1_COLOR_F &color) override;

	virtual void 			RenderText(
		_In_reads_(stringLength) CONST WCHAR *string,
		UINT32 stringLength,
		_In_ IDWriteTextFormat *textFormat,
		_In_ CONST D2D1_RECT_F *layoutRect,
		_In_ ID2D1Brush *defaultForegroundBrush,
		D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL
	) override;

	virtual unsigned int	GetTargetsCount() const override
	{
		return 1;
	}

	virtual void			BeginDraw(unsigned int target) override;

	virtual void			EndDraw(unsigned int target) override;

	//@}

private:

	CComPtr<ID2D1Factory>			_ptrD2DFactory;
	CComPtr<IDWriteFactory>			_ptrDwriteFactory;
	CComPtr<IDXGISwapChain>			_ptrD2DSwapChain;

	CComPtr<ID2D1RenderTarget>		_ptrRenderTarget;

	bool							_resourceCreated = false;
};