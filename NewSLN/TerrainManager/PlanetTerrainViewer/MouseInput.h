#pragma once

#include "vecmath.h"

#include <Windows.h>

class CMouseInput
{
public:

	enum Button
	{
		BUTTON_LEFT = 0,
		BUTTON_MIDDLE,
		BUTTON_RIGHT,

		NUM_BUTTONS
	};

	CMouseInput();

	void					Frame();

	void					SetButtonState(Button, bool in_bState);
	bool					GetButtonState(Button) const;

	void					WheelMoved(int iWheel);
	int						GetFrameWheelDelta() const;

	const vm::Vector2df&	GetLastFrameDelta() const;

private:

	vm::Vector2df			_vLastFrameDelta = vm::Vector2df(0.0);
	POINT					_CursorPos;

	bool					_abButtonStates[Button::NUM_BUTTONS];
	int						_iFrameWheelDelta = 0;
	int						_iFrameWheelDeltaSum = 0;
};
