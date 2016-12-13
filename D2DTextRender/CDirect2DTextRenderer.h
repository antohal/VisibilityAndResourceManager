#pragma once

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

	// ������� ������������� - ���������� ���� ��� � ����� ����� ���������� ��������
	void Init(ID2D1Factory* in_pD2DFactory, IDXGISwapChain* in_pDXGISwapChain);

	// ������� ������� - ����� ����������� ������� ����� �������� SwapChain
	void CreateResources();

	// ���������� ������� - ���������� � ������ ������������ SwapChain (�������� ��� ��������� ��������� ����)
	void ReleaseResources();

	// ������� ��������� - ���������� �������� ����� SwapChain->Present
	void Render();

	// ������� ����� ��������� ���� - ���������� ��������� �� ����� ��������� ����, � ������� ����� ����������� ��������, ��������� � ������ ����
	CDirect2DTextBlock*	CreateTextBlock();

	// ������� ��������� ����
	void DeleteTextBlock(CDirect2DTextBlock* in_pTextBlock);

private:

	struct TextRendererPrivate;
	TextRendererPrivate*	_private = nullptr;

	friend class CDirect2DTextBlock;
};


class D2DTEXTRENDER_INTERFACE CDirect2DTextBlock
{
public:

	// ���������������� ��������� ����. ��� ������� ����� �������� �����������, ����� �������� ���������� �����
	void Init(const D2D1_COLOR_F& in_Color, const D2D1_RECT_F& in_rcPlacement, const char* in_pcszFontName, DWRITE_FONT_WEIGHT in_FontWeight, float in_fFontSize);

	// �������� ��������� ���������� �����
	void ChangePlacement(const D2D1_RECT_F& in_rcPlacement);

	// �������� ������� ��������� ������ (������ ANSI)
	void AddTextLine(const char* in_pcszTextLine);

	// �������� ������� ��������� ������ (������ UNICODE)
	void AddTextLine(const wchar_t* in_pcwszTextLine);

	// �������� �������� (���������� ����� ���������, �� �������� ����� �������� ��������) (������ ANSI)
	UINT AddParameter(const char* in_pcszParamName);

	// �������� �������� (���������� ����� ���������, �� �������� ����� �������� ��������) (������ UNICODE)
	UINT AddParameter(const wchar_t* in_pcwszParamName);

	// ���������� �������� ���������
	void SetParameterValue(UINT in_uiParamHandle, float in_fParameterValue);

	// �������� ����� - ������� ��� ����������� ������ � ���������
	void ClearText();

protected:
	
	// ��������� � ������� ��������� ����� ����� ������ ��������

	CDirect2DTextBlock(CDirect2DTextRenderer* in_pOwner);
	~CDirect2DTextBlock();

private:

	// ������� ������� - �������� CDirect2DTextRenderer
	void CreateResources();

	// ���������� ������� - �������� CDirect2DTextRenderer
	void ReleaseResources();

	// ������� ��������� - �������� CDirect2DTextRenderer
	void Render();

	struct TextBlockPrivate;
	TextBlockPrivate*				_private = nullptr;

	friend class CDirect2DTextRenderer;
};
