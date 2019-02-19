#include "Buttons.h"
#include "SideMenu.h"

const bool Buttons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
				{
					if (child->pParent) child->pParent->ChangeMode();
				}
				else
					if (!child->Interact(p)) return false;
				return true;
			}
		}
	}
	//Interaction... look into what exactly to do with this later
	if(bEnableSelection) bSelected ^= true;
	return true;
}

const bool Buttons::Interact()
{
	if (IsHidden()) return true;
	if (!Interact(*pMouseCoordinates)) return false;
	return true;
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

const bool Checkbox::Interact()
{
	if (IsHidden()) return true;
	if (IsSelected()) return true;

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
		for (unsigned int i = 0; i < pParent->pParent->pChild.size(); i++)
		{
			if (pParent) if (pParent->pParent) if (pParent->pParent->pChild.at(i)) if (pParent->pParent->pChild.at(i)->pChild.back())
			{
				if (pParent->pParent->pChild.at(i)->pChild.back()->IsSelected())
				{
					pParent->pParent->SetRoom(i);
					for (auto& layer : static_cast<SideMenu*>(pParent->pParent->pParent)->pLayersMenu)
					{
						layer->SetHidden();
					}
					static_cast<SideMenu*>(pParent->pParent->pParent)->pLayersMenu.at(i)->SetUnhidden();
					static_cast<SideMenu*>(pParent->pParent->pParent)->RealignAddLayerButton(i);
					break;
				}
			}
		}
	}
	else
	{//if its not a room its a layer
		for (unsigned int i = 0; i < pParent->pParent->pChild.size(); i++)
		{
			if (pParent) if (pParent->pParent) if (pParent->pParent->pChild.at(i)) if (pParent->pParent->pChild.at(i)->pChild.back())
			{
				if (pParent->pParent->pChild.at(i)->pChild.back()->IsSelected())
				{
					pParent->pParent->SetLayer(i);
					static_cast<SideMenu*>(pParent->pParent->pParent)->RealignAddLayerButton();
					break;
				}
			}
		}
	}	
	return false;
}

const bool RoomLayerBox::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	//toggle associated room or layer's visiblity to on / off
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				if (!child->Interact(p)) return false;
				return true;
			}
		}
	}	
	bSelected ^= true;
	if (bRoom)
	{
		if (uRoomNumber < pvVisibleRoom->size())
			pvVisibleRoom->at(uRoomNumber) = bSelected;
	}
	else
	{
		if (pvVisibleLayer) if (uRoomNumber < pvVisibleLayer->size()) if (uLayerNumber < pvVisibleLayer->at(uRoomNumber).size())
			pvVisibleLayer->at(uRoomNumber).at(uLayerNumber) = bSelected;
	}
	return true;
}

const bool RoomLayerBox::Interact()
{
	if (IsHidden()) return true;
	if (!Interact(*pMouseCoordinates)) return false;
	return true;
}

const bool AddItem::Interact(const D2D1_POINT_2F p)
{
	if (bRoom)
	{
		SideMenu* parent = (static_cast<SideMenu*>(pParent));
		unsigned int roomnum = parent->GetSelectedRoomNumber();
		unsigned int layernum = parent->GetSelectedLayerNumber(roomnum);
		//its a room
		if (!parent) return true;
		if (!*parent->vSelectRoomsandLayers) return true;
		if (!parent->pVisibleLayers) return true;
		if (!parent->pVisibleRooms) return true;
		(*parent->vSelectRoomsandLayers)->push_back(std::vector< std::vector<SpritePointer*>>());
		parent->pVisibleRooms->push_back(true);
		parent->pVisibleLayers->push_back(std::vector<bool>());
		parent->CreateLayerMenuSection();
		(*parent->vSelectRoomsandLayers)->back().push_back(std::vector<SpritePointer*>());
		parent->pVisibleLayers->back().push_back(true);
		parent->CreateLayer((*parent->vSelectRoomsandLayers)->size() - 1);
		parent->CreateRoomButton(pParent->pTransforms, pParent->pClientRect);
		parent->CreateLayerButton(pParent->pTransforms, pParent->pClientRect, (*parent->vSelectRoomsandLayers)->size() - 1);
		parent->RealignAddLayerButton(parent->GetSelectedRoomNumber());
		parent->SetRoom(roomnum);
		parent->SetLayer(layernum);
		return false;
	}
	else
	{
		//its a layer		
		SideMenu* parent = static_cast<SideMenu*>(pParent);
		if (!parent) return true;
		unsigned int uRoom = parent->GetSelectedRoomNumber();
		unsigned int uLayer = parent->GetSelectedLayerNumber(uRoom);
		if (!*parent->vSelectRoomsandLayers) return true;
		if (!parent->pVisibleLayers) return true;

		if (uRoom < (*parent->vSelectRoomsandLayers)->size()) (*parent->vSelectRoomsandLayers)->at(uRoom).push_back(std::vector<SpritePointer*>());
		parent->pVisibleLayers->at(uRoom).push_back(true);
		parent->CreateLayer(uRoom);
		parent->CreateLayerButton(parent->pTransforms, parent->pClientRect, uRoom);
		if (uRoom < parent->pLayersMenu.size()) parent->pLayersMenu[uRoom]->SetUnhidden();
		parent->RealignAddLayerButton(uRoom);
		parent->SetRoom(uRoom);
		parent->SetLayer(uLayer);
		return false;
	}
	return true;
}

const bool TypeButtons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	if (!pParent) return true;

	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				if (!child->Interact(p)) return false;
				return true;
			}
		}
	}

	if (bEnableSelection)
	{
		SideMenu* parent = static_cast<SideMenu*>(pParent);
		
		for (unsigned int i = 0; i < parent->CategoryMenu->pChild.size(); i++)
		{
			if (parent->CategoryMenu->pChild.at(i) == this)
			{
				parent->CategoryMenu->vSubsections.at(i)->SetUnhidden();
				continue;
			}
			parent->CategoryMenu->pChild.at(i)->UnsetIsSelected();
			parent->CategoryMenu->vSubsections.at(i)->SetHidden();
		}

		SetIsSelected();
	}
	return true;
}

const bool SubsectionButtons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	if (!pParent) return true;

	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				if (!child->Interact(p)) return false;
				return true;
			}
		}
	}

	if (bEnableSelection)
	{
		MenuSection* parent = static_cast<MenuSection*>(pParent);
		for (unsigned int i = 0; i < parent->pChild.size(); i++)
		{
			if (parent->pChild.at(i) == this)
			{
				parent->vSubsections.at(i)->SetUnhidden();
				continue;
			}
			parent->pChild.at(i)->UnsetIsSelected();
			parent->vSubsections.at(i)->SetHidden();
			for (auto& t : parent->vSubsections.at(i)->pChild)
				t->UnsetIsSelected();
		}
		SetIsSelected();
	}
	return true;
}

const bool SpriteItemButtons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	if (!pParent) return true;

	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				if (!child->Interact(p)) return false;
				return true;
			}
		}
	}

	if (bEnableSelection)
	{
		MenuSection* parent = static_cast<MenuSection*>(pParent);
		for (unsigned int i = 0; i < parent->pChild.size(); i++)
		{
			if (parent->pChild.at(i) == this)
				continue;
			parent->pChild.at(i)->UnsetIsSelected();
		}
		SetIsSelected();
	}

	if (ppSelectedSprite)
	{
		SafeDelete(&(*ppSelectedSprite));
		(*ppSelectedSprite) = new SpritePointer(pPiecesW, Location());
		(*ppSelectedSprite)->SetCreatureSize(pPiecesW->GetSize());
	}
	return true;
}

void SpriteItemButtons::Draw()
{
	if (IsHidden()) return;
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, D2D1::ColorF(0.0f, 0.0f, 0.0f), m_Radius.width, m_Radius.height, fThickness);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_Color, m_Radius.width, m_Radius.height);
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), GetLabel(), m_Dest, TextColor, m_Alignment, m_pAlignment);
	if (pPiecesW->GetPortrait())
	{
		gfx->DrawBitmap(gfx->GetCompatibleTarget(), pPiecesW->GetPortrait()->GetBitmap(), m_Dest, 1.0f, pPiecesW->GetPortrait()->GetFrame());
	}
	else if (pPiecesW->GetSprite())
	{
		gfx->DrawBitmap(gfx->GetCompatibleTarget(), pPiecesW->GetSprite()->GetBitmap(), m_Dest, 1.0f, pPiecesW->GetSprite()->GetFrame());
	}
	if (PointInRect()) gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_HighlightColor, m_Radius.width, m_Radius.height);
	if (bSelected) gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_SelectedColor, m_Radius.width, m_Radius.height);
	for (auto& child : pChild)
	{
		if (child) child->Draw();
	}
}

bool SpriteItemButtons::PointInRect()
{
	return PointInRect(*pMouseCoordinates);
}

bool SpriteItemButtons::PointInRect(const D2D1_POINT_2F p)
{
	D2D1::Matrix3x2F t = *pMatrix;
	t.Invert();
	D2D1_POINT_2F transformedPoint = t.TransformPoint(p);
	return (transformedPoint.x >= m_Dest.left && transformedPoint.x <= m_Dest.right && transformedPoint.y >= m_Dest.top && transformedPoint.y <= m_Dest.bottom);
}

const bool SizeMenuButtons::Interact()
{
	if (IsHidden()) return true;
	if (!pSizeMenu) return true;

	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect())
			{
				if (!child->Interact()) return false;
				return true;
			}
		}
	}

	if (bEnableSelection)
	{
		SizeMenu* parent = static_cast<SizeMenu*>(pSizeMenu);
		for (unsigned int i = 0; i < parent->vpChild.size(); i++)
		{
			if (parent->vpChild.at(i) == this)
				continue;
			parent->vpChild.at(i)->UnsetIsSelected();
		}
		SetIsSelected();
	}
	return true;
}