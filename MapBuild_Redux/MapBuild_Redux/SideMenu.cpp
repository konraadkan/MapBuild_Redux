#include "SideMenu.h"

void SideMenu::Draw()
{
	for (auto& child : pChild)
	{
		if (child) 
			if(child->IsBack()) child->Draw();
	}
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, D2D1::ColorF(0.0f, 0.0f, 0.0f), m_Radius.width, m_Radius.height, fThickness);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_Color, m_Radius.width, m_Radius.height);
	for (auto& child : pChild)
	{
		if (child)
			if (!child->IsBack()) child->Draw();
	}
}

void SideMenu::Interact(const D2D1_POINT_2F p)
{
	for (auto& child : pChild)
	{
		if (child->PointInRect(p))
		{
			if (!_wcsicmp(child->GetLabel(), L"ShowHide"))
			{
				bHide ? SetUnhidden() : SetHidden();
			}
			else child->Interact(p);
			return;
		}
	}
}

bool SideMenu::PointInRect(const D2D1_POINT_2F p)
{
	m_ExpandedDest = D2D1::RectF(m_Dest.left - pChild.front()->GetSize().width, m_Dest.top, m_Dest.right, m_Dest.bottom);
	return (p.x >= m_ExpandedDest.left && p.x <= m_ExpandedDest.right && p.y >= m_ExpandedDest.top && p.y <= m_ExpandedDest.bottom);
}