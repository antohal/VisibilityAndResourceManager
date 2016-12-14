﻿#pragma once

#include <d2d1.h>
#include <dwrite.h>

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

	// Создать ресурсы - нужно обязательно вызвать после создания SwapChain
	void CreateResources();

	// Освободить ресурсы - вызывается в случае освобождения SwapChain (например при изменении размоеров окна)
	void ReleaseResources();

	// Функция отрисовки - необходимо вызывать перед SwapChain->Present
	void Render();

	// Создать новый текстовый блок - возвращает указатель на новый текстовый блок, с которым можно производить операции, описанные в классе ниже
	CDirect2DTextBlock*	CreateTextBlock();

	// Удалить текстовый блок
	void DeleteTextBlock(CDirect2DTextBlock* in_pTextBlock);

private:

	struct TextRendererPrivate;
	TextRendererPrivate*	_private = nullptr;

	friend class CDirect2DTextBlock;
};


class D2DTEXTRENDER_INTERFACE CDirect2DTextBlock
{
public:

	// Инициализировать текстовый блок. Эту функцию нужно вызывать обязательно, после создания текстового блока
	void Init(const D2D1_COLOR_F& in_Color, const D2D1_RECT_F& in_rcPlacement, const char* in_pcszFontName, DWRITE_FONT_WEIGHT in_FontWeight, float in_fFontSize);

	// Изменить положение текстового блока
	void ChangePlacement(const D2D1_RECT_F& in_rcPlacement);

	// Добавить простую текстовую строку (версия ANSI)
	void AddTextLine(const char* in_pcszTextLine);

	// Добавить простую текстовую строку (версия UNICODE)
	void AddTextLine(const wchar_t* in_pcwszTextLine);

	// Добавить параметр (возвращает хэндл параметра, по которому можно изменять значение) (версия ANSI)
	UINT AddParameter(const char* in_pcszParamName);

	// Добавить параметр (возвращает хэндл параметра, по которому можно изменять значение) (версия UNICODE)
	UINT AddParameter(const wchar_t* in_pcwszParamName);

	// Установить значение параметра
	void SetParameterValue(UINT in_uiParamHandle, float in_fParameterValue);

	// Очистить текст - удалить все добавленные строки и параметры
	void ClearText();

protected:
	
	// Создавать и удалять текстовые блоки может только рендерер

	CDirect2DTextBlock(CDirect2DTextRenderer* in_pOwner);
	~CDirect2DTextBlock();

private:

	// Создать ресурсы - вызывает CDirect2DTextRenderer
	void CreateResources();

	// Освободить ресурсы - вызывает CDirect2DTextRenderer
	void ReleaseResources();

	// Функция отрисовки - вызывает CDirect2DTextRenderer
	void Render();

	struct TextBlockPrivate;
	TextBlockPrivate*				_private = nullptr;

	friend class CDirect2DTextRenderer;
};
