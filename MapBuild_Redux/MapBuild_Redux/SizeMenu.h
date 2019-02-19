#pragma once
#include "Graphics.h"
#include "Shapes.h"
#include "Buttons.h"
#include "CreatureSize.h"

class SizeMenu : public SafeReleaseMemory
{
private:
	void FillRect(D2D1_RECT_F& rect, const D2D1_SIZE_F size)
	{
		rect.right = rect.left + size.width;
		rect.bottom = rect.top + size.height;
	}
private:
	Graphics* gfx = nullptr;
	bool bHidden = true;
	D2D1_SIZE_F mSize = D2D1::SizeF();
	D2D1_RECT_F mDest = D2D1::RectF();
	D2D1_RECT_F mRealDest = D2D1::RectF();
	D2D1_POINT_2F* pMouseCoordinates = nullptr;
	D2D1_ELLIPSE mEllipse;
	D2D1::Matrix3x2F mTransform = D2D1::Matrix3x2F::Identity();
	D2D1::Matrix3x2F mInvTransform = D2D1::Matrix3x2F::Identity();	
	CreatureSize mCreatureSize = CreatureSize::Medium;
	ClassShapes* ShowHide = nullptr;
private:
	void ConfigureShowHide();
	const CreatureSize WstringToCreatureSize(const std::wstring wSize);
public:
	SizeMenu(Graphics* const graphics, const D2D1_RECT_F dest, D2D1_POINT_2F* const p);
	~SizeMenu();
public:
	const bool IsHidden() { return bHidden; }
	const D2D1_SIZE_F GetSize() { return mSize; }
	const D2D1_RECT_F GetDest() { return mDest; }
	const CreatureSize GetSelectedCreatureSize() { return mCreatureSize; }
public:
	void FindSelectedCreatureSize();
	void SetDest(const D2D1_RECT_F dest);
	void SetEllipse(const D2D1_ELLIPSE ellipse) { mEllipse = ellipse; }
	void SetHidden();
	void UnsetHidden();
	void ToggleHidden();
	void UpdateTransform();
	void Draw();
	void BuildMenuCreatureSize();
	void BuildMenuFeet();
	void SetSelectedCreatureSize(const CreatureSize size);
	const bool Interact();
	const bool PointInRect();
	const bool PointInRect(const D2D1_POINT_2F p);
	const bool IsInRealRect();
public:
	std::vector<InteractObjects*> vpChild;
};