#pragma once
#include "Buttons.h"

class SideMenu : public Buttons
{
public:
	SideMenu(Graphics* const graphics) : Buttons(graphics) {}
	SideMenu(const D2D1_RECT_F targetDest, Graphics* const graphics) : Buttons(graphics)
	{
		SetDest(targetDest);
		SetColor(D2D1::ColorF(0.75f, 0.80f, 0.82f));
	}
};