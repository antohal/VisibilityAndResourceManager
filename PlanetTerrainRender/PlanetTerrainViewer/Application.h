#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>


#include "KeyboardInput.h"
#include "GraphicsContext.h"


class CD3DApplication
{
public:

	bool			Initialize(const std::wstring& in_wsApplicationName, unsigned int in_uiWidth, unsigned in_uiHeight, bool in_bFullScreen);
	
	void			Shutdown();
	void			Run();

	bool			IsFullscreen() const;
	unsigned int	GetWindowWidth() const;
	unsigned int	GetWindowHeight() const;

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	
	bool Frame();
	void InitializeWindowsAPI(int&, int&);
	void ShutdownWindowsAPI();

private:
	
	std::wstring			_wsApplicationName;
	HINSTANCE				_Hinstance;
	HWND					_Hwnd;

	bool					_bFullScreen = false;
	unsigned int			_uiWidth = 0;
	unsigned int			_uiHeight = 0;

	CKeyboardInput*			_pKeyboardInput = nullptr;
	CD3DGraphicsContext*	_pGraphicsContext = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static CD3DApplication* GetApplicationHandle();
