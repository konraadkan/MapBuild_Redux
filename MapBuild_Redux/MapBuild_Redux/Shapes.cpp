#include "Shapes.h"

void ClassShapes::Draw()
{
	if (bHide) return;
	switch (ShapeType)
	{
	case ShapeTypes::Ellipses:
		if (bFill)
		{
			gfx->FillEllipse(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, Center, m_Radius.width, m_Radius.height, m_Color);
			gfx->DrawEllipse(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, Center, m_Radius.width, m_Radius.height, D2D1::ColorF(0.0f, 0.0f, 0.0f), fThickness);
		}
		else
		gfx->DrawEllipse(gfx->GetCompatibleTarget(), *pTransforms, *pClientRect, Center, m_Radius.width, m_Radius.height, m_Color, fThickness);
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

void ClassShapes::Interact(const D2D1_POINT_2F p)
{
	if (pChild.size())
	{
		for (auto& child : pChild)
		{
			if (child->PointInRect(p))
			{
				child->Interact(p);
				return;
			}
		}
	}
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