#include "Buttons.h"

void Buttons::Interact(D2D1_POINT_2F p)
{
	if (IsHidden()) return;
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect())
			{
				if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
				{
					if(child->pParent) child->pParent->ChangeMode();
				}
				else
					child->Interact();
				return;
			}
		}
	}
	//Interaction... look into what exactly to do with this later
	if(bEnableSelection) bSelected ^= true;
}

void Buttons::Interact()
{
	if (IsHidden()) return;
	Interact(*pMouseCoordinates);
}

void Buttons::Draw()
{
	if (IsHidden()) return;
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, D2D1::ColorF(0.0f, 0.0f, 0.0f), m_Radius.width, m_Radius.height, fThickness);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_Color, m_Radius.width, m_Radius.height);
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), GetLabel(), m_Dest, TextColor, m_Alignment, m_pAlignment);
	if (PointInRect()) gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_HighlightColor, m_Radius.width, m_Radius.height);
	if (bSelected) gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_SelectedColor, m_Radius.width, m_Radius.height);
	for (auto& child : pChild)
	{
		if (child) child->Draw();
	}
}
