#pragma once

#include <d2d1.h>
#include <dwrite.h>

#include <string>
#include <set>

#include <atlbase.h>

class CDirect2DTextBlock;

class CDirect2DTextRenderer
{
public:

	CDirect2DTextRenderer();
	~CDirect2DTextRenderer();

	void Init(ID2D1Factory* in_pD2DFactory, IDXGISwapChain* in_pDXGISwapChain);
	
	void Render();

	void ReleaseResources();
	void CreateResources();


	CDirect2DTextBlock*	CreateTextBlock();
	void DeleteTextBlock(CDirect2DTextBlock* in_pTextBlock);

protected:

	ID2D1RenderTarget*	GetRenderTarget() const;
	IDWriteFactory*		GetDWriteFactory() const;

private:

	std::set<CDirect2DTextBlock*>	m_setTextBlocks;

	CComPtr<ID2D1Factory>		m_ptrD2DFactory;
	CComPtr<IDWriteFactory>		m_ptrDwriteFactory;
	CComPtr<IDXGISwapChain>		m_ptrD2DSwapChain;

	CComPtr<ID2D1RenderTarget>	m_ptrRenderTarget;

	friend class CDirect2DTextBlock;
};


class CDirect2DTextBlock
{
public:

	void Init(const D2D1_COLOR_F& in_Color, const D2D1_RECT_F& in_rcPlacement, const std::wstring& in_wsFontName, DWRITE_FONT_WEIGHT in_FontWeight, float in_fFontSize);

	void ChangePlacement(const D2D1_RECT_F& in_rcPlacement);

	void AddTextLine(const std::wstring& in_wsTextLine);

	UINT AddParameter(const std::wstring& in_wsParamName);

	void SetParameterValue(UINT in_paramHandle, float in_fParameterValue);

	void ClearText();

protected:
	
	CDirect2DTextBlock(CDirect2DTextRenderer* in_pOwner);
	~CDirect2DTextBlock();

	void Render();

	void ReleaseResources();
	void CreateResources();


private:


	//@{ params

	D2D1_COLOR_F					m_Color;
	D2D1_RECT_F						m_Rect;
	std::wstring					m_FontName;
	DWRITE_FONT_WEIGHT				m_FontWeight;
	float							m_fFontSize;
	bool							m_bInited = false;

	//@}

	std::wstring					m_RenderedText;

	CComPtr<ID2D1SolidColorBrush>   m_ptrSolidBrush;
	CComPtr<IDWriteTextFormat>		m_ptrTextFormat;

	CDirect2DTextRenderer*			m_pOwner = nullptr;

	friend class CDirect2DTextRenderer;
};
