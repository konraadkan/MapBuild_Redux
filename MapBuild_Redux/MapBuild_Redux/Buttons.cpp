#include "Buttons.h"

void Buttons::Interact(D2D1_POINT_2F p)
{
	if (IsHidden()) return;
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
				{
					if(child->pParent) child->pParent->ChangeMode();
				}
				else
					child->Interact(p);
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

void Checkbox::Interact()
{
	if (IsHidden()) return;
	if (IsSelected()) return;

	SetIsSelected();
	for (auto& c : pParent->pParent->pChild)
	{
		if (!c) continue;
		for (auto& cb : c->pChild)
		{
			if (!cb || cb == this) continue;
			if (cb->IsSelected())
				cb->UnsetIsSelected();
		}
	}

	if (bRoom)
	{
		for (size_t i = 0; i < pParent->pParent->pChild.size(); i++)
		{
			if (pParent->pParent->pChild.at(i)->pChild.back()->IsSelected())
			{
				pParent->pParent->SetRoom(i);
				break;
			}
		}
	}
	else
	{//if its not a room its a layer

	}	
}

void RoomLayerBox::Interact(D2D1_POINT_2F p)
{
	if (IsHidden()) return;
	//toggle associated room or layer's visiblity to on / off
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
	bSelected ^= true;
	if (bRoom)
	{
		pvVisibleRoom->at(uRoomNumber) = bSelected;
	}
}

void RoomLayerBox::Interact()
{
	if (IsHidden()) return;
	Interact(*pMouseCoordinates);
}