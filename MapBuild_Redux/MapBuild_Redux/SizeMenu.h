#pragma once
#include "Graphics.h"
#include "Shapes.h"
#include "Buttons.h"
#include "CreatureSize.h"
#include "SliderBar.h"

class MeasurementMenu : public SafeReleaseMemory
{
public:
	enum class SizeMenuType
	{
		CreatureSize,
		ThicknessSize,
		DistanceSize,
		Error
	};
protected:
	virtual void FillRect(D2D1_RECT_F& rect, const D2D1_SIZE_F size)
	{
		rect.right = rect.left + size.width;
		rect.bottom = rect.top + size.height;
	}
protected:
	Graphics* gfx = nullptr;
	D2D1_RECT_F mDest = D2D1::RectF();
	D2D1_RECT_F mRealDest = D2D1::RectF();
	D2D1_SIZE_F mSize = D2D1::SizeF();
	D2D1_POINT_2F* const pMouseCoordinates;
	bool bHidden = true;
	D2D1::Matrix3x2F mTransform = D2D1::Matrix3x2F::Identity();
	D2D1::Matrix3x2F mInvTransform = D2D1::Matrix3x2F::Identity();
	D2D1_ELLIPSE mEllipse;
	ClassShapes* ShowHide = nullptr;
protected:
	virtual void ConfigureShowHide();
public:
	MeasurementMenu(Graphics* const graphics, const D2D1_RECT_F dest, D2D1_POINT_2F* const p) : gfx(graphics), pMouseCoordinates(p)
	{
		mSize = D2D1::SizeF(fabs(dest.right - dest.left), fabs(dest.bottom - dest.top));
		SetDest(dest);
		ConfigureShowHide();
		UpdateTransform();
	}
	~MeasurementMenu() {}
public:
	virtual void SetDest(const D2D1_RECT_F dest);
	virtual void UpdateTransform();
	virtual void SetEllipse(const D2D1_ELLIPSE ellipse) { mEllipse = ellipse; }
	virtual void SetHidden();
	virtual void UnsetHidden();
	virtual void ToggleHidden();
	virtual void Draw() = 0;
	virtual const bool Interact() = 0;
	virtual const bool PointInRect();
	virtual const bool PointInRect(const D2D1_POINT_2F p);
	virtual const bool IsInRealRect();
public:
	virtual const bool IsHidden() { return bHidden; }
	virtual const D2D1_SIZE_F GetSize() { return mSize; }
	virtual const D2D1_RECT_F GetDest() { return mDest; }
};

class SizeMenu : public MeasurementMenu
{
private:
	CreatureSize mCreatureSize = CreatureSize::Medium;
	const CreatureSize WstringToCreatureSize(const std::wstring wSize);
public:
	SizeMenu(Graphics* const graphics, const D2D1_RECT_F dest, D2D1_POINT_2F* const p) : MeasurementMenu(graphics, dest, p) {}
	~SizeMenu();
public:
	void Draw() override;
	const bool Interact() override;

	const CreatureSize GetSelectedCreatureSize() { return mCreatureSize; }
	void FindSelectedCreatureSize();	
	void BuildMenuCreatureSize();
	void SetSelectedCreatureSize(const CreatureSize size);	
public:
	std::vector<InteractObjects*> vpChild;
};

class ThicknessMenu : public MeasurementMenu
{
protected:
	D2D1_RECT_F mTextDest = D2D1::RectF();
	D2D1_RECT_F mPreviewDest = D2D1::RectF();
	float fPreviewCirlceRadius = 0.0f;
	D2D1_POINT_2F mPreviewCenter = D2D1::Point2F();
	SliderBar* pSliderBar = nullptr;
public:
	ThicknessMenu(Graphics* const graphics, const D2D1_RECT_F dest, D2D1_POINT_2F* const p) : MeasurementMenu(graphics, dest, p)
	{
		pSliderBar = new SliderBar(graphics, D2D1::RectF(dest.left + (dest.right - dest.left) * 0.25f, dest.top + (dest.bottom - dest.top) * 0.10f, dest.right - (dest.right - dest.left) * 0.25f, dest.bottom - (dest.bottom - dest.top) * 0.30f),
			D2D1::ColorF(1, 0, 0));
		mTextDest.left = dest.left + 5.0f;
		mTextDest.right = pSliderBar->GetDest().left - 5.0f;
		mTextDest.top = pSliderBar->GetDest().top;
		mTextDest.bottom = pSliderBar->GetDest().bottom;
		mPreviewDest.left = pSliderBar->GetDest().right + 5.0f;
		mPreviewDest.right = mDest.right - 5.0f;
		mPreviewDest.top = pSliderBar->GetDest().top;
		mPreviewDest.bottom = pSliderBar->GetDest().bottom;
		fPreviewCirlceRadius = pSliderBar->GetSize() * 0.5f;
		mPreviewCenter.x = mPreviewDest.left + (mPreviewDest.right - mPreviewDest.left) * 0.5f;
		mPreviewCenter.y = mPreviewDest.top + (mPreviewDest.bottom - mPreviewDest.top) * 0.5f;
	}
	~ThicknessMenu();
public:
	void Draw() override;
	void SetSelected() { pSliderBar->SetSelected(); }
	void UnsetSelected() { pSliderBar->UnsetSelected(); }
	void UpdateSlider();
	void JumpPosition();
	void SetPreviewRadius(const float r) { fPreviewCirlceRadius = r; }
	const float CalcRadius() { return pSliderBar->GetSize() * 0.5f; }
	const bool Interact() override;
	const bool PointInSlider() { return pSliderBar->PointOnSlider(*pMouseCoordinates); }
	const bool PointOnSlideLine() { return pSliderBar->PointOnLine(*pMouseCoordinates); }
	const bool IsSelected() { return pSliderBar->IsSelected(); }
public:
	const float GetSelectedThickness() { return pSliderBar->GetSize(); }
	std::vector<InteractObjects*> vpChild;
};