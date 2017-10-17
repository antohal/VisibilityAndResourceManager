#pragma once

class CKeyboardInput
{
public:

	CKeyboardInput();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int) const;

private:

	bool _keys[256];

};
