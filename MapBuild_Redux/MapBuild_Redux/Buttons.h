#pragma once
#include "InteractObjects.h"
#include "Pieces.h"
#include "Walls.h"
#include "SizeMenu.h"
#include "TurnCounter.h"

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
	Buttons(const Buttons&) = delete;
	Buttons& operator=(const Buttons&) = delete;
	const bool Interact(const D2D1_POINT_2F p) override;
	const bool Interact() override;
	const bool AlternateInteract() override;
	const bool AlternateInteract(const D2D1_POINT_2F p) override;
	void Draw() override;
	const bool IsSelected() override { return bSelected; }
	void SetIsSelected() override { bSelected = true; }
	void UnsetIsSelected() override { bSelected = false; }
	void ToggleIsSelected() { bSelected ^= true; }
};

class CounterButton : public Buttons
{
private:
	bool** ppShowCounter;
	TurnCounter** ppTurnCounter;
	PiecesW** ppFirstPiecesW;
public:
	CounterButton(TurnCounter** const counter, PiecesW** const first, bool** const showcount, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : ppTurnCounter(counter), ppFirstPiecesW(first), ppShowCounter(showcount), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	CounterButton(const CounterButton&) = delete;
	CounterButton& operator=(const CounterButton&) = delete;
	~CounterButton() = default;
	const bool Interact(const D2D1_POINT_2F p) override;
	const bool Interact() override;
};

class AttachObjectButtons : public Buttons
{
private:
	bool* const pAttachObject;
public:
	AttachObjectButtons(bool* const AttachObject, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : pAttachObject(AttachObject), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	AttachObjectButtons(const AttachObjectButtons&) = delete;
	AttachObjectButtons& operator=(const AttachObjectButtons&) = delete;
	~AttachObjectButtons() { }
	const bool Interact() override;
};

class NewButtons : public Buttons
{
private:
	bool* const pNew;
public:
	NewButtons(bool* const New, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : pNew(New), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	~NewButtons() {}
	NewButtons(const NewButtons&) = delete;
	NewButtons& operator=(const NewButtons&) = delete;
	const bool Interact() override;
};

class SaveButtons : public Buttons
{
private:
	const HWND hWnd;
public:
	SaveButtons(const HWND hwnd, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : hWnd(hwnd), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	~SaveButtons() {}
	SaveButtons(const SaveButtons&) = delete;
	SaveButtons& operator=(const SaveButtons&) = delete;
	const bool Interact() override;
};

class LoadButtons : public Buttons
{
private:
	const HWND hWnd;
public:
	LoadButtons(const HWND hwnd, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : hWnd(hwnd), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	~LoadButtons() { }
	LoadButtons(const LoadButtons&) = delete;
	LoadButtons& operator=(const LoadButtons&) = delete;
	const bool Interact() override;
};

class ExitButtons : public Buttons
{
private:
	bool* const pExit;
public:
	ExitButtons(bool* const Exit, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : pExit(Exit), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	~ExitButtons() {}
	ExitButtons(const ExitButtons&) = delete;
	ExitButtons& operator=(const ExitButtons&) = delete;
	const bool Interact() override;
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
	RoomLayerBox(const RoomLayerBox&) = delete;
	RoomLayerBox& operator=(const RoomLayerBox&) = delete;
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
	Checkbox(const Checkbox&) = delete;
	Checkbox& operator=(const Checkbox&) = delete;
};

class AddItem : public Buttons
{
private:
	bool bRoom = true;
public:
	AddItem(bool room, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : bRoom(room), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	const bool Interact(const D2D1_POINT_2F p) override;
	AddItem(const AddItem&) = delete;
	AddItem& operator=(const AddItem&) = delete;
};

class TypeButtons : public Buttons
{
private:
	bool* pWallMenuSelected = nullptr;
	bool bIsWall = false;
public:
	TypeButtons(bool* const pIsWall, const bool iswall,Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign), pWallMenuSelected(pIsWall), bIsWall(iswall) {}
	TypeButtons(const TypeButtons&) = delete;
	TypeButtons& operator=(const TypeButtons&) = delete;
	const bool Interact(const D2D1_POINT_2F p) override;
	void SetIsSelected() override { bSelected = true; if (bIsWall) if (pWallMenuSelected) *pWallMenuSelected = true; }
	void UnsetIsSelected() override { bSelected = false; if (pWallMenuSelected) *pWallMenuSelected = false; }
};

class SubsectionButtons : public Buttons
{
public:
	SubsectionButtons(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	SubsectionButtons(const SubsectionButtons&) = delete;
	SubsectionButtons& operator=(const SubsectionButtons&) = delete;
	const bool Interact(const D2D1_POINT_2F p) override;
};

class WallSubsectionButtons : public SubsectionButtons
{
private:
	bool* pUseTexture = nullptr;
	bool bUseTexture = false;
public:
	WallSubsectionButtons(bool* const pusetexture, const bool usetexture, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) :
		pUseTexture(pusetexture), bUseTexture(usetexture), SubsectionButtons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	WallSubsectionButtons(const WallSubsectionButtons&) = delete;
	WallSubsectionButtons& operator=(const WallSubsectionButtons&) = delete;
	const bool Interact(const D2D1_POINT_2F p) override;
};

class AoeSubsectionButtons : public SubsectionButtons
{
private:
	const AoeSpritePointer::AoeTypes mType;
	AoeSpritePointer::AoeTypes* pType;
public:
	AoeSubsectionButtons(AoeSpritePointer::AoeTypes* const ptype, const AoeSpritePointer::AoeTypes type, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) :
		mType(type), pType(ptype), SubsectionButtons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	const bool Interact(const D2D1_POINT_2F p) override;
};

class SpriteItemButtons : public Buttons
{
private:
	PiecesW* pPiecesW = nullptr;
	SpritePointer** ppSelectedSprite = nullptr;
	std::vector<PiecesW*>* pvInitativeList = nullptr;
	D2D1::Matrix3x2F* pMatrix = nullptr;
	D2D1_SIZE_F* pGridSquareSize = nullptr;
public:
	SpriteItemButtons(D2D1_SIZE_F* const GridSquareSize, std::vector<PiecesW*>* const pInitiativeVector, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, PiecesW* const piecew = nullptr, SpritePointer** const ppsp = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : pGridSquareSize(GridSquareSize), pPiecesW(piecew), ppSelectedSprite(ppsp), pvInitativeList(pInitiativeVector), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	SpriteItemButtons(const SpriteItemButtons&) = delete;
	SpriteItemButtons& operator=(const SpriteItemButtons&) = delete;
	void SetMatrixPointer(D2D1::Matrix3x2F* p) { pMatrix = p; }
	const bool Interact(const D2D1_POINT_2F p) override;
	void Draw() override;
	bool PointInRect() override;
	bool PointInRect(const D2D1_POINT_2F p) override;
	const bool AlternateInteract(const D2D1_POINT_2F p) override;
	const bool AlternateInteract() override;
};

class SizeMenuButtons : public Buttons
{
private:
	void* pSizeMenu = nullptr;
public:
	SizeMenuButtons(void* const psizemenu, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : pSizeMenu(psizemenu), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	SizeMenuButtons(const SizeMenuButtons&) = delete;
	SizeMenuButtons& operator=(const SizeMenuButtons&) = delete;
	const bool Interact() override;
};

class ColorButtons : public Buttons
{
private:
	SpritePointer** ppSelectedSprite = nullptr;
	D2D1_COLOR_F DrawColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	D2D1::Matrix3x2F* pMatrix = nullptr;
public:
	ColorButtons(D2D1_COLOR_F drawcolor, SpritePointer** ppsp, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : ppSelectedSprite(ppsp), DrawColor(drawcolor), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	ColorButtons(const ColorButtons&) = delete;
	ColorButtons& operator=(const ColorButtons&) = delete;
	void Draw() override;
	void SetMatrixPointer(D2D1::Matrix3x2F* const matrix) { pMatrix = matrix; }
	const bool Interact() override;
	const D2D1_COLOR_F GetDrawColor() { return DrawColor; }
};

class WallTextureButtons : public Buttons
{
public:
	WallTextureButtons(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	WallTextureButtons(const WallTextureButtons&) = delete;
	WallTextureButtons& operator=(const WallTextureButtons&) = delete;
};

class InitiativeListButtons : public SpriteItemButtons
{
private:
	PiecesW* pPiece = nullptr;
public:
	InitiativeListButtons(std::vector<PiecesW*>* const pInitiativeVector, D2D1_SIZE_F* const GridSquareSize, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, PiecesW* const piecew = nullptr, SpritePointer** const ppsp = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : SpriteItemButtons(GridSquareSize, pInitiativeVector, graphics, Transform, area, p, text, dest, textColor, parent, piecew, ppsp, enableselection, selected, highlight, talign, palign), pPiece(piecew) {}
	InitiativeListButtons(const InitiativeListButtons&) = delete;
	InitiativeListButtons& operator=(const InitiativeListButtons&) = delete;
	void Draw() override;
	const bool Interact() override;
	const bool AlternateInteract() override;
};

class TurnButtons : public Buttons
{
protected:
	void* const pSideMenu;
	ID2D1PathGeometry* pGeometry = nullptr;
	virtual void BuildGeometry() = 0;
public:
	TurnButtons(void* const psidemenu, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign), pSideMenu(psidemenu) {}
	~TurnButtons() { }
	TurnButtons(const TurnButtons&) = delete;
	TurnButtons& operator=(const TurnButtons&) = delete;
};

class NextTurnButtons : public TurnButtons
{
public:
	NextTurnButtons(void* const psidemenu, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : TurnButtons(psidemenu, graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign)	{ BuildGeometry(); }
	~NextTurnButtons() { SafeRelease(&pGeometry); }
	NextTurnButtons(const NextTurnButtons&) = delete;
	NextTurnButtons& operator=(const NextTurnButtons&) = delete;
	void Draw() override;
	const bool Interact() override;
	const bool AlternateInteract() override;
	void BuildGeometry() override;
};

class PreviousTurnButtons : public TurnButtons
{
public:
	PreviousTurnButtons(void* const psidemenu, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : TurnButtons(psidemenu, graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) { BuildGeometry(); }
	~PreviousTurnButtons() { SafeRelease(&pGeometry); }
	PreviousTurnButtons(const PreviousTurnButtons&) = delete;
	PreviousTurnButtons& operator=(const PreviousTurnButtons&) = delete;
	void Draw() override;
	const bool Interact() override;
	const bool AlternateInteract() override;
	void BuildGeometry() override;
};

class LockToGridButton : public Buttons
{
private:
	bool* const pLockToGrid;
public:
	LockToGridButton(bool* const LockToGrid, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, const wchar_t* text = nullptr, const D2D1_RECT_F dest = D2D1::RectF(), D2D1_COLOR_F textColor = D2D1::ColorF(0.0f, 0.0f, 0.0f), InteractObjects* const parent = nullptr, bool enableselection = false, bool selected = false,
		D2D1_COLOR_F highlight = D2D1::ColorF(1.0f, 0.0f, 1.0f, 0.60f), DWRITE_TEXT_ALIGNMENT talign = DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT palign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER) : pLockToGrid(LockToGrid), Buttons(graphics, Transform, area, p, text, dest, textColor, parent, enableselection, selected, highlight, talign, palign) {}
	const bool Interact() override;
	const bool Interact(const D2D1_POINT_2F p) override;
};