#include "TurnCounter.h"

TurnCounter::TurnCounter(Graphics* const graphics, const D2D1_ELLIPSE ellipse, uint32_t* const TurnNumber, const D2D1_COLOR_F color, const float thickness) : gfx(graphics), mEllpseInfo(ellipse), mFillColor(color), fThickness(thickness), pTurnNumber(TurnNumber)
{
	mStringRect.left = ellipse.point.x;
	mStringRect.top = ellipse.point.y;
	mStringRect.right = ellipse.point.x + cosf(0.785398f) * ellipse.radiusX;
	mStringRect.bottom = ellipse.point.y + sinf(0.785398f) * ellipse.radiusY;
	ShowTransform = D2D1::Matrix3x2F::Identity();
	HideTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, -ellipse.radiusY * 0.8f));
	SetHidden();
}

void TurnCounter::Draw()
{
	if (IsHidden()) return;
	D2D1::Matrix3x2F oMatrix;
	gfx->GetCompatibleTarget()->GetTransform(&oMatrix);

	gfx->GetCompatibleTarget()->SetTransform(bShow ? ShowTransform : HideTransform);
	gfx->FillEllipse(gfx->GetCompatibleTarget(), mEllpseInfo, mFillColor);
	gfx->DrawEllipse(gfx->GetCompatibleTarget(), mEllpseInfo, D2D1::ColorF(0, 0, 0), fThickness);
	gfx->OutputText(gfx->GetCompatibleTarget(), wCounterString.c_str(), mStringRect, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	gfx->GetCompatibleTarget()->SetTransform(oMatrix);
}

void TurnCounter::Interact()
{
	//ToggleHidden();
}

const bool TurnCounter::PointInEllipse(const D2D1_POINT_2F p)
{
	D2D1::Matrix3x2F cTransform = bShow ? ShowTransform : HideTransform;
	D2D1_POINT_2F tp = cTransform.TransformPoint(p);
	return (p.x > mEllpseInfo.point.x - mEllpseInfo.radiusX && p.x < mEllpseInfo.point.x + mEllpseInfo.radiusX
		&& p.y > mEllpseInfo.point.y - mEllpseInfo.radiusY && p.y < mEllpseInfo.point.y + mEllpseInfo.radiusY);
}

void TurnCounter::Update()
{
	wCounterString = std::to_wstring(*pTurnNumber);
}