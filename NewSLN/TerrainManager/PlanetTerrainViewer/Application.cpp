#include "Application.h"
#include "MouseInput.h"
#include "KeyboardInput.h"
#include "GraphicsContext.h"

static CD3DApplication* g_pApplicationHandle = nullptr;

CD3DApplication* GetApplicationHandle()
{
	return g_pApplicationHandle;
}

bool CD3DApplication::Initialize(const std::wstring& in_wsApplicationName, unsigned int in_uiWidth, unsigned in_uiHeight, float zNear, float zFar, bool in_bFullScreen)
{
	_wsApplicationName = in_wsApplicationName;

	_uiWidth = in_uiWidth;
	_uiHeight = in_uiHeight;

	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindowsAPI(screenWidth, screenHeight);

	// Create the keyboard input object.  This object will be used to handle reading the keyboard input from the user.
	_pKeyboardInput = new CKeyboardInput();

	// Create mouse input object/
	_pMouseInput = new CMouseInput();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	_pGraphicsContext = new CD3DGraphicsContext();

	// Initialize the graphics object.
	result = _pGraphicsContext->Initialize(screenWidth, screenHeight, zNear, zFar, _Hwnd, in_bFullScreen);
	if(!result)
	{
		return false;
	}
	
	return true;
}

bool CD3DApplication::IsFullscreen() const
{
	return _bFullScreen;
}

unsigned int CD3DApplication::GetWindowWidth() const
{
	return _uiWidth;
}

unsigned int CD3DApplication::GetWindowHeight() const
{
	return _uiHeight;
}

float CD3DApplication::LastFrameDeltaTime() const
{
	return _fLastFrameDeltaTime;
}

void CD3DApplication::Shutdown()
{
	// Release the graphics object.
	if(_pGraphicsContext)
	{
		_pGraphicsContext->Shutdown();
		delete _pGraphicsContext;

		_pGraphicsContext = nullptr;
	}

	// Release the input object.
	if(_pKeyboardInput)
	{
		delete _pKeyboardInput;
		_pKeyboardInput = nullptr;
	}

	// Release mouse input object
	if (_pMouseInput)
	{
		delete _pMouseInput;
		_pMouseInput = nullptr;
	}

	// Shutdown the window.
	ShutdownWindowsAPI();
	
	return;
}


void CD3DApplication::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}

	}

	return;
}


bool CD3DApplication::Frame()
{
	// Check if the user pressed escape and wants to exit the application.
	if(_pKeyboardInput->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	_pMouseInput->Frame();

	std::chrono::time_point<std::chrono::steady_clock> thisFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = thisFrameTime - _prevFrameTime;

	double deltaTime = elapsed.count() / 1000.0;

	if (deltaTime > 1)
		deltaTime = 1;

	_fLastFrameDeltaTime = static_cast<float>(deltaTime);

	// Do the frame processing for the graphics object.
	_pGraphicsContext->FrameUpdate();
	
	for (CD3DAppHandler* in_pHandler : _setAppHandlers)
	{
		in_pHandler->OnFrame(_fLastFrameDeltaTime);
	}

	_pGraphicsContext->FrameRender();

	_prevFrameTime = thisFrameTime;

	return true;
}


LRESULT CALLBACK CD3DApplication::MessageHandler(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	switch(umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			_pKeyboardInput->KeyDown((unsigned int)wParam);

			for (CD3DAppHandler* in_pHandler : _setAppHandlers)
			{
				in_pHandler->OnKeyDown((unsigned int)wParam);
			}

			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			_pKeyboardInput->KeyUp((unsigned int)wParam);

			for (CD3DAppHandler* in_pHandler : _setAppHandlers)
			{
				in_pHandler->OnKeyUp((unsigned int)wParam);
			}

			return 0;
		}

		//@{ Mouse buttons handling
		case WM_LBUTTONDOWN:
		{
			_pMouseInput->SetButtonState(CMouseInput::BUTTON_LEFT, true);
			return 0;
		}

		case WM_LBUTTONUP:
		{
			_pMouseInput->SetButtonState(CMouseInput::BUTTON_LEFT, false);
			return 0;
		}

		case WM_MBUTTONDOWN:
		{
			_pMouseInput->SetButtonState(CMouseInput::BUTTON_MIDDLE, true);
			return 0;
		}

		case WM_MBUTTONUP:
		{
			_pMouseInput->SetButtonState(CMouseInput::BUTTON_MIDDLE, false);
			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			_pMouseInput->SetButtonState(CMouseInput::BUTTON_RIGHT, true);
			return 0;
		}

		case WM_RBUTTONUP:
		{
			_pMouseInput->SetButtonState(CMouseInput::BUTTON_RIGHT, false);
			return 0;
		}

		case WM_MOUSEWHEEL:
		{
			_pMouseInput->WheelMoved(GET_WHEEL_DELTA_WPARAM(wParam));
			return 0;
		}
		//@}


		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wParam, lParam);
		}
	}
}


void CD3DApplication::InitializeWindowsAPI(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	g_pApplicationHandle = this;

	// Get the instance of this application.
	_Hinstance = GetModuleHandle(NULL);

	
	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = _Hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = _wsApplicationName.c_str();
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	DWORD dwFlags = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(_bFullScreen)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;

		// Hide the mouse cursor.
		ShowCursor(false);
	}
	else
	{
		screenWidth = 1024;
		screenHeight = 768;

		if (_uiWidth != 0 && _uiHeight != 0)
		{
			screenWidth = _uiWidth;
			screenHeight = _uiHeight;
		}

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

		dwFlags = WS_OVERLAPPEDWINDOW;
	}

	// Create the window with the screen settings and get the handle to it.
	_Hwnd = CreateWindowExW(WS_EX_APPWINDOW, _wsApplicationName.c_str(), _wsApplicationName.c_str(),
		dwFlags, posX, posY, screenWidth, screenHeight, NULL, NULL, _Hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(_Hwnd, SW_SHOW);
	SetForegroundWindow(_Hwnd);
	SetFocus(_Hwnd);

	return;
}


void CD3DApplication::ShutdownWindowsAPI()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(_bFullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(_Hwnd);
	_Hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(_wsApplicationName.c_str(), _Hinstance);
	_Hinstance = NULL;

	// Release the pointer to this class.
	g_pApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return g_pApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}

CD3DGraphicsContext* CD3DApplication::GetGraphicsContext()
{
	return _pGraphicsContext;
}

const CD3DGraphicsContext* CD3DApplication::GetGraphicsContext() const
{
	return _pGraphicsContext;
}

CKeyboardInput* CD3DApplication::GetKeyboardInput()
{
	return _pKeyboardInput;
}

const CKeyboardInput* CD3DApplication::GetKeyboardInput() const
{
	return _pKeyboardInput;
}

CMouseInput * CD3DApplication::GetMouseInput()
{
	return _pMouseInput;
}

const CMouseInput * CD3DApplication::GetMouseInput() const
{
	return _pMouseInput;
}

void CD3DApplication::InstallAppHandler(CD3DAppHandler* in_pFrameHandler)
{
	_setAppHandlers.insert(in_pFrameHandler);
}

void CD3DApplication::UninstallAppHandler(CD3DAppHandler* in_pFrameHandler)
{
	_setAppHandlers.erase(in_pFrameHandler);
}
