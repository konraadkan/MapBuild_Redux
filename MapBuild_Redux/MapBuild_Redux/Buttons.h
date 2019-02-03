#pragma once
#include "InteractObjects.h"

class Buttons : public InteractObjects
{
private:
	D2D1_COLOR_F TextColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	D2D1_RECT_F TextRect = D2D1::RectF();	
	bool bSelected = false;
	bool bEnableSelection = false;
public:
	Buttons(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : InteractObjects(graphics, Transform, area, p)
	{
		bEnableSelection = enableselection;
		bSelected = selected;
		SetLabel(text ? text : L"");
		m_Alignment = talign;
		m_pAlignment = palign;
		fThickness = 2.0f;
		SetHighlightColor(highlight);
		SetDest(dest);
		TextRect.top = dest.top;
		TextRect.bottom = dest.bottom;
		TextRect.left = dest.left + 1.0f;
		TextRect.right = dest.right - 1.0f;
		SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f));
		SetRadiusX((dest.right - dest.left) * 0.15f);
		SetRadiusY((dest.bottom - dest.top) * 0.15f);
		pParent = parent;
		SetSelectionColor(D2D1::ColorF(1, 0, 0, 0.5f));
	}
	void Interact(D2D1_POINT_2F p) override;
	void Interact() override;
	void Draw() override;
	const bool IsSelected() override { return bSelected; }
};