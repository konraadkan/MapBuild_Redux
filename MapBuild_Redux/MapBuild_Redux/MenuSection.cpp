#include "MenuSection.h"

void MenuSection::ResizeDest()
{
	if (pChild.empty()) return;
	if (pChild.back()->pChild.size())
	{
		if (pChild.back()->GetRect().right + pChild.back()->GetSize().width >= m_Dest.right)
			m_Dest.bottom = pChild.back()->pChild.back()->GetRect().bottom + 1.0f + pChild.back()->GetSize().height + pChild.back()->pChild.back()->GetSize().height;
		if (pChild.back()->pChild.back()->GetRect().bottom >= m_Dest.bottom)
			m_Dest.bottom = pChild.back()->pChild.back()->GetRect().bottom + 1.0f;
	}
	if (pChild.back()->GetRect().bottom >= m_Dest.bottom)
		m_Dest.bottom = pChild.back()->GetRect().bottom + 1.0f;
}

void MenuSection::ResizeDest(const D2D1_SIZE_F r)
{
	if (pChild.back()->pChild.size())
		if (r.height >= m_Dest.bottom)
			m_Dest.bottom = r.height + 1.0f;
	if (pChild.back()->GetRect().top >= m_Dest.bottom)
		m_Dest.bottom = pChild.back()->GetRect().bottom + 1.0f;
}

void MenuSection::Draw() 
{
	if (IsHidden()) return;
	D2D1::Matrix3x2F ttransform;
	gfx->GetCompatibleTarget()->GetTransform(&ttransform);
	AllTransforms = ttransform * Transforms;
	gfx->GetCompatibleTarget()->SetTransform(AllTransforms);
	for (auto& child : pChild)
	{
		child->Draw();
	}
	
	for (auto& subsection : vSubsections)
	{
		subsection->Draw();
	}
	switch (Border)
	{
	case BorderStyle::Dotted:
		gfx->DrawDottedLine(gfx->GetCompatibleTarget(), D2D1::Point2F(m_Dest.left, m_Dest.bottom + 2.0f), D2D1::Point2F(m_Dest.right, m_Dest.bottom + 2.0f));
		break;
	case BorderStyle::Solid:
		gfx->DrawLine(gfx->GetCompatibleTarget(), D2D1::Point2F(m_Dest.left, m_Dest.bottom + 2.0f), D2D1::Point2F(m_Dest.right, m_Dest.bottom + 2.0f));
		break;
	}
	gfx->GetCompatibleTarget()->SetTransform(&ttransform);
}

void MenuSection::AddChild(InteractObjects* const Iobject, const D2D1_SIZE_F size, float seperation)
{
	D2D1_RECT_F lastrect = (!pChild.empty()) ? pChild.back()->GetRect() : D2D1::RectF(m_Dest.left, m_Dest.top, m_Dest.left, m_Dest.top + size.height);
	pChild.push_back(Iobject);
	D2D1_RECT_F newrect = D2D1::RectF();
	if (lastrect.right + size.width > m_Dest.right)
		newrect = D2D1::RectF(m_Dest.left + 0.2f, lastrect.bottom + seperation, m_Dest.left + size.width + 0.2f, lastrect.bottom + seperation + size.height);
	else
		newrect = D2D1::RectF(lastrect.right + 1.0f, lastrect.top, lastrect.right + size.width + 1.0f, lastrect.top + size.height);
	pChild.back()->SetDest(newrect);
	pChild.back()->SetInvertTransformPointer(&InvTransforms);
	pChild.back()->SetRadius(D2D1::SizeF(10.0f, 10.0f));
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
	while (vSubsections.size())
	{
		SafeDelete(&vSubsections.back());
		vSubsections.pop_back();
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
		InvTransforms = AllTransforms * Transforms;
		InvTransforms.Invert();

		for (auto& child : pChild)
		{
			D2D1_POINT_2F t = Transforms.TransformPoint(D2D1::Point2F(child->GetRect().left, child->GetRect().top));
			if (t.y < m_Dest.top)
				child->SetHidden();
		}
		InvTransforms = AllTransforms;
		InvTransforms.Invert();
	}
}

void MenuSection::WheelDown()
{
	if (!bIsScrollable) return;
	if (Transforms.TransformPoint(D2D1::Point2F()).y < 0.0f)
	{
		Transforms = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0, ScrollStep)) * Transforms;
		InvTransforms = AllTransforms * Transforms;
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
		InvTransforms = AllTransforms;
		InvTransforms.Invert();
	}
}

bool MenuSection::PointInRect(const D2D1_POINT_2F p)
{
	TransformedPoint = InvTransforms.TransformPoint(p);
	for (auto& subsections : vSubsections)
	{
		if (subsections->PointInRect(p))
			return true;
	}
	return (TransformedPoint.x >= m_Dest.left && TransformedPoint.x <= m_Dest.right && TransformedPoint.y >= m_Dest.top && TransformedPoint.y <= m_Dest.bottom);
}

bool MenuSection::PointInRect()
{
	return PointInRect(*pMouseCoordinates);
}

void MenuSection::CreateSubsection(const wchar_t* label, bool scroll, float scrollsize)
{
	vSubsections.push_back(new MenuSection(gfx, &Transforms, pClientRect, pMouseCoordinates, D2D1::Rect(m_ExpandedDest.left, m_ExpandedDest.bottom + 2.0f, m_ExpandedDest.right, m_ExpandedDest.bottom + 66.0f), scrollsize, label, scroll));
}

void MenuSection::CreateSubsection(const wchar_t* label, D2D1_RECT_F dest, bool scroll, float scrollsize)
{
	vSubsections.push_back(new MenuSection(gfx, &Transforms, pClientRect, pMouseCoordinates, dest, scrollsize, label, scroll));
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

const bool MenuSection::AlternateInteract()
{
	if (bHide) return true;
	for (auto& child : pChild)
	{
		if (!child) continue;
		if (child->pChild.size())
		{
			for (auto& b : child->pChild)
			{
				if (!b) continue;
				if (b->PointInRect())
					if (!b->AlternateInteract()) return false;
			}
		}
		if (child->PointInRect())
			if (!child->AlternateInteract()) return false;
	}
	for (auto& section : vSubsections)
	{
		if (!section) continue;
		if (section->PointInRect())
			if (!section->AlternateInteract()) return false;
	}
	return true;
}

const bool MenuSection::Interact()
{
	if (bHide) return true;
	for (auto& child : pChild)
	{
		if (!child) continue;
		if (child->pChild.size())
		{
			for (auto& b : child->pChild)
			{
				if (!b) continue;
				if (b->PointInRect())
					if (!b->Interact()) return false;
			}
		}
		if (child->PointInRect())
			if (!child->Interact()) return false;
	}
	for (auto& section : vSubsections)
	{
		if (!section) continue;
		if (section->PointInRect())
			if (!section->Interact()) return false;
	}
	return true;
}

void MenuSection::SetTranslation(D2D1_SIZE_F size)
{
	//doing it this way so it reverts to default position;
	Transforms = D2D1::Matrix3x2F::Translation(size);
	InvTransforms = Transforms;
	InvTransforms.Invert();
}

void MenuSection::UpdateInvTranforms(D2D1::Matrix3x2F transform)
{
	InvTransforms = transform;
	InvTransforms.Invert();
}

const D2D1_RECT_F MenuSection::GetTranslatedRect()
{
	D2D1_POINT_2F tl = InvTransforms.TransformPoint(D2D1::Point2F(m_Dest.left, m_Dest.top));
	D2D1_POINT_2F br = InvTransforms.TransformPoint(D2D1::Point2F(m_Dest.right, m_Dest.bottom));
	return D2D1::RectF(tl.x, tl.y, br.x, br.y);
}

void MenuSection::ShiftTranslation(D2D1_SIZE_F distance)
{
	Transforms = Transforms * D2D1::Matrix3x2F::Translation(distance);
}

const D2D1_RECT_F MenuSection::GetTranslatedRectNotInv()
{
	D2D1_POINT_2F tl = Transforms.TransformPoint(D2D1::Point2F(m_Dest.left, m_Dest.top));
	D2D1_POINT_2F br = Transforms.TransformPoint(D2D1::Point2F(m_Dest.right, m_Dest.bottom));
	return D2D1::RectF(tl.x, tl.y, br.x, br.y);
}

void MenuSection::SetRoom(const size_t pos)
{
	if (pos < (*vSelectRoomsandLayers)->size())
	{
		*pSelectedRoom = &(*vSelectRoomsandLayers)->at(pos);
		*pSelectedLayer = &(*pSelectedRoom)->front();
		*ppSelectedWallRoom = &(*pSelectWallRoomsandLayers)->at(pos);
		*ppSelectedWallLayer = &(*ppSelectedWallRoom)->front();
	}
	else MessageBoxW(nullptr, L"Failed to set room pointer.", L"Error", MB_OK | MB_ICONERROR);
	//show layer buttons for proper layer; hide rest of layer buttons
}

void MenuSection::SetLayer(const size_t pos)
{
	if ((*pSelectedRoom)->size() > pos)	*pSelectedLayer = &(*pSelectedRoom)->at(pos);
	if ((*ppSelectedWallRoom)->size() > pos) *ppSelectedWallLayer = &(*ppSelectedWallRoom)->at(pos);
	else MessageBoxW(nullptr, L"Failed to set layer", L"Error", MB_OK | MB_ICONERROR);
}

MenuSection* const MenuSection::FindSubmenuSection(const wchar_t* wCategoryName)
{
	for (auto& subsection : vSubsections)
	{
		if (!_wcsicmp(subsection->GetLabel(), wCategoryName))
			return subsection;
	}
	return nullptr;
}

