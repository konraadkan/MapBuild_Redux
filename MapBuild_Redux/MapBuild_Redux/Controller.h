#pragma once
#include "Level.h"

class Controller
{
	Controller() { bLoading = true; }
	~Controller() {}
	Controller(const Controller&) = delete;
	Controller& operator=(const Controller&) = delete;
public:
	static Keyboard m_Keyboard;
	static Mouse m_Mouse;
	static Level* CurrentLevel;
	static bool bLoading;
	static void Render();
	static void Update(double dDelta);
	static void SwitchLevel(Level* const level);
	static void Cleanup();
};