#pragma once

#include <d2d1.h>
#include <dwrite.h>

#include <string>
#include <set>

#include <atlbase.h>


#ifdef D2DTEXTRENDER_EXPORTS
#define D2DTEXTRENDER_INTERFACE __declspec(dllexport)
#else
#define D2DTEXTRENDER_INTERFACE __declspec(dllimport)
#endif

class CDirect2DTextBlock;

class D2DTEXTRENDER_INTERFACE CDirect2DTextRenderer
{
public:

	CDirect2DTextRenderer();
	~CDirect2DTextRenderer();

	// Функция инициализации - вызывается один раз и перед всеми остальными вызовами
	void Init(ID2D1Factory* in_pD2DFactory, IDXGISwapChain* in_pDXGISwapChain);

	// Освободить ресурсы - вызывается в случае освобождения SwapChain (например при изменении размоеров окна)
	void ReleaseResources();

	// Создать ресурсы - нужно обязательно вызвать после создания SwapChain
	void CreateResources();

	// Функция отрисовки - необходимо вызывать перед SwapChain->Present
	void Render();

	// Создать новый текстовый блок - возвращает указатель на новый текстовый блок, с которым можно производить операции, описанные в классе ниже
	CDirect2DTextBlock*	CreateTextBlock();

	// Удалить текстовый блок
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


class D2DTEXTRENDER_INTERFACE CDirect2DTextBlock
{
public:

	// Инициализировать текстовый блок. Эту функцию нужно вызывать обязательно, после создания текстового блока
	void Init(const D2D1_COLOR_F& in_Color, const D2D1_RECT_F& in_rcPlacement, const std::wstring& in_wsFontName, DWRITE_FONT_WEIGHT in_FontWeight, float in_fFontSize);

	// Изменить положение текстового блока
	void ChangePlacement(const D2D1_RECT_F& in_rcPlacement);

	// Добавить простую текстовую строку
	void AddTextLine(const std::wstring& in_wsTextLine);

	// Добавить параметр (возвращает хэндл параметра, по которому можно изменять значение)
	UINT AddParameter(const std::wstring& in_wsParamName);

	// Установить значение параметра
	void SetParameterValue(UINT in_paramHandle, float in_fParameterValue);

	// Очистить текст - удалить все добавленные строки и параметры
	void ClearText();

protected:
	
	// Создавать и удалять текстовые блоки может только рендерер

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
