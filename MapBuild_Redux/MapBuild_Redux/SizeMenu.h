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
		AoeSize,
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
	MeasurementMenu(const MeasurementMenu&) = delete;
	MeasurementMenu& operator=(const MeasurementMenu&) = delete;
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
	SizeMenu(const SizeMenu&) = delete;
	SizeMenu& operator=(const SizeMenu&) = delete;
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
		pSliderBar = new SliderBar(40.0f, 1.0f, 8.0f, graphics, D2D1::RectF(dest.left + (dest.right - dest.left) * 0.25f, dest.top + (dest.bottom - dest.top) * 0.10f, dest.right - (dest.right - dest.left) * 0.25f, dest.bottom - (dest.bottom - dest.top) * 0.30f),
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
	ThicknessMenu(const ThicknessMenu&) = delete;
	ThicknessMenu& operator=(const ThicknessMenu&) = delete;
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

class AoeSizeMenu : public MeasurementMenu
{
private:
	AoeSpritePointer::AoeTypes CurrentSelectedType = AoeSpritePointer::AoeTypes::Invalid;
	AoeSpritePointer::AoeTypes* pSelectedType;
	SliderBar* pOpacitySlider = nullptr;
	SliderBar* pLengthSlider = nullptr;
	SliderBar* pRadiusSlider = nullptr;
	SliderBar* pWidthSlider = nullptr;
	D2D1_RECT_F OpacityTextBox = D2D1::RectF();
	D2D1_RECT_F LengthTextBox = D2D1::RectF();
	D2D1_RECT_F RadiusTextBox = D2D1::RectF();
	D2D1_RECT_F WidthTextBox = D2D1::RectF();
	D2D1_RECT_F PreviewBox = D2D1::RectF();
	std::vector<InteractObjects*> vpChild;
public:
	AoeSizeMenu(AoeSpritePointer::AoeTypes* const pselectedtype, Graphics* const graphics, const D2D1_RECT_F dest, D2D1_POINT_2F* const p) : pSelectedType(pselectedtype), MeasurementMenu(graphics, dest, p)
	{
		pOpacitySlider = new SliderBar(100.0f, 0.0f, 60.0f, graphics, D2D1::RectF(dest.left + (dest.right - dest.left) * 0.25f, dest.top + 20.0f, dest.right - (dest.right - dest.left) * 0.25f, dest.top + 64.0f),
			D2D1::ColorF(1, 0, 0));
		OpacityTextBox.left = dest.left + 5.0f;
		OpacityTextBox.right = pOpacitySlider->GetDest().left - 5.0f;
		OpacityTextBox.top = pOpacitySlider->GetDest().top;
		OpacityTextBox.bottom = pOpacitySlider->GetDest().bottom;

		pLengthSlider = new SliderBar(200.0f, 5.0f, 15.0f, graphics, D2D1::RectF(pOpacitySlider->GetDest().left, pOpacitySlider->GetDest().bottom + 20.0f, pOpacitySlider->GetDest().right, pOpacitySlider->GetDest().bottom + 20.0f + (pOpacitySlider->GetDest().bottom - pOpacitySlider->GetDest().top)),
			D2D1::ColorF(1, 0, 0));
		LengthTextBox.left = dest.left + 5.0f;
		LengthTextBox.right = pLengthSlider->GetDest().left - 5.0f;
		LengthTextBox.top = pLengthSlider->GetDest().top;
		LengthTextBox.bottom = pLengthSlider->GetDest().bottom;

		pRadiusSlider = new SliderBar(200.0f, 5.0f, 15.0f, graphics, D2D1::RectF(pOpacitySlider->GetDest().left, pOpacitySlider->GetDest().bottom + 20.0f, pOpacitySlider->GetDest().right, pOpacitySlider->GetDest().bottom + 20.0f + (pOpacitySlider->GetDest().bottom - pOpacitySlider->GetDest().top)),
			D2D1::ColorF(1, 0, 0));
		RadiusTextBox.left = dest.left + 5.0f;
		RadiusTextBox.right = pRadiusSlider->GetDest().left - 5.0f;
		RadiusTextBox.top = pRadiusSlider->GetDest().top;
		RadiusTextBox.bottom = pRadiusSlider->GetDest().bottom;

		pWidthSlider = new SliderBar(200.0f, 5.0f, 5.0f, graphics, D2D1::RectF(pLengthSlider->GetDest().left, pLengthSlider->GetDest().bottom + 20.0f, pLengthSlider->GetDest().right, pLengthSlider->GetDest().bottom + 20.0f + (pLengthSlider->GetDest().bottom - pLengthSlider->GetDest().top)),
			D2D1::ColorF(1, 0, 0));
		WidthTextBox.left = dest.left + 5.0f;
		WidthTextBox.right = pWidthSlider->GetDest().left - 5.0f;
		WidthTextBox.top = pWidthSlider->GetDest().top;
		WidthTextBox.bottom = pWidthSlider->GetDest().bottom;

		PreviewBox = D2D1::RectF(pOpacitySlider->GetDest().right + 5.0f, pOpacitySlider->GetDest().top, dest.right - 5.0f, pLengthSlider->GetDest().bottom);
		UpdateTransform();
	}
	AoeSizeMenu(const AoeSizeMenu&) = delete;
	AoeSizeMenu& operator=(const AoeSizeMenu&) = delete;
	~AoeSizeMenu() { SafeDelete(&pOpacitySlider); SafeDelete(&pLengthSlider); SafeDelete(&pWidthSlider); SafeDelete(&pRadiusSlider); }
public:
	void Draw() override;
	const bool Interact() override;
	void DrawConeMenu();
	void DrawCubeMenu();
	void DrawCylinderMenu();
	void DrawLineMenu();
	void DrawSphereMenu();
	void UpdateTransform() override;
	void UpdateSlider();
	void JumpPosition();
	
	const bool PointInOpacitySlider() { return pOpacitySlider->PointOnSlider(mInvTransform.TransformPoint(*pMouseCoordinates)); }
	const bool PointOnOpacitySlideLine() { return pOpacitySlider->PointOnLine(mInvTransform.TransformPoint(*pMouseCoordinates)); }
	const bool PointInLengthSlider() { return pLengthSlider->PointOnSlider(mInvTransform.TransformPoint(*pMouseCoordinates)); }
	const bool PointOnLengthSlideLine() { return pLengthSlider->PointOnLine(mInvTransform.TransformPoint(*pMouseCoordinates)); }
	const bool PointInWidthSlider() { return pWidthSlider->PointOnSlider(mInvTransform.TransformPoint(*pMouseCoordinates)); }
	const bool PointOnWidthSlideLine() { return pWidthSlider->PointOnLine(mInvTransform.TransformPoint(*pMouseCoordinates)); }
	const bool PointInRadiusSlider() { return pRadiusSlider->PointOnSlider(mInvTransform.TransformPoint(*pMouseCoordinates)); }
	const bool PointOnRadiusSlideLine() { return pRadiusSlider->PointOnLine(mInvTransform.TransformPoint(*pMouseCoordinates)); }

	void SetOpacitySelected() { pOpacitySlider->SetSelected(); }
	void UnsetOpacitySelected() { pOpacitySlider->UnsetSelected(); }
	void SetLengthSelected() { pLengthSlider->SetSelected(); }
	void UnsetLengthSelected() { pLengthSlider->UnsetSelected(); }
	void SetWidthSelected() { pWidthSlider->SetSelected(); }
	void UnsetWidthSelected() { pWidthSlider->UnsetSelected(); }
	void SetRadiusSelected() { pRadiusSlider->SetSelected(); }
	void UnsetRadiusSelected() { pRadiusSlider->UnsetSelected(); }

	const bool OpacitySelected() { return pOpacitySlider->IsSelected(); }
	const bool LengthSelected() { return pLengthSlider->IsSelected(); }
	const bool WidthSelected() { return pWidthSlider->IsSelected(); }
	const bool RadiusSelected() { return pRadiusSlider->IsSelected(); }
	const bool IsSelected() { return WidthSelected() || RadiusSelected() || LengthSelected() || OpacitySelected(); }

	const float GetOpacity() { return pOpacitySlider->GetSize() * 0.01f; }
	const float GetLength() { return pLengthSlider->GetSize() / 5.0f; }
	const float GetWidth() { return pWidthSlider->GetSize() / 5.0f; }
	const float GetRadius() { return pRadiusSlider->GetSize() / 5.0f; }
};