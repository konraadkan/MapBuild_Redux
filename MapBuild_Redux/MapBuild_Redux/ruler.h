#pragma once
#include"Graphics.h"

class Ruler
{
private:
	D2D1_POINT_2F mStartPoint = D2D1::Point2F();
	D2D1_POINT_2F mEndPoint = D2D1::Point2F();
	D2D1_SIZE_F fSquareSize = D2D1::SizeF();
	D2D1_RECT_F mDest = D2D1::RectF();
	Graphics* const gfx;
	float fDistance = 0.0f;
public:
	Ruler(Graphics* const graphics, const D2D1_SIZE_F SquareSize, D2D1_POINT_2F StartPoint, const D2D1_RECT_F Dest) : mDest(Dest), gfx(graphics), fSquareSize(SquareSize), mStartPoint(StartPoint), mEndPoint(StartPoint) {}
	~Ruler() {}
	Ruler(const Ruler&) = delete;
	Ruler& operator=(const Ruler&) = delete;
public:
	void DrawLine();
	void DrawDistance();
	void SetEndPoint(const D2D1_POINT_2F p) { mEndPoint = p; }
	void CalcDistance();
	const float GetDistance();
};