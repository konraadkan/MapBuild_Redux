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

bool InteractObjects::PointInRect(const D2D1_POINT_2F p)
{
	return (p.x >= m_Dest.left && p.x <= m_Dest.right && p.y >= m_Dest.top && p.y <= m_Dest.bottom);
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