#pragma once
#include "Level.h"

class Controller
{
	Controller() { bLoading = true; }
public:
	static Keyboard m_Keyboard;
	static Level* CurrentLevel;
	static bool bLoading;
	static void Render();
	static void Update(double dDelta);
	static void SwitchLevel(Level* const level);
	static void Cleanup();
};