#include "MenuSection.h"

void MenuSection::ResizeDest()
{
	if (pChild.back()->GetRect().top >= m_Dest.bottom)
		m_Dest.bottom = pChild.back()->GetRect().bottom + 1.0f;
}

void MenuSection::Draw() 
{
	D2D1::Matrix3x2F ttransform;
	gfx->GetCompatibleTarget()->GetTransform(&ttransform);
	gfx->GetCompatibleTarget()->SetTransform(ttransform * Transforms);
	for (auto& child : pChild)
	{
		child->Draw();
	}
	gfx->GetCompatibleTarget()->SetTransform(&ttransform);
}

void MenuSection::AddChild(InteractObjects* const Iobject)
{
	pChild.push_back(Iobject);
	pChild.back()->SetInvertTransformPointer(&InvTransforms);
	ResizeDest();
}

void MenuSection::Unload()
{
	for (auto& child : pChild)
	{
		SafeDelete(&child);
	}
	while (pChild.size())
	{
		pChild.pop_back();
	}
}

void MenuSection::WheelUp()
{
	if (!bIsScrollable) return;
	if (Transforms.TransformPoint(D2D1::Point2F(pChild.back()->GetRect().left, pChild.back()->GetRect().top)).y > m_Dest.top)
	{
		Transforms = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0, ScrollStep * -1)) * Transforms;
		InvTransforms = Transforms;
		InvTransforms.Invert();

		for (auto& child : pChild)
		{
			D2D1_POINT_2F t = Transforms.TransformPoint(D2D1::Point2F(child->GetRect().left, child->GetRect().top));
			if (t.y < m_Dest.top)
				child->SetHidden();
		}
	}
}

void MenuSection::WheelDown()
{
	if (!bIsScrollable) return;
	if (Transforms.TransformPoint(D2D1::Point2F()).y < 0.0f)
	{
		Transforms = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0, ScrollStep)) * Transforms;
		InvTransforms = Transforms;
		InvTransforms.Invert();

		for (auto& child : pChild)
		{
			if (child->IsHidden())
			{
				D2D1_POINT_2F t = Transforms.TransformPoint(D2D1::Point2F(child->GetRect().left, child->GetRect().top));
				if (t.y >= m_Dest.top)
					child->SetUnhidden();
			}
		}
	}
}

bool MenuSection::PointInRect(const D2D1_POINT_2F p)
{
	TransformedPoint = InvTransforms.TransformPoint(p);
	return (TransformedPoint.x >= m_Dest.left && TransformedPoint.x <= m_Dest.right && TransformedPoint.y >= m_Dest.top && TransformedPoint.y <= m_Dest.bottom);
}

bool MenuSection::PointInRect()
{
	return PointInRect(*pMouseCoordinates);
}