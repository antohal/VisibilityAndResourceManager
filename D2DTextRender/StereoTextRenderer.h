#pragma once


#include "IAbstractTextRenderer.h"

#include <atlbase.h>

#include <d2d1.h>
#include <d2d1_2.h>

#include <dwrite.h>

#include <dxgi.h>
#include <dxgi1_2.h>

class CStereoTextRenderer : public IAbstractTextRenderer
{
public:

	CStereoTextRenderer(ID2D1Factory2* in_d2dFactory, ID2D1Device1* in_d2dDevice, ID2D1DeviceContext1* in_d2dContext, IDXGISwapChain1* in_dxgiSwapChain);
	~CStereoTextRenderer();

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

	virtual unsigned int	GetTargetsCount() const override;

	virtual void			BeginDraw(unsigned int target) override;

	virtual void			EndDraw(unsigned int target) override;

	//@}

private:

	ID2D1Bitmap1*			GetTarget(unsigned int target) const;

	CComPtr<ID2D1Factory2>          _d2dFactory;
	CComPtr<ID2D1Device1>           _d2dDevice;
	CComPtr<ID2D1DeviceContext1>    _d2dContext;
	CComPtr<IDWriteFactory>			_ptrDwriteFactory;

	CComPtr<ID2D1Bitmap1>           _d2dTargetBitmap;
	CComPtr<ID2D1Bitmap1>           _d2dTargetBitmapRight;

	CComPtr<IDXGISwapChain1>        _swapChain;

	bool							_resourceCreated = false;
};
