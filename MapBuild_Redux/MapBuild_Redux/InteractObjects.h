#pragma once
#include "Graphics.h"
#include "SafeReleaseMemory.h"
#include <vector>
#include <string>


//***** this virtual class may take the place of InteractObjects in the future to make the code cleaner
//class InteractableObjects : public SafeReleaseMemory
//{
//protected:
//	enum class Sector
//	{
//		East,
//		West,
//		North,
//		South,
//		NorthEast,
//		SouthEast,
//		NorthWest,
//		SouthWest
//	};
//	struct RectDetails
//	{
//		struct Colors
//		{
//			D2D1_COLOR_F BackgroundColor = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
//			D2D1_COLOR_F TextColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
//			D2D1_COLOR_F BorderColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
//			D2D1_COLOR_F HighlightColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
//			D2D1_COLOR_F SelectedColor = D2D1::ColorF(0.f, 0.0f, 0.0f, 0.0f);
//		} RectColors;
//		D2D1_RECT_F m_Rect = D2D1::RectF();
//		float fBorderThickness = 1.0f;
//	} RectDetails;
//protected:
//	D2D1_POINT_2F* pMouseCoordinates;
//	std::wstring wLabel;
//	DWRITE_PARAGRAPH_ALIGNMENT dpAlignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;
//	DWRITE_TEXT_ALIGNMENT dtAlignment = DWRITE_TEXT_ALIGNMENT_LEADING;
//	D2D1::Matrix3x2F mTranslation = D2D1::Matrix3x2F::Identity();				//used exclusively for positioning
//	D2D1::Matrix3x2F mTransforms = D2D1::Matrix3x2F::Identity();				//apply this with the above for any further transforms
//public:
//	virtual bool Interact() = 0;
//	virtual bool PointInRect() = 0;
//	virtual bool PointInRect(D2D1_POINT_2F p) = 0;
//	virtual bool PointInSector(Sector s) = 0;
//	virtual bool PointInSector(Sector s, D2D1_POINT_2F p) = 0;
//	virtual bool IsHidden() = 0;
//	virtual void SetHidden() = 0;
//	virtual void UnsetHidden() = 0;
//	virtual void Draw() = 0;
//	virtual void SetMousePointer(D2D1_POINT_2F* const p) = 0;
//	virtual void SetBackgroundColor(D2D1_COLOR_F color) = 0;
//	virtual void SetBorderColor(D2D1_COLOR_F color) = 0;
//	virtual void SetTextColor(D2D1_COLOR_F color) = 0;
//	virtual void SetLabel(std::wstring label) = 0;
//	virtual void AddChild(InteractableObjects* const child) = 0;	
//	virtual void SetTranslation(D2D1_SIZE_F ShiftDistance) = 0;
//	virtual void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT pAlign) = 0;
//	virtual void SetTextAlignment(DWRITE_TEXT_ALIGNMENT tAlign) = 0;
//	virtual void SetBorderThickness(const float fThickness) = 0;
//	virtual void SetParent(InteractableObjects* const parent) = 0;
//	virtual const DWRITE_TEXT_ALIGNMENT GetTextAlignment() = 0;
//	virtual const DWRITE_PARAGRAPH_ALIGNMENT GetParagraphAlignment() = 0;	
//	virtual const std::wstring GetLabel() = 0;	
//	virtual std::vector<InteractableObjects>* const GetChildren() = 0;
//	virtual const D2D1_RECT_F GetRect() = 0;
//	virtual const D2D1_RECT_F GetTranslatedRect() = 0;
//	virtual const D2D1_RECT_F GetInvTranslatedRect() = 0;	
//	virtual const D2D1_SIZE_F GetRectSize() = 0;
//	virtual InteractableObjects* const GetChild(size_t position) = 0;
//	virtual InteractableObjects* const GetParent() = 0;
//public:
//	std::vector<InteractableObjects> vChildren;
//	InteractableObjects* pParent = nullptr;
//};

class InteractObjects : public SafeReleaseMemory
{
public:
	D2D1::Matrix3x2F* pTransforms = nullptr;
	D2D1_RECT_F* pClientRect = nullptr;
protected:
	D2D1_RECT_F m_Dest = D2D1::RectF();
	D2D1_RECT_F m_ExpandedDest = D2D1::RectF();
	D2D1_COLOR_F m_Color = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	D2D1_COLOR_F m_HighlightColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
	D2D1_COLOR_F m_SelectedColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
	D2D1_SIZE_F m_Radius = D2D1::SizeF();
	float fThickness = 1.0f;
	Graphics* gfx = nullptr;
	bool bHide = false;
	bool bMouseOver = false;
	D2D1::Matrix3x2F* pInvTransforms = nullptr;
	std::wstring wLable;
	DWRITE_TEXT_ALIGNMENT m_Alignment;
	DWRITE_PARAGRAPH_ALIGNMENT m_pAlignment;
	D2D1_POINT_2F* pMouseCoordinates;	
protected:
	enum class Sector
	{
		East,
		West,
		North,
		South,
		NorthEast,
		SouthEast,
		NorthWest,
		SouthWest
	};
	enum class BorderStyle
	{
		Invisible,
		Solid,
		Dotted
	};
public:
	InteractObjects(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p) : gfx(graphics), pTransforms(Transform), pClientRect(area), pMouseCoordinates(p) {}
	~InteractObjects();
	InteractObjects(const InteractObjects&) = delete;
	InteractObjects& operator=(const InteractObjects&) = delete;
	virtual const bool Interact(const D2D1_POINT_2F p);
	virtual const bool Interact();
	virtual void Draw() = 0;
public:
	virtual void SetSelectionColor(const D2D1_COLOR_F color) { memcpy(&m_SelectedColor, &color, sizeof(m_SelectedColor)); }
	virtual void SetInvertTransformPointer(D2D1::Matrix3x2F* const pInvT) { pInvTransforms = pInvT; }
	virtual void SetMousePointer(D2D1_POINT_2F* const p) { pMouseCoordinates = p; }
	virtual void WheelUp() {}
	virtual void WheelDown() {}
	virtual void Unload();
	virtual bool PointInRect();
	virtual bool PointInRect(const D2D1_POINT_2F p);
	virtual bool PointInTargetRect(const D2D1_RECT_F rect) { return PointInTargetRect(rect, *pMouseCoordinates); }
	virtual bool PointInTargetRect(const D2D1_RECT_F rect, const D2D1_POINT_2F p);
	virtual bool PointInSector(const Sector sector) { return PointInSector(sector, *pMouseCoordinates); }
	virtual bool PointInSector(const Sector sector, const D2D1_POINT_2F p);
	virtual void SetDest(const D2D1_RECT_F rect);
	virtual void SetDestBottom(const float bottom);
	virtual void MovePosition(const D2D1_POINT_2F p);
	virtual void SetColor(const D2D1_COLOR_F color);
	virtual void SetHighlightColor(const D2D1_COLOR_F color);
	virtual void SetThickness(const float thickness);
	virtual void SetRadiusX(const float rad);
	virtual void SetRadiusY(const float rad);
	virtual void SetRadius(const D2D1_SIZE_F rad);
	virtual void SetCenter(const D2D1_POINT_2F p);
	virtual void SetHidden();
	virtual void SetUnhidden();
	virtual void SetFill() {}
	virtual void UnsetFill() {}
	virtual bool IsBack() { return false; }
	virtual bool BuildCustomShape(std::queue<D2D1_POINT_2F> p, D2D1_COLOR_F color) { return false; }
	virtual const D2D1_RECT_F GetRect() { return m_Dest; }
	virtual const D2D1_SIZE_F GetSize() { return D2D1::SizeF(m_Dest.right - m_Dest.left, m_Dest.bottom - m_Dest.top); }
	virtual void SetLabel(const wchar_t* name) { wLable = name; }
	virtual const wchar_t* GetLabel() { return wLable.c_str(); }
	virtual bool IsHidden() { return bHide; }
	virtual void SetParagraphAlignment(const DWRITE_PARAGRAPH_ALIGNMENT pAlign) { m_pAlignment = pAlign; }
	virtual const DWRITE_PARAGRAPH_ALIGNMENT GetParagraphAlignment() { return m_pAlignment; }
	virtual void SetTextAlignment(const DWRITE_TEXT_ALIGNMENT Align) { m_Alignment = Align; }
	virtual const DWRITE_TEXT_ALIGNMENT GetTextAlignment() { return m_Alignment; }
	virtual void MouseOver() {}
	virtual void AddChild(InteractObjects* const Iobject) {}
	virtual void AddChild(InteractObjects* const Iobjects, D2D1_SIZE_F size, float seperation = 0.0f) {}
	virtual void ChangeMode() {}
	virtual const bool IsSelected() { return false; }
	virtual void SetIsSelected() { }
	virtual void UnsetIsSelected() { }
	virtual const D2D1_RECT_F GetTranslatedRect() { return m_Dest; }
	virtual D2D1::Matrix3x2F* const GetInvTransforms() { return pInvTransforms; }
	virtual void SetRoom(const size_t pot) { }
	virtual void SetLayer(const size_t pos) { }
	virtual void SetBorderStyle(const BorderStyle border) {}
public:
	std::vector<InteractObjects*> pChild;
	InteractObjects* pParent = nullptr;
};