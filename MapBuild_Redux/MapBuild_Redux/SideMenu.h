#pragma once
#include "Buttons.h"
#include "Shapes.h"

class SideMenu : public Buttons
{
public:
	SideMenu(Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area) : Buttons(graphics, Transform, area) {}
	SideMenu(const D2D1_RECT_F targetDest, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area) : Buttons(graphics, Transform, area)
	{
		SetDest(targetDest);
		SetColor(D2D1::ColorF(0.75f, 0.80f, 0.82f));
		pChild.push_back(new ClassShapes(ShapeTypes::Ellipses, gfx, Transform, pClientRect, true));
		pChild.back()->SetRadius(D2D1::SizeF(15.0f, (m_Dest.bottom + m_Dest.top) * 0.5f));
		pChild.back()->SetCenter(D2D1::Point2F(m_Dest.left, (m_Dest.bottom + m_Dest.top) * 0.5f));		
		pChild.back()->SetFill();
		pChild.back()->SetColor(D2D1::ColorF(0.0f, 0.2f, 0.8f, 0.95f));
		pChild.back()->SetLable(L"ShowHide");
		pChild.back()->pChild.push_back(new ClassShapes(ShapeTypes::Custom, gfx, Transform, pClientRect));
		std::queue<D2D1_POINT_2F> ps;
		ps.push(D2D1::Point2F(m_Dest.left - 12.0f, (m_Dest.bottom + m_Dest.top) * 0.5f));
		ps.push(D2D1::Point2F(m_Dest.left - 3.0f, (m_Dest.bottom + m_Dest.top) * 0.5f - 15.0f));
		ps.push(D2D1::Point2F(m_Dest.left - 3.0f, (m_Dest.bottom + m_Dest.top) * 0.5f + 15.0f));
		pChild.back()->pChild.back()->BuildCustomShape(ps, D2D1::ColorF(1.0f, 1.0f, 1.0f));
		pChild.back()->pChild.back()->SetLable(L"ShowHideTriangle");
		pChild.back()->pChild.back()->SetFill();
	}
	~SideMenu()
	{
		while (pChild.size())
		{
			SafeDelete(&pChild.back());
			pChild.pop_back();
		}
	}
	void Draw() override;
	void Interact(const D2D1_POINT_2F p) override;
	bool PointInRect(const D2D1_POINT_2F p) override;
};