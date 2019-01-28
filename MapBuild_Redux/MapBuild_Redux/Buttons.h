#pragma once
#include "InteractObjects.h"

class Buttons : public InteractObjects
{
public:
	Buttons(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area) : InteractObjects(graphics, Transform, area) {}
	void Interact(D2D1_POINT_2F p) override;
	void Draw() override;
};