#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <set>
#include <string>

#include <chrono>

class CKeyboardInput;
class CD3DGraphicsContext;
class CMouseInput;

class CD3DAppHandler
{
public:

	virtual void				OnFrame(float in_fFrameTime) {};
	virtual void				OnKeyDown(unsigned int in_wKey) {};
	virtual void				OnKeyUp(unsigned int in_wKey) {};
};

class CD3DApplication
{
public:

	bool						Initialize(const std::wstring& in_wsApplicationName, unsigned int in_uiWidth, unsigned in_uiHeight, float zNear, float zFar, bool in_bFullScreen);
	
	void						Shutdown();
	void						Run();

	bool						IsFullscreen() const;
	unsigned int				GetWindowWidth() const;
	unsigned int				GetWindowHeight() const;

	float						LastFrameDeltaTime() const;

	LRESULT CALLBACK			MessageHandler(HWND, UINT, WPARAM, LPARAM);

	CD3DGraphicsContext*		GetGraphicsContext();
	const CD3DGraphicsContext*	GetGraphicsContext() const;

	CKeyboardInput*				GetKeyboardInput();
	const CKeyboardInput*		GetKeyboardInput() const;

	CMouseInput*				GetMouseInput();
	const CMouseInput*			GetMouseInput() const;

	void						InstallAppHandler(CD3DAppHandler* in_pFrameHandler);
	void						UninstallAppHandler(CD3DAppHandler* in_pFrameHandler);

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

	std::set<CD3DAppHandler*>	_setAppHandlers;

	std::chrono::time_point<std::chrono::steady_clock>	_prevFrameTime;
	float						_fLastFrameDeltaTime = 0;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

CD3DApplication* GetApplicationHandle();
