#pragma once

#include <d2d1.h>
#include <dwrite.h>

#include <atlbase.h>

class IAbstractTextRenderer
{
public:
	
	virtual void			CreateResources() = 0;
	virtual void			ReleaseResources() = 0;

	virtual void	CreateTextFormat(
		_In_z_ WCHAR const* fontFamilyName,
        _In_opt_ IDWriteFontCollection* fontCollection,
        DWRITE_FONT_WEIGHT fontWeight,
        DWRITE_FONT_STYLE fontStyle,
        DWRITE_FONT_STRETCH fontStretch,
        FLOAT fontSize,
        _In_z_ WCHAR const* localeName,

		CComPtr<IDWriteTextFormat>& out_ptrTextFormat
	) = 0;

	virtual void	CreateSolidColorBrush(CONST D2D1_COLOR_F &color, CComPtr<ID2D1SolidColorBrush>& out_ptrSolidBrush) = 0;

	virtual void 			RenderText(
		_In_reads_(stringLength) CONST WCHAR *string,
	        UINT32 stringLength,
        	_In_ IDWriteTextFormat *textFormat,
	        _In_ CONST D2D1_RECT_F *layoutRect,
        	_In_ ID2D1Brush *defaultForegroundBrush,
	        D2D1_DRAW_TEXT_OPTIONS options = D2D1_DRAW_TEXT_OPTIONS_NONE,
        	DWRITE_MEASURING_MODE measuringMode = DWRITE_MEASURING_MODE_NATURAL 
		) = 0;

	virtual void			FillRectangle(const D2D1_RECT_F &rect, ID2D1Brush  *brush) = 0;
	virtual void			DrawRectangle(const D2D1_RECT_F &rect, ID2D1Brush  *brush) = 0;

	virtual unsigned int	GetTargetsCount() const = 0;
	
	virtual void			BeginDraw(unsigned int target) = 0;

	virtual void			EndDraw(unsigned int target) = 0;

};
