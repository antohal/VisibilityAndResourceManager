#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>

class CKeyboardInput;
class CD3DGraphicsContext;
class CMouseInput;

class CD3DApplication
{
public:

	bool						Initialize(const std::wstring& in_wsApplicationName, unsigned int in_uiWidth, unsigned in_uiHeight, float zNear, float zFar, bool in_bFullScreen);
	
	void						Shutdown();
	void						Run();

	bool						IsFullscreen() const;
	unsigned int				GetWindowWidth() const;
	unsigned int				GetWindowHeight() const;

	LRESULT CALLBACK			MessageHandler(HWND, UINT, WPARAM, LPARAM);

	CD3DGraphicsContext*		GetGraphicsContext();
	const CD3DGraphicsContext*	GetGraphicsContext() const;

	CKeyboardInput*				GetKeyboardInput();
	const CKeyboardInput*		GetKeyboardInput() const;

	CMouseInput*				GetMouseInput();
	const CMouseInput*			GetMouseInput() const;

private:
	
	bool						Frame();
	void						InitializeWindowsAPI(int&, int&);
	void						ShutdownWindowsAPI();

private:
	
	std::wstring				_wsApplicationName;
	HINSTANCE					_Hinstance;
	HWND						_Hwnd;

	bool						_bFullScreen = false;
	unsigned int				_uiWidth = 0;
	unsigned int				_uiHeight = 0;

	CKeyboardInput*				_pKeyboardInput = nullptr;
	CMouseInput*				_pMouseInput = nullptr;
	CD3DGraphicsContext*		_pGraphicsContext = nullptr;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

CD3DApplication* GetApplicationHandle();
