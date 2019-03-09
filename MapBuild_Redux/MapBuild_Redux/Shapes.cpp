#include "Shapes.h"

void ClassShapes::Draw()
{
	if (bHide) return;
	switch (ShapeType)
	{
	case ShapeTypes::Ellipses:
		if (bFill)
		{
			if (bUseTransform)
			{
				gfx->FillEllipse(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, Center, m_Radius.width, m_Radius.height, m_Color);
				gfx->DrawEllipse(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, Center, m_Radius.width, m_Radius.height, D2D1::ColorF(0.0f, 0.0f, 0.0f), fThickness);
			}
			else
			{
				gfx->FillEllipse(gfx->GetCompatibleTarget(), Center, m_Radius.width, m_Radius.height, m_Color);
				gfx->DrawEllipse(gfx->GetCompatibleTarget(), Center, m_Radius.width, m_Radius.height, D2D1::ColorF(0.0f, 0.0f, 0.0f), fThickness);
			}
		}
		else
		{
			if (bUseTransform)
				gfx->DrawEllipse(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, Center, m_Radius.width, m_Radius.height, m_Color, fThickness);
			else
				gfx->DrawEllipse(gfx->GetCompatibleTarget(), Center, m_Radius.width, m_Radius.height, m_Color, fThickness);
		}
		break;
	case ShapeTypes::Custom:
		if (pPathGeometry.size())
		{
			for (auto& path : pPathGeometry)
			{
				if (path)
				{
					gfx->DrawGeometry(gfx->GetCompatibleTarget(), path, m_Color, fThickness, bFill);
				}
			}
		}
		break;
	case ShapeTypes::Rectangles:
		if (bUseTransform)
		{
			if (bFill) gfx->FillRect(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, m_Dest, m_Color);
			gfx->DrawRect(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, m_Dest, bFill ? D2D1::ColorF(0.0f, 0.0f, 0.0f) : m_Color, fThickness);
		}
		else
		{
			if (bFill) gfx->FillRect(gfx->GetCompatibleTarget(), m_Dest, m_Color);
			gfx->DrawRect(gfx->GetCompatibleTarget(), m_Dest, bFill ? D2D1::ColorF(0.0f, 0.0f, 0.0f) : m_Color, fThickness);
		}
		break;
	case ShapeTypes::RoundedRectangles:
		if (bUseTransform)
		{
			if (bFill) gfx->FillRoundedRect(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, m_Dest, m_Color, m_Radius.width, m_Radius.height);
			gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, m_Dest, bFill ? D2D1::ColorF(0.0f, 0.0f, 0.0f) : m_Color, m_Radius.width, m_Radius.height, fThickness);
		}
		else
		{
			if (bFill) gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_Color, m_Radius.width, m_Radius.height);
			gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, bFill ? D2D1::ColorF(0.0f, 0.0f, 0.0f) : m_Color, m_Radius.width, m_Radius.height, fThickness);
		}
		break;
	case ShapeTypes::Lines:
		if (vPoints.size() >= 2)
		{
			for (size_t i = 0; i < vPoints.size() - 1; i++)
			{
				if (bUseTransform) gfx->DrawLine(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, vPoints[i], vPoints[i + 1], m_Color, fThickness);
				else gfx->DrawLine(gfx->GetCompatibleTarget(), vPoints[i], vPoints[i + 1], m_Color, fThickness);
			}
		}
		break;
	}
	if (pChild.size())
	{
		for (auto& child : pChild)
		{
			if (child)
				child->Draw();
		}
	}
}

void ClassShapes::SetCenter(const D2D1_POINT_2F p)
{
	memcpy(&Center, &p, sizeof(Center));
	CalcRect();
}

const bool ClassShapes::Interact(const D2D1_POINT_2F p)
{
	if (pChild.size())
	{
		for (auto& child : pChild)
		{
			if (child->PointInRect(p))
			{
				if (!child->Interact(p)) return false;
				return true;
			}
		}
	}
	return true;
}

const bool ClassShapes::Interact()
{
	if (pChild.size())
	{
		for (auto& child : pChild)
		{
			if (child->PointInRect())
			{
				if (!child->Interact()) return false;
				return true;
			}
		}
	}
	return true;
}

void ClassShapes::SetFill()
{
	bFill = true;
}

void ClassShapes::UnsetFill()
{
	bFill = false;
}

bool ClassShapes::BuildCustomShape(std::queue<D2D1_POINT_2F> p, D2D1_COLOR_F color)
{
	m_Color = color;
	return gfx->BuildCustomGeometry(pPathGeometry, p, color);
}

void ClassShapes::CalcRect()
{
	switch(ShapeType)
	{
	case ShapeTypes::Ellipses:
		m_Dest = D2D1::RectF(Center.x - m_Radius.width, Center.y - m_Radius.height, Center.x + m_Radius.width, Center.y + m_Radius.height);
		break;
	}
}

void ClassShapes::Unload()
{
	while (pPathGeometry.size())
	{
		SafeRelease(&pPathGeometry.back());
		pPathGeometry.pop_back();
	}
}

ShowHideParentShape::ShowHideParentShape(InteractObjects* const Parent, ShapeTypes Type, Graphics* const graphics, bool UseTransform,
	D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, bool Back) : ClassShapes(Type, graphics, UseTransform, Transform, area, p, Back), pParent(Parent)
{

}

const bool ShowHideParentShape::Interact()
{
	if (PointInRect())
	{
		if (pParent) pParent->ToggleHidden();
		return false;
	}
	return true;
}

const bool ShowHideParentShape::Interact(const D2D1_POINT_2F p)
{
	if (PointInRect(p))
	{
		if (pParent) pParent->ToggleHidden();
		return false;
	}
	return true;
}