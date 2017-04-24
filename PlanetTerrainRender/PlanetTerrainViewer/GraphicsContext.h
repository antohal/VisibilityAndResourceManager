#pragma once

#include <chrono>

#include "CDirect2DTextRenderer.h"
#include "Direct3DSystem.h"
#include "Scene.h"

class CD3DGraphicsContext
{
public:

	bool					Initialize(unsigned int in_uiWidth, unsigned int in_uiHeight, HWND in_Hwnd, bool in_bFullscreen);
	
	void					Shutdown();
	bool					Frame();

	CDirect2DTextBlock*		GetTextBlock() { return _pTextBlock; }
	CD3DScene*				GetScene() { return _pScene; }

private:
	
	void Render();

private:
	
	CDirect3DSystem*		_pD3DSystem = nullptr;
	CDirect2DTextBlock*		_pTextBlock = nullptr;

	CD3DScene*				_pScene = nullptr;

	UINT					_uiFpsParam = -1;

	std::chrono::time_point<std::chrono::steady_clock>	_prevFrameTime;
};
