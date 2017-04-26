#include "CDirect2DTextRenderer.h"

#include "PlanetCameraController.h"
#include "Application.h"

#include "MouseInput.h"
#include "KeyboardInput.h"
#include "GraphicsContext.h"

#include "vecmath.h"

void CPlanetCameraController::SetCoordinates(const CPlanetCameraController::Coordinates& in_Coordinates)
{
	_coordinates = in_Coordinates;
}

const CPlanetCameraController::Coordinates&	CPlanetCameraController::GetCoordinates() const
{
	return _coordinates;
}

void CPlanetCameraController::Update(CD3DCamera* in_pCamera, float deltaTime)
{
	const CKeyboardInput* pKeyboardInput = GetApplicationHandle()->GetKeyboardInput();
	const CMouseInput* pMouseInput = GetApplicationHandle()->GetMouseInput();

	if (pMouseInput->GetButtonState(CMouseInput::BUTTON_RIGHT))
	{

		double dfAzDelta = pMouseInput->GetLastFrameDelta()[0] * deltaTime;
		double dfElDelta = pMouseInput->GetLastFrameDelta()[1] * deltaTime;

		_coordinates._azimuth += dfAzDelta;
		_coordinates._elevation += dfElDelta;

		if (_coordinates._elevation < 0)
			_coordinates._elevation = 0;

		if (_coordinates._elevation > M_PI)
			_coordinates._elevation = M_PI;
	}

	if (_pTextBlock)
	{
		_pTextBlock->UpdateFormattedTextLine(_uiLatLongHeightParam, _coordinates._lattitude * R2D, _coordinates._longitude * R2D, _coordinates._height);
		_pTextBlock->UpdateFormattedTextLine(_uiAzElParam, _coordinates._azimuth * R2D, _coordinates._elevation * R2D);
	}
}

void CPlanetCameraController::CreateDebugTextBlock()
{
	_pTextBlock = GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetTextRenderer()->CreateTextBlock();

	_pTextBlock->Init(D2D1::ColorF(D2D1::ColorF::LimeGreen), D2D1::RectF(10, 250, 250, 400), D2D1::ColorF(0.1f, 0.2f, 0.6f, 0.2f), D2D1::ColorF(D2D1::ColorF::Red), 4, 4,
		"Consolas", DWRITE_FONT_WEIGHT_NORMAL, 12.f);

	_pTextBlock->AddTextLine(L"CPlanetCameraController:");

	_uiLatLongHeightParam = _pTextBlock->AddFormattedTextLine(L"Lattitude = %.2f, Longitude = %.2f, Height = %.2f");
	_uiAzElParam = _pTextBlock->AddFormattedTextLine(L"Azimuth = %.1f, Elevation = %.1f");
}

void CPlanetCameraController::ShowDebugTextBlock(bool in_bShow)
{
	if (!_pTextBlock)
		return;

	_pTextBlock->SetVisible(in_bShow);
}

void CPlanetCameraController::HideDebugTextBlock()
{
	if (!_pTextBlock)
		return;

	_pTextBlock->SetVisible(false);
}

void CPlanetCameraController::DeleteDebugTextBlock()
{
	if (!_pTextBlock)
		return;

	GetApplicationHandle()->GetGraphicsContext()->GetSystem()->GetTextRenderer()->DeleteTextBlock(_pTextBlock);

	_pTextBlock = nullptr;
}
