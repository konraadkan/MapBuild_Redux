#pragma once
#include "Graphics.h"
#include <vector>
#include <string>

class InteractObjects
{
protected:
	template<typename T> void SafeRelease(T** ppT)
	{
		if (ppT)
		{
			(*ppT)->Release();
			(*ppT) = nullptr;
		}
	}
	template<typename T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			delete (*ppT);
			(*ppT) = nullptr;
		}
	}
	template<typename T> void SafeDeleteArray(T** ppT)
	{
		if (ppT)
		{
			delete[] (*ppT);
			(*ppT) = nullptr;
		}
	}
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
	D2D1::Matrix3x2F* pTransforms = nullptr;
	D2D1::Matrix3x2F* pInvTransforms = nullptr;
	D2D1_RECT_F* pClientRect = nullptr;
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
public:
	InteractObjects(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p) : gfx(graphics), pTransforms(Transform), pClientRect(area), pMouseCoordinates(p) {}
	~InteractObjects();
	virtual void Interact(const D2D1_POINT_2F p);
	virtual void Interact();
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
	virtual void AddChild(InteractObjects* Iobject) {}
	virtual void ChangeMode() {}
	virtual const bool IsSelected() { return false; }
public:
	std::vector<InteractObjects*> pChild;
	InteractObjects* pParent = nullptr;
};