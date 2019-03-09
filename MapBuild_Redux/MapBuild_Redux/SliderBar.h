#pragma once
#include "Graphics.h"

class SliderBar
{
private:
	struct details
	{
		D2D1_POINT_2F LeftBar[2];
		D2D1_POINT_2F RightBar[2];
		D2D1_POINT_2F CenterLine[2];
		float fDistance;
		float fTickDistance;
		float fTickSize;
		float fSliderSize;
		float fSliderSizeHalf;
		float fValueIncrease;
		float fMaxSize;
		void CalcDistance()
		{
			fDistance = RightBar[0].x - LeftBar[0].x;
			fTickDistance = fDistance * 0.10f;
			fTickSize = fTickDistance * 0.15f;
			fSliderSize = 6.0f;
			fSliderSizeHalf = fSliderSize * 0.5f;
			fValueIncrease = (fMaxSize - 1.0f) / fDistance;
		}
	} LineDimensions;
private:
	bool bHidden = false;
	bool bSelected = false;
	Graphics* gfx = nullptr;
	D2D1_RECT_F mDest = D2D1::RectF();
	D2D1_SIZE_F mSize = D2D1::SizeF();
	float fDestOutlineThickness = 2.0f;
	float fMaxSize = 40.0f;
	float fMinSize = 1.0f;
	float fCurrentSize = 8.0f;
	D2D1_COLOR_F mDestOutlineColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	D2D1_COLOR_F mBackgroundColor = D2D1::ColorF(0.75f, 0.75f, 0.75f);
	D2D1_COLOR_F mLineColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	D2D1_COLOR_F mIOColor = D2D1::ColorF(1.0f, 1.0f, 1.0f);
	D2D1_COLOR_F mIOOutlineColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	D2D1_RECT_F mSliderPosition = D2D1::RectF();
public:
	SliderBar(const float MaxSize, const float MinSize, const float CurrentSize, Graphics* const graphics, D2D1_RECT_F dest, D2D1_COLOR_F backgroundcolor = D2D1::ColorF(0.75f, 0.75f, 0.75f), D2D1_COLOR_F linecolor = D2D1::ColorF(0.0f, 0.0f, 0.0f),
		D2D1_COLOR_F iocolor = D2D1::ColorF(1.0f, 1.0f, 1.0f), D2D1_COLOR_F iooutlinecolor = D2D1::ColorF(0.0f, 0.0f, 0.0f), D2D1_COLOR_F destoutlinecolor = D2D1::ColorF(0.0f, 0.0f, 0.0f), float outlinethickness = 2.0f) :
		fMaxSize(MaxSize), fMinSize(MinSize), fCurrentSize(CurrentSize), gfx(graphics), mDest(dest), mBackgroundColor(backgroundcolor), mLineColor(linecolor), mIOColor(iocolor), mIOOutlineColor(iooutlinecolor), mDestOutlineColor(destoutlinecolor), fDestOutlineThickness(outlinethickness) 
	{
		mSize = D2D1::SizeF(mDest.right - mDest.left, mDest.bottom - mDest.top);
		LineDimensions.LeftBar[0] = D2D1::Point2F(mDest.left + 10.0f, mDest.top + (mDest.bottom - mDest.top) * 0.25f);
		LineDimensions.LeftBar[1] = D2D1::Point2F(mDest.left + 10.0f, mDest.top + (mDest.bottom - mDest.top) * 0.75f);
		LineDimensions.RightBar[0] = D2D1::Point2F(mDest.right - 10.0f, mDest.top + (mDest.bottom - mDest.top) * 0.25f);
		LineDimensions.RightBar[1] = D2D1::Point2F(mDest.right - 10.0f, mDest.top + (mDest.bottom - mDest.top) * 0.75f);
		LineDimensions.CenterLine[0] = D2D1::Point2F(mDest.left + 10.0f, mDest.top + (mDest.bottom - mDest.top) * 0.5f);
		LineDimensions.CenterLine[1] = D2D1::Point2F(mDest.right - 10.0f, mDest.top + (mDest.bottom - mDest.top) * 0.5f);
		LineDimensions.fMaxSize = MaxSize;
		LineDimensions.CalcDistance();
		mSliderPosition = D2D1::RectF(LineDimensions.LeftBar[0].x - LineDimensions.fSliderSizeHalf + (1.0f / LineDimensions.fValueIncrease) * fCurrentSize, LineDimensions.LeftBar[0].y, LineDimensions.LeftBar[0].x + LineDimensions.fSliderSizeHalf + (1.0f / LineDimensions.fValueIncrease) * fCurrentSize, LineDimensions.LeftBar[1].y);
	}
	~SliderBar() {}
	SliderBar(const SliderBar&) = delete;
	SliderBar& operator=(const SliderBar&) = delete;
	void Draw();
	void SetSelected() { bSelected = true; }
	void UnsetSelected() { bSelected = false; }
	void SetHidden() { bHidden = true; }
	void UnsetHidden() { bHidden = false; }
	void ToggleHidden() { bHidden ^= true; }
	void UpdateSliderPosition(const D2D1_POINT_2F p);
	void UpdateSize();
	void MoveSlider(const float x);
	void ShiftSlider(const float x);
	const bool IsSelected() { return bSelected; }
	const bool IsHidden() { return bHidden; }
	const bool PointOnSlider(const D2D1_POINT_2F p);
	const bool PointOnLine(const D2D1_POINT_2F p);
	const float GetSize() { return fCurrentSize; }
	const D2D1_RECT_F GetSliderPosition() { return mSliderPosition; }
	const D2D1_RECT_F GetDest() { return mDest; }
};