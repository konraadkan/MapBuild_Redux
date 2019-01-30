#include "InteractObjects.h"

InteractObjects::~InteractObjects()
{
	Unload();
}

void InteractObjects::Unload()
{
	while (pChild.size())
	{
		pChild.back()->Unload();
		SafeDelete(&pChild.back());
		pChild.pop_back();
	}
}

bool InteractObjects::PointInRect()
{
	if (!pInvTransforms) return PointInRect(*pMouseCoordinates);
	return PointInRect(pInvTransforms->TransformPoint(*pMouseCoordinates));
}

bool InteractObjects::PointInRect(const D2D1_POINT_2F p)
{
	return (p.x >= m_Dest.left && p.x <= m_Dest.right && p.y >= m_Dest.top && p.y <= m_Dest.bottom);
}

bool InteractObjects::PointInTargetRect(const D2D1_RECT_F rect, const D2D1_POINT_2F p)
{
	return (p.x >= rect.left && p.x <= rect.right && p.y >= rect.top && p.y <= rect.bottom);
}

bool InteractObjects::PointInSector(const Sector sector, const D2D1_POINT_2F p)
{
	D2D1_RECT_F sector_rect = D2D1::RectF();
	switch (sector)
	{
	case Sector::East:
		sector_rect.left = m_Dest.left + (m_Dest.right - m_Dest.left) * 0.5f;
		sector_rect.top = m_Dest.top;
		sector_rect.right = m_Dest.right;
		sector_rect.bottom = m_Dest.bottom;
		break;
	case Sector::West:
		sector_rect.left = m_Dest.left;
		sector_rect.top = m_Dest.top;
		sector_rect.right = m_Dest.left + (m_Dest.right - m_Dest.left) * 0.5f;
		sector_rect.bottom = m_Dest.bottom;
		break;
	case Sector::North:
		sector_rect.left = m_Dest.left;
		sector_rect.top = m_Dest.top;
		sector_rect.right = m_Dest.right;
		sector_rect.bottom = m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.5f;
		break;
	case Sector::South:
		sector_rect.left = m_Dest.left;
		sector_rect.top = m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.5f;
		sector_rect.right = m_Dest.right;
		sector_rect.bottom = m_Dest.bottom;
		break;
	}
	return PointInTargetRect(sector_rect, p);
}

void InteractObjects::SetDest(const D2D1_RECT_F rect)
{
	memcpy(&m_Dest, &rect, sizeof(m_Dest));
}

void InteractObjects::MovePosition(const D2D1_POINT_2F p)
{
	D2D1_RECT_F newrect;
	newrect.left = p.x;
	newrect.top = p.y;
	newrect.right = newrect.left + (m_Dest.right - m_Dest.left);
	newrect.bottom = newrect.top + (m_Dest.bottom - m_Dest.top);
	SetDest(newrect);
}

void InteractObjects::SetColor(const D2D1_COLOR_F color)
{
	memcpy(&m_Color, &color, sizeof(m_Color));
}

void InteractObjects::SetHighlightColor(const D2D1_COLOR_F color)
{
	memcpy(&m_HighlightColor, &color, sizeof(m_HighlightColor));
}

void InteractObjects::SetThickness(const float thickness)
{
	fThickness = thickness;
}

void InteractObjects::SetRadiusX(const float rad)
{
	m_Radius.width = rad;
}

void InteractObjects::SetRadiusY(const float rad)
{
	m_Radius.height = rad;
}

void InteractObjects::SetRadius(const D2D1_SIZE_F rad)
{
	memcpy(&m_Radius, &rad, sizeof(m_Radius));
}

void InteractObjects::SetCenter(const D2D1_POINT_2F p)
{
	//default
}

void InteractObjects::SetHidden()
{
	bHide = true;
}

void InteractObjects::SetUnhidden()
{
	bHide = false;
}

void InteractObjects::Interact(const D2D1_POINT_2F p)
{
	for (auto& child : pChild)
	{
		if (child->PointInRect(p))
		{
			if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
			{
				if (child->pParent) child->pParent->ChangeMode();
			}
			child->Interact(p);
		}
	}
}

void InteractObjects::Interact()
{
	for (auto& child : pChild)
	{
		if (child->PointInRect())
		{
			if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
			{
				if (child->pParent) child->pParent->ChangeMode();
			}
			child->Interact();
		}
	}
}