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
	
	for (auto& subsection : pSubsections)
	{
		for (auto& child : pChild)
		{
			if (child->IsSelected())
			if (!_wcsicmp(child->GetLabel(), subsection->GetLabel()))
				subsection->Draw();
		}
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
	while (vChildObjects.size())
	{
		while (vChildObjects.back().size())
		{
			SafeDelete(&vChildObjects.back().back());
			vChildObjects.back().pop_back();
		}
		vChildObjects.pop_back();
	}
	while (pSubsections.size())
	{
		SafeDelete(&pSubsections.back());
		pSubsections.pop_back();
	}
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

void MenuSection::CreateSubsection(const wchar_t* label, bool scroll, float scrollsize)
{
	pSubsections.push_back(new MenuSection(gfx, &Transforms, pClientRect, pMouseCoordinates, D2D1::Rect(m_ExpandedDest.left, m_ExpandedDest.bottom + 2.0f, m_ExpandedDest.right, m_ExpandedDest.bottom + 66.0f), scrollsize, label, scroll));
}

void MenuSection::UpdateChildPositions(std::vector<InteractObjects*>& childobjs)
{
	if (childobjs.empty()) return;
	if (!childobjs[0]) return;

	childobjs[0]->SetDest(D2D1::RectF(m_Dest.left, m_Dest.top + 5.0f, m_Dest.left + EntrySize.width, m_Dest.top + EntrySize.height + 5.0f));
	for (size_t i = 1; i < childobjs.size(); i++)
	{
		childobjs[i]->SetDest(D2D1::RectF(childobjs[i - 1]->GetRect().right + 2.0f, childobjs[i - 1]->GetRect().top, childobjs[i - 1]->GetRect().right + EntrySize.width + 2.0f, childobjs[i - 1]->GetRect().bottom));
		if (childobjs[i]->GetRect().right > m_Dest.right)
		{
			childobjs[i]->SetDest(D2D1::RectF(m_Dest.left, childobjs[i - 1]->GetRect().bottom + 2.0f, m_Dest.left + EntrySize.width, childobjs[i - 1]->GetRect().bottom + EntrySize.height + 2.0f));
		}
		if (childobjs[i]->GetRect().bottom > m_Dest.bottom) m_Dest.bottom = childobjs[i]->GetRect().bottom;
	}
}

void MenuSection::UpdateChildPositions()
{
	if (pChild.empty()) return;
	if (!pChild[0]) return;

	pChild[0]->SetDest(D2D1::RectF(m_Dest.left, m_Dest.top + 5.0f, m_Dest.left + EntrySize.width, m_Dest.top + EntrySize.height + 5.0f));
	for (size_t i = 1; i < pChild.size(); i++)
	{
		pChild[i]->SetDest(D2D1::RectF(pChild[i - 1]->GetRect().right + 2.0f, pChild[i - 1]->GetRect().top, pChild[i - 1]->GetRect().right + EntrySize.width + 2.0f, pChild[i - 1]->GetRect().bottom));
		if (pChild[i]->GetRect().right > m_Dest.right)
		{
			pChild[i]->SetDest(D2D1::RectF(m_Dest.left, pChild[i - 1]->GetRect().bottom + 2.0f, m_Dest.left + EntrySize.width, pChild[i - 1]->GetRect().bottom + EntrySize.height + 2.0f));
		}
	}
	m_Dest.bottom = pChild.back()->GetRect().bottom;
}

void MenuSection::Interact()
{
	for (auto& child : pChild)
	{
		if (!child) continue;
		if (child->PointInRect())
			child->Interact();
	}
	for (auto& section : pSubsections)
	{
		if (!section) continue;
		if (section->PointInRect())
			section->Interact();
	}
}