#pragma once
#include "InteractObjects.h"

class MenuSection : public InteractObjects
{
private:
	void ResizeDest();
	bool bIsScrollable = false;
	D2D1::Matrix3x2F Transforms = D2D1::Matrix3x2F::Identity();
	D2D1::Matrix3x2F InvTransforms = D2D1::Matrix3x2F::Identity();
	D2D1_POINT_2F TransformedPoint = D2D1::Point2F();
	float ScrollStep = 100.0f;
public:
	MenuSection(Graphics* const graphics, D2D1::Matrix3x2F* const transform, D2D1_RECT_F* const clientrect, D2D1_POINT_2F* const p, D2D1_RECT_F dest, float scrollstep, const wchar_t* label, bool scroll = false) : InteractObjects(graphics, transform, clientrect, p)
	{
		bIsScrollable = scroll;
		ScrollStep = scrollstep;
		SetDest(dest);
		SetLabel(label);
	}
	~MenuSection()
	{
		Unload();
	}
public:
	void Draw() override;	
	void AddChild(InteractObjects* const Iobject) override;
	void Unload() override;
	void WheelUp() override;
	void WheelDown() override;
	bool PointInRect(const D2D1_POINT_2F p) override;
	bool PointInRect() override;
};