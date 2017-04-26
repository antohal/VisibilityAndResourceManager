////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "KeyboardInput.h"

CKeyboardInput::CKeyboardInput()
{
	for(int i = 0; i < 256; i++)
	{
		_keys[i] = false;
	}

}


void CKeyboardInput::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	_keys[input] = true;
	return;
}


void CKeyboardInput::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	_keys[input] = false;
	return;
}


bool CKeyboardInput::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return _keys[key];
}
