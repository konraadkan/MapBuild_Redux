#pragma once
#include "Graphics.h"
#include <vector>

class InteractObjects
{
protected:
	template<typename T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			delete (*ppT);
			(*ppT) = nullptr;
		}
	}
protected:
	D2D1_RECT_F m_Dest = D2D1::RectF();
	D2D1_COLOR_F m_Color = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	D2D1_SIZE_F m_Radius = D2D1::SizeF();
	float fThickness = 1.0f;
	std::vector<InteractObjects*> pChild;
	Graphics* gfx = nullptr;
public:
	InteractObjects(Graphics* const graphics) : gfx(graphics) {}
	~InteractObjects();
	virtual void Interact(const D2D1_POINT_2F p) = 0;
	virtual void Draw() = 0;
public:
	virtual bool PointInRect(const D2D1_POINT_2F p);	
	virtual void SetDest(const D2D1_RECT_F rect);
	virtual void MovePosition(const D2D1_POINT_2F p);
	virtual void SetColor(const D2D1_COLOR_F color);
	virtual void SetThickness(const float thickness);
	virtual void SetRadiusX(const float rad);
	virtual void SetRadiusY(const float rad);
};