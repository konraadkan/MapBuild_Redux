#pragma once
#include "InteractObjects.h"
#include "Pieces.h"

class Buttons : public InteractObjects
{
protected:
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
	const bool Interact(const D2D1_POINT_2F p) override;
	const bool Interact() override;
	void Draw() override;
	const bool IsSelected() override { return bSelected; }
	void SetIsSelected() override { bSelected = true; }
	void UnsetIsSelected() override { bSelected = false; }
};

class RoomLayerBox : public Buttons
{
private:
	bool bRoom = true;
	std::vector<bool>* pvVisibleRoom = nullptr;
	std::vector< std::vector<bool>>* pvVisibleLayer = nullptr;
	size_t uRoomNumber = 0;
	size_t uLayerNumber = 0;
public:
	RoomLayerBox(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), size_t visibleroomnumber = 0, size_t visiblelayernumber = 0, std::vector<bool>* const visibleroom = nullptr, std::vector< std::vector<bool>>* const visiblelayer = nullptr, bool room = true, DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) :  bRoom(room), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) 
	{
		pvVisibleLayer = visiblelayer;
		pvVisibleRoom = visibleroom;
		uRoomNumber = visibleroomnumber;
		uLayerNumber = visiblelayernumber;
	}
	const bool Interact() override;
	const bool Interact(const D2D1_POINT_2F p) override;
};

class Checkbox : public Buttons
{
private:
	bool bRoom = true;
public:
	Checkbox(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), bool room = true, DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : bRoom(room), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	const bool Interact() override;
};

class AddItem : public Buttons
{
private:
	bool bRoom = true;
public:
	AddItem(bool room, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : bRoom(room), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	const bool Interact(const D2D1_POINT_2F p) override;
};

class TypeButtons : public Buttons
{
public:
	TypeButtons(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	const bool Interact(const D2D1_POINT_2F p) override;
};

class SubsectionButtons : public Buttons
{
public:
	SubsectionButtons(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	const bool Interact(const D2D1_POINT_2F p) override;
};

class SpriteItemButtons : public Buttons
{
private:
	PiecesW* pPiecesW = nullptr;
	SpritePointer** ppSelectedSprite = nullptr;
	D2D1::Matrix3x2F* pMatrix = nullptr;
public:
	SpriteItemButtons(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, PiecesW* const piecew = nullptr, SpritePointer** const ppsp = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : pPiecesW(piecew), ppSelectedSprite(ppsp), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	void SetMatrixPointer(D2D1::Matrix3x2F* p) { pMatrix = p; }
	const bool Interact(const D2D1_POINT_2F p) override;
	void Draw() override;
	bool PointInRect() override;
	bool PointInRect(const D2D1_POINT_2F p) override;	
};