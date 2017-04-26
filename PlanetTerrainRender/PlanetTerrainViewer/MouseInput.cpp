#include "MouseInput.h"

CMouseInput::CMouseInput()
{
	if (!GetCursorPos(&_CursorPos))
	{
		_CursorPos.x = 0;
		_CursorPos.y = 0;
	}

	for (int i = 0; i < Button::NUM_BUTTONS; i++)
		_abButtonStates[i] = false;
}

void CMouseInput::Frame()
{
	POINT currentCursorPos;
	if (GetCursorPos(&currentCursorPos))
	{
		_vLastFrameDelta.v[0] = (double)currentCursorPos.x - (double)_CursorPos.x;
		_vLastFrameDelta.v[1] = (double)currentCursorPos.y - (double)_CursorPos.y;

		_CursorPos = currentCursorPos;
	}

	// save last frame wheel delta
	_iFrameWheelDelta = _iFrameWheelDeltaSum;
	_iFrameWheelDeltaSum = 0;
}

void CMouseInput::SetButtonState(Button in_Button, bool in_bState)
{
	_abButtonStates[in_Button] = in_bState;
}

bool CMouseInput::GetButtonState(Button in_Button) const
{
	return _abButtonStates[in_Button];
}

void CMouseInput::WheelMoved(int iWheel)
{
	_iFrameWheelDeltaSum += iWheel;
}

int CMouseInput::GetFrameWheelDelta() const
{
	return _iFrameWheelDelta;
}

const vm::Vector2df & CMouseInput::GetLastFrameDelta() const
{
	return _vLastFrameDelta;
}
