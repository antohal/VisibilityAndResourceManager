#include "CDirect2DTextRenderer.h"

#include "PlanetCameraController.h"
#include "Application.h"

#include "MouseInput.h"
#include "KeyboardInput.h"
#include "GraphicsContext.h"

#include "vecmath.h"

#include <algorithm>

void CPlanetCameraController::SetCoordinates(const CPlanetCameraController::Coordinates& in_Coordinates)
{
	_coordinates = in_Coordinates;

	_fHeightCommand = _coordinates._height;
}

const CPlanetCameraController::Coordinates&	CPlanetCameraController::GetCoordinates() const
{
	return _coordinates;
}

void CPlanetCameraController::Update(CD3DCamera* in_pCamera, float deltaTime)
{
	if (deltaTime > 0.1f)
		deltaTime = 0.1f;

	const CKeyboardInput* pKeyboardInput = GetApplicationHandle()->GetKeyboardInput();
	const CMouseInput* pMouseInput = GetApplicationHandle()->GetMouseInput();

	double dfDeltaLongitude = deltaTime;
	double dfDeltaLattitude = deltaTime;

	if (pKeyboardInput->IsKeyDown(VK_LEFT))
	{
		_coordinates._longitude += dfDeltaLongitude;
	}

	if (pKeyboardInput->IsKeyDown(VK_RIGHT))
	{
		_coordinates._longitude -= dfDeltaLongitude;
	}

	if (pKeyboardInput->IsKeyDown(VK_UP))
	{
		_coordinates._lattitude += dfDeltaLongitude;
	}

	if (pKeyboardInput->IsKeyDown(VK_DOWN))
	{
		_coordinates._lattitude -= dfDeltaLongitude;
	}

	if (pKeyboardInput->IsKeyDown(VK_PRIOR))
	{
		MoveHeight(deltaTime, 50);
	}

	if (pKeyboardInput->IsKeyDown(VK_NEXT))
	{
		MoveHeight(deltaTime, -50);
	}

	if (_coordinates._lattitude > M_PI*0.5)
		_coordinates._lattitude = M_PI*0.5;

	if (_coordinates._lattitude < -M_PI*0.5)
		_coordinates._lattitude = -M_PI*0.5;

	if (pMouseInput->GetFrameWheelDelta() != 0)
	{
		MoveHeight(deltaTime, pMouseInput->GetFrameWheelDelta());
	}

	vm::Vector2df vMouseDelta = pMouseInput->GetLastFrameDelta();

	if (vm::length(vMouseDelta) > 20.f)
		vMouseDelta = vm::normalize(vMouseDelta) * 20.f;

	if (pMouseInput->GetButtonState(CMouseInput::BUTTON_RIGHT))
	{

		double dfAzDelta = 0.1f*vMouseDelta[0] * deltaTime;
		double dfElDelta = 0.1f*vMouseDelta[1] * deltaTime;

		_coordinates._azimuth += dfAzDelta;
		_coordinates._elevation += dfElDelta;

		if (_coordinates._elevation < -M_PI)
			_coordinates._elevation = -M_PI;

		if (_coordinates._elevation > M_PI)
			_coordinates._elevation = M_PI;
	}

	if (pMouseInput->GetButtonState(CMouseInput::BUTTON_LEFT))
	{
		double heightMin = _fWorldScale * 6357000;
		double heightMax = _fWorldScale * 10000000.0;

		double coeffMin = 0.001;
		double coeffMax = 1;

		double coeff = coeffMax;

		if (_fHeightCommand < heightMin)
		{
			coeff = coeffMin;
			_fHeightCommand = heightMin;
		}
		else
		{
			if (_fHeightCommand > heightMax)
				coeff = coeffMax;
			else
			{
				coeff = vm::lerp((_coordinates._height - heightMin) / (heightMax - heightMin), coeffMin, coeffMax);
			}
		}

		double dfDeltaLon = -coeff*0.1f*vMouseDelta[0] * deltaTime;
		double dfDeltaLat = coeff*0.1f*vMouseDelta[1] * deltaTime;

		_coordinates._lattitude += dfDeltaLat;
		_coordinates._longitude += dfDeltaLon;

		if (_coordinates._lattitude > M_PI*0.5)
			_coordinates._lattitude = M_PI*0.5;

		if (_coordinates._lattitude < -M_PI*0.5)
			_coordinates._lattitude = -M_PI*0.5;
	}

	if (_pTextBlock)
	{
		_pTextBlock->UpdateFormattedTextLine(_uiLatLongHeightParam, _coordinates._lattitude * R2D, _coordinates._longitude * R2D, _coordinates._height);
		_pTextBlock->UpdateFormattedTextLine(_uiAzElParam, _coordinates._azimuth * R2D, _coordinates._elevation * R2D);
	}

	vm::Vector3df vCameraPos = vm::Vector3df(
		//_coordinates._height*cos(_coordinates._lattitude)*cos(_coordinates._longitude),
		//_coordinates._height*cos(_coordinates._lattitude)*sin(_coordinates._longitude),
		//_coordinates._height*sin(_coordinates._lattitude)

		_coordinates._height*cos(_coordinates._lattitude)*sin(_coordinates._longitude),
		_coordinates._height*sin(_coordinates._lattitude),
		-_coordinates._height*cos(_coordinates._lattitude)*cos(_coordinates._longitude)

		);

	vm::Vector3df vVertical = vm::normalize(vCameraPos);
	vm::Vector3df vEast = vm::normalize(vm::cross(vm::Vector3df(0, 1, 0), vVertical));
	vm::Vector3df vNorth = vm::normalize(vm::cross(vVertical, vEast));
	vm::Vector3df vWest = -vEast;

	double elevation = _coordinates._elevation;// -M_PI*0.5;

	vm::Vector3df vCameraLocalDir = vm::normalize(vm::Vector3df(
			/*cos(elevation)*cos(_coordinates._azimuth),
			cos(elevation)*sin(_coordinates._azimuth),
			sin(elevation)*/

		cos(elevation)*sin(_coordinates._azimuth),
		sin(elevation),
		-cos(elevation)*cos(_coordinates._azimuth)
		));

	vm::Vector3df vCameraLocalLeft = vm::normalize(vm::cross(vm::Vector3df(0, 1, 0), vCameraLocalDir));
	vm::Vector3df vCameraLocalUp = vm::normalize(vm::cross(vCameraLocalDir, vCameraLocalLeft));

	vm::Vector3df vCameraGlobalDir = vNorth * vCameraLocalDir[1] + vWest * vCameraLocalDir[0] + vVertical * vCameraLocalDir[2];
	vm::Vector3df vCameraGlobalUp = vNorth * vCameraLocalUp[1] + vWest * vCameraLocalUp[0] + vVertical * vCameraLocalUp[2];

	_vCameraPrevPos = in_pCamera->GetPos():

	in_pCamera->Set(vCameraPos, vCameraGlobalUp, vCameraGlobalDir);

	_coordinates._height += (_fHeightCommand - _coordinates._height)*std::max<float>(1.f, _fScrollCoeff*deltaTime);
}

void CPlanetCameraController::MoveHeight(float deltaTime, int wheelDelta)
{
	double heightMin = _fWorldScale * 6357000;
	double heightMax = _fWorldScale * 10000000.0;

	double coeffMax = _fWorldScale * 600000.0;
	double coeffMin = _fWorldScale * 1000.0;

	double coeff = coeffMax;

	if (_fHeightCommand < heightMin)
	{
		coeff = coeffMin;
		_fHeightCommand = heightMin;
	}
	else
	{
		if (_fHeightCommand > heightMax)
			coeff = coeffMax;
		else
		{
			coeff = vm::lerp((_coordinates._height - heightMin) / (heightMax - heightMin), coeffMin, coeffMax);
		}
	}

	if (wheelDelta > 50)
		wheelDelta = 50;

	if (wheelDelta < -50)
		wheelDelta = -50;

	_fHeightCommand += wheelDelta * deltaTime * coeff;

	if (_fHeightCommand > _dfMaxHeight)
		_fHeightCommand = _dfMaxHeight;

	if (_fHeightCommand < 0)
		_fHeightCommand = 0;
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
