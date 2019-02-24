#include "SliderBar.h"

void SliderBar::Draw()
{
	if (!gfx) return;
	if (bHidden) return;

	gfx->FillRect(gfx->GetCompatibleTarget(), mDest, mBackgroundColor);

	gfx->DrawLine(gfx->GetCompatibleTarget(), LineDimensions.LeftBar[0], LineDimensions.LeftBar[1]);
	gfx->DrawLine(gfx->GetCompatibleTarget(), LineDimensions.RightBar[0], LineDimensions.RightBar[1]);
	gfx->DrawLine(gfx->GetCompatibleTarget(), LineDimensions.CenterLine[0], LineDimensions.CenterLine[1]);

	for (float i = LineDimensions.fTickDistance; i < LineDimensions.fDistance; i += LineDimensions.fTickDistance)
	{
		gfx->DrawLine(gfx->GetCompatibleTarget(), D2D1::Point2F(LineDimensions.LeftBar[0].x + i, LineDimensions.CenterLine[0].y - LineDimensions.fTickSize),
			D2D1::Point2F(LineDimensions.LeftBar[0].x + i, LineDimensions.CenterLine[0].y + LineDimensions.fTickSize));
	}
	gfx->FillRect(gfx->GetCompatibleTarget(), mSliderPosition, mIOColor);
	gfx->DrawRect(gfx->GetCompatibleTarget(), mSliderPosition, mIOOutlineColor);

	gfx->DrawRect(gfx->GetCompatibleTarget(), mDest, mDestOutlineColor, fDestOutlineThickness);
}

void SliderBar::UpdateSliderPosition(const D2D1_POINT_2F p)
{
	if (p.x < LineDimensions.LeftBar[0].x - LineDimensions.fSliderSizeHalf)
	{
		MoveSlider(LineDimensions.LeftBar[0].x - LineDimensions.fSliderSizeHalf);
		UpdateSize();
	}
	else if (p.x > LineDimensions.RightBar[0].x - LineDimensions.fSliderSizeHalf)
	{
		MoveSlider(LineDimensions.RightBar[0].x - LineDimensions.fSliderSizeHalf);
		UpdateSize();
	}
	else
	{
		MoveSlider(p.x);
		UpdateSize();
	}
}

void SliderBar::MoveSlider(const float x)
{
	mSliderPosition.left = x;
	mSliderPosition.right = mSliderPosition.left + LineDimensions.fSliderSize;
}

void SliderBar::ShiftSlider(const float x)
{
	mSliderPosition.left += x;
	if (mSliderPosition.left < LineDimensions.LeftBar[0].x - LineDimensions.fSliderSizeHalf)
		mSliderPosition.left = LineDimensions.LeftBar[0].x - LineDimensions.fSliderSizeHalf;
	else if (mSliderPosition.left > LineDimensions.RightBar[0].x - LineDimensions.fSliderSizeHalf)
		mSliderPosition.left = LineDimensions.RightBar[0].x - LineDimensions.fSliderSizeHalf;
	mSliderPosition.right = mSliderPosition.left + LineDimensions.fSliderSize;
}

const bool SliderBar::PointOnSlider(const D2D1_POINT_2F p)
{
	return (p.x > mSliderPosition.left && p.x < mSliderPosition.right && p.y > mSliderPosition.top && p.y < mSliderPosition.bottom);
}

const bool SliderBar::PointOnLine(const D2D1_POINT_2F p)
{
	return (p.x > mDest.left && p.x < mDest.right && p.y > mDest.top && p.y < mDest.bottom);
}

void SliderBar::UpdateSize()
{
	fCurrentSize = fMinSize + LineDimensions.fValueIncrease * (mSliderPosition.left - LineDimensions.LeftBar[0].x);
	if (fCurrentSize < fMinSize) fCurrentSize = fMinSize;
	if (fCurrentSize > fMaxSize) fCurrentSize = fMaxSize;
}