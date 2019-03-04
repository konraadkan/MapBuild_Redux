#include "ruler.h"
#include <string>

void Ruler::DrawLine()
{
	gfx->DrawLine(gfx->GetCompatibleTarget(), mStartPoint, mEndPoint, D2D1::ColorF(1.0f, 0.0f, 0.59f, 0.8f), 10.0f);	
}

void Ruler::DrawDistance()
{
	gfx->OutputText(gfx->GetRenderTarget(), std::to_wstring(static_cast<int>(fDistance)).c_str(), mDest);
}

void Ruler::CalcDistance()
{
	if (fSquareSize.width == 0.0f && fSquareSize.height == 0.0f) return;
	
	float a = (mEndPoint.x - mStartPoint.x) * fSquareSize.width;
	float b = (mEndPoint.y - mStartPoint.y) * fSquareSize.height;
	float dist = sqrt(a * a + b * b);
	dist /= (fSquareSize.width * fSquareSize.height);

	fDistance = dist * 5.0f;
}

const float Ruler::GetDistance()
{
	return fDistance;
}