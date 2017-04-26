////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsContext.h"

const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

CD3DGraphicsContext::CD3DGraphicsContext()
{
	_pScene = new CD3DScene;
}

CD3DGraphicsContext::~CD3DGraphicsContext()
{
	delete _pScene;
}

bool CD3DGraphicsContext::Initialize(unsigned int in_uiScreenWidth, unsigned int in_uiScreenHeight, HWND in_Hwnd, bool in_bFullscreen)
{
	bool result;


	// Create the Direct3D object.
	_pD3DSystem = new CDirect3DSystem;
	if(!_pD3DSystem)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = _pD3DSystem->Initialize(in_uiScreenWidth, in_uiScreenHeight, VSYNC_ENABLED, in_Hwnd, in_bFullscreen, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(in_Hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	_pTextBlock = _pD3DSystem->GetTextRenderer()->CreateTextBlock();

	_pTextBlock->Init(D2D1::ColorF(D2D1::ColorF::LimeGreen), D2D1::RectF(10, 10, 250, 200), D2D1::ColorF(0.1f, 0.2f, 0.6f, 0.2f), D2D1::ColorF(D2D1::ColorF::Red), 4, 4,
		"Consolas", DWRITE_FONT_WEIGHT_NORMAL, 14.f);

	_uiFpsParam = _pTextBlock->AddParameter(L"FPS");


	return true;
}


void CD3DGraphicsContext::Shutdown()
{
	// Release the D3D object.
	if(_pD3DSystem)
	{
		_pD3DSystem->Shutdown();
		delete _pD3DSystem;

		_pD3DSystem = nullptr;
	}

	return;
}


bool CD3DGraphicsContext::Frame()
{

	if (!_bResourcesCreated)
	{
		_pD3DSystem->CreateResources();

		_bResourcesCreated = true;
	}

	std::chrono::time_point<std::chrono::steady_clock> thisFrameTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = thisFrameTime - _prevFrameTime;
	
	double deltaTime = elapsed.count() / 1000.0;
	
	if (deltaTime > 1)
		deltaTime = 1;

	double fps = 1.0 / deltaTime;

	_pTextBlock->SetParameterValue(_uiFpsParam, (float)fps);

	_prevFrameTime = thisFrameTime;

	_pScene->Update((float)deltaTime);

	Render();

	return true;
}

CDirect3DSystem * CD3DGraphicsContext::GetSystem()
{
	return _pD3DSystem;
}

const CDirect3DSystem * CD3DGraphicsContext::GetSystem() const
{
	return _pD3DSystem;
}


void CD3DGraphicsContext::Render()
{
	// Clear the buffers to begin the scene.
	_pD3DSystem->BeginScene(0.0f, 0.0f, 1.0f, 1.0f);

	_pScene->Render(this);

	// Present the rendered scene to the screen.
	_pD3DSystem->EndScene();
}