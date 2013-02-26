#include "InputClass.h"

InputClass::InputClass(void)
{
}
InputClass::InputClass(const InputClass&)
{
}

InputClass::~InputClass()
{
}

void InputClass::Initialize()
{
	int i = 0;
	for (i = 0; i < 256; ++i)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	if (input < 0 || input >= 256)
		return;

	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	if (input < 0 || input >= 256)
		return;

	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int input)
{
	if (input < 0 || input >= 256)
		return false;

	return m_keys[input];

}
