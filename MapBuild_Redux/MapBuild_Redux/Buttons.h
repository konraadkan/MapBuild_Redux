#pragma once
#include "InteractObjects.h"

class Buttons : public InteractObjects
{
public:
	Buttons(Graphics* const graphics) : InteractObjects(graphics) {}
	void Interact(D2D1_POINT_2F p) override;
	void Draw() override;
};