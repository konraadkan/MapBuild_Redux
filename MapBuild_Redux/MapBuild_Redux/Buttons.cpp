#include "Buttons.h"

void Buttons::Interact(D2D1_POINT_2F p)
{
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				child->Interact(p);
				return;
			}
		}
	}
	//Interaction... look into what exactly to do with this later
}

void Buttons::Draw()
{
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, D2D1::ColorF(0.0f, 0.0f, 0.0f), m_Radius.width, m_Radius.height, fThickness);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_Color, m_Radius.width, m_Radius.height);
	for (auto& child : pChild)
	{
		if (child) child->Draw();
	}
}