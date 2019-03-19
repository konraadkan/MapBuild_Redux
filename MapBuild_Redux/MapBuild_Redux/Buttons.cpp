#include "Buttons.h"
#include "SideMenu.h"
#include "resource.h"

const bool Buttons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	UpdateLog(L"Buttons::Interact(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				return (!child->Interact(p));
			}
		}
	}
	if(bEnableSelection) bSelected ^= true;
	return true;
}

const bool Buttons::AlternateInteract(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	UpdateLog(L"Buttons::AlternateInteract(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				if (!child->AlternateInteract(p)) return false;
				return true;
			}
		}
	}
	return true;
}

const bool Buttons::Interact()
{
	if (IsHidden()) return true;
	if (!Interact(*pMouseCoordinates)) return false;
	UpdateLog(L"Buttons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	return true;
}

const bool CounterButton::Interact()
{
	if (IsHidden()) return true;
	if (!Interact(*pMouseCoordinates)) return false;
	UpdateLog(L"CounterButton::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	return true;
}

const bool CounterButton::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	UpdateLog(L"CounterButton::Interact(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));

	if (!(*ppTurnCounter)->IsHidden())
	{
		if ((*ppTurnCounter)->GetTurnNumber() > 0)
		{
			(*ppTurnCounter)->ResetCounter();
			(*ppTurnCounter)->Update();
			(*ppFirstPiecesW) = nullptr;
		}
		else
		{
			if (bEnableSelection) bSelected = false;
			(*ppTurnCounter)->SetHidden();
			(*ppFirstPiecesW) = nullptr;
		}
	}
	else
	{
		(*ppTurnCounter)->UnsetHidden();
		if (bEnableSelection) bSelected = true;
		(*ppFirstPiecesW) = nullptr;
	}
	if (ppShowCounter) **ppShowCounter = !(*ppTurnCounter)->IsHidden();
	return true;
}

const bool Buttons::AlternateInteract()
{
	if (IsHidden()) return true;
	if (!AlternateInteract(*pMouseCoordinates)) return false;
	UpdateLog(L"Buttons::AlternateInteract()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
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
		UpdateLog(L"Buttons::Interact()[bRoom=true]", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
		for (uint32_t i = 0; i < pParent->pParent->pChild.size(); i++)
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

					for (size_t j = 0; j < static_cast<SideMenu*>(pParent->pParent->pParent)->pLayersMenu.at(i)->pChild.size(); j++)
					{
						if (static_cast<SideMenu*>(pParent->pParent->pParent)->pLayersMenu.at(i)->pChild.at(j)->pChild.back()->IsSelected())
						{
							pParent->pParent->pParent->SetLayer(j);
							break;
						}
					}

					break;
				}
			}
		}
	}
	else
	{//if its not a room its a layer
		UpdateLog(L"Buttons::Interact()[bRoom=false]", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
		for (uint32_t i = 0; i < pParent->pParent->pChild.size(); i++)
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
		UpdateLog(L"RoomLayerBox::Interact(p) [bRoom=true]", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
		if (uRoomNumber < pvVisibleRoom->size())
			pvVisibleRoom->at(uRoomNumber) = bSelected;
	}
	else
	{
		UpdateLog(L"RoomLayerBox::Interact(p) [bRoom=false]", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
		if (pvVisibleLayer) if (uRoomNumber < pvVisibleLayer->size()) if (uLayerNumber < pvVisibleLayer->at(uRoomNumber).size())
			pvVisibleLayer->at(uRoomNumber).at(uLayerNumber) = bSelected;
	}
	return true;
}

const bool RoomLayerBox::Interact()
{
	if (IsHidden()) return true;
	if (!Interact(*pMouseCoordinates)) return false;
	UpdateLog(L"RoomLayerBox::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	return true;
}

const bool AddItem::Interact(const D2D1_POINT_2F p)
{
	if (bRoom)
	{
		UpdateLog(L"AddItem::Interact(p) [bRoom=true]", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
		SideMenu* parent = (static_cast<SideMenu*>(pParent));
		uint32_t roomnum = parent->GetSelectedRoomNumber();
		uint32_t layernum = parent->GetSelectedLayerNumber(roomnum);
		//its a room
		if (!parent) return true;
		if (!*parent->vSelectRoomsandLayers) return true;
		if (!parent->pSelectWallRoomsandLayers) return true;
		if (!parent->pVisibleLayers) return true;
		if (!parent->pVisibleRooms) return true;
		(*parent->vSelectRoomsandLayers)->push_back(std::vector< std::vector<SpritePointer*>>());
		(*parent->pSelectWallRoomsandLayers)->push_back(std::vector < std::vector<std::unique_ptr<Wall>>>());
		parent->pVisibleRooms->push_back(true);
		parent->pVisibleLayers->push_back(std::vector<bool>());
		parent->CreateLayerMenuSection();
		(*parent->vSelectRoomsandLayers)->back().push_back(std::vector<SpritePointer*>());
		(*parent->pSelectWallRoomsandLayers)->back().push_back(std::vector<std::unique_ptr<Wall>>());
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
		UpdateLog(L"AddItem::Interact(p) [bRoom=false]", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
		//its a layer		
		SideMenu* parent = static_cast<SideMenu*>(pParent);
		if (!parent) return true;
		uint32_t uRoom = parent->GetSelectedRoomNumber();
		uint32_t uLayer = parent->GetSelectedLayerNumber(uRoom);
		if (!*parent->vSelectRoomsandLayers) return true;
		if (!parent->pSelectWallRoomsandLayers) return true;
		if (!parent->pVisibleLayers) return true;

		if (uRoom < (*parent->vSelectRoomsandLayers)->size()) (*parent->vSelectRoomsandLayers)->at(uRoom).push_back(std::vector<SpritePointer*>());
		if (uRoom < (*parent->pSelectWallRoomsandLayers)->size()) ((*parent->pSelectWallRoomsandLayers)->at(uRoom).push_back(std::vector<std::unique_ptr<Wall>>()));
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
	UpdateLog(L"TypeButtons::Interact(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
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
		
		for (uint32_t i = 0; i < parent->CategoryMenu->pChild.size(); i++)
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
		if (pWallMenuSelected) *pWallMenuSelected = bIsWall;
	}
	return true;
}

const bool SubsectionButtons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	if (!pParent) return true;
	UpdateLog(L"SubsectionButtons::Interact(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
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
		for (uint32_t i = 0; i < parent->pChild.size(); i++)
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

const bool WallSubsectionButtons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	if (!pParent) return true;
	UpdateLog(L"WallSubsectionButtons::Interact(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
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
		for (uint32_t i = 0; i < parent->pChild.size(); i++)
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
		if (pUseTexture) *pUseTexture = bUseTexture;
	}
	return true;
}

const bool AoeSubsectionButtons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	if (!pParent) return true;
	UpdateLog(L"AoeSubsectionButtons::Interact(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
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
		for (uint32_t i = 0; i < parent->pChild.size(); i++)
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
		if (pType) *pType = mType;
	}
	return true;
}

const bool SpriteItemButtons::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	if (!pParent) return true;
	UpdateLog(L"SpriteItemButtons::Interact(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
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
		for (uint32_t i = 0; i < parent->pChild.size(); i++)
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
		(*ppSelectedSprite) = new SpritePointer(gfx, pPiecesW, Location(), pGridSquareSize);
		(*ppSelectedSprite)->SetCreatureSize(pPiecesW->GetSize());
	}
	return true;
}

const bool SpriteItemButtons::AlternateInteract(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	if (!pParent) return true;
	UpdateLog(L"SpriteItemButtons::AlternateInteract(p)", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				if (!child->AlternateInteract(p)) return false;
				return true;
			}
		}
	}
	if (pvInitativeList)
	{
		pvInitativeList->push_back(pPiecesW);
	}
	return true;
}

const bool SpriteItemButtons::AlternateInteract()
{
	if (IsHidden()) return true;
	if (!AlternateInteract(*pMouseCoordinates)) return false;
	UpdateLog(L"SpriteItemButtons::AlternateInteract()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	return true;
}

void SpriteItemButtons::Draw()
{
	if (IsHidden()) return;
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, D2D1::ColorF(0.0f, 0.0f, 0.0f), m_Radius.width, m_Radius.height, fThickness);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_Color, m_Radius.width, m_Radius.height);
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), GetLabel(), m_Dest, TextColor, m_Alignment, m_pAlignment);
	if (pPiecesW)
	{
		if (pPiecesW->GetPortrait())
		{
			gfx->DrawBitmap(gfx->GetCompatibleTarget(), pPiecesW->GetPortrait()->GetBitmap(), m_Dest, 1.0f, pPiecesW->GetPortrait()->GetFrame());
		}
		else if (pPiecesW->GetSprite())
		{
			gfx->DrawBitmap(gfx->GetCompatibleTarget(), pPiecesW->GetSprite()->GetBitmap(), m_Dest, 1.0f, pPiecesW->GetSprite()->GetFrame());
		}
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
	UpdateLog(L"SizeMenuButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
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
		for (uint32_t i = 0; i < parent->vpChild.size(); i++)
		{
			if (parent->vpChild.at(i) == this)
				continue;
			parent->vpChild.at(i)->UnsetIsSelected();
		}
		SetIsSelected();
	}
	return true;
}

void ColorButtons::Draw()
{
	if (bHide) return;
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, DrawColor, m_Radius.width, m_Radius.height);
	if (bSelected) gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_SelectedColor, m_Radius.width, m_Radius.height);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, DrawColor, fThickness);
}

const bool ColorButtons::Interact()
{
	if (IsHidden()) return true;
	UpdateLog(L"ColorButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	SafeDelete(&(*ppSelectedSprite));
	if (bEnableSelection)
	{
		for (auto& child : pParent->pChild)
		{
			child->UnsetIsSelected();
		}
		SetIsSelected();
	}
	return true;
}

void InitiativeListButtons::Draw()
{
	if (IsHidden()) return;	
	if (pPiece->GetPortrait())
	{
		gfx->DrawBitmap(gfx->GetCompatibleTarget(), pPiece->GetPortrait()->GetBitmap(), D2D1::RectF(m_Dest.left, m_Dest.top, m_Dest.left + 125.0f, m_Dest.bottom), 1.0f, pPiece->GetPortrait()->GetFrame());
	}
	else if (pPiece->GetSprite())
	{
		gfx->DrawBitmap(gfx->GetCompatibleTarget(), pPiece->GetSprite()->GetBitmap(), D2D1::RectF(m_Dest.left, m_Dest.top, m_Dest.left + 125.0f, m_Dest.bottom), 1.0f, pPiece->GetSprite()->GetFrame());
	}
	gfx->OutputText(gfx->GetCompatibleTarget(), pPiece->GetName().c_str(), D2D1::RectF(m_Dest.left + 125.0f, m_Dest.top, m_Dest.right, m_Dest.bottom), TextColor, this->GetTextAlignment(), this->GetParagraphAlignment());
	if (IsSelected()) gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_HighlightColor, m_Radius.width, m_Radius.height);
}

const bool InitiativeListButtons::Interact()
{
	if (IsHidden()) return true;
	UpdateLog(L"InitiativeListButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	bool wasselected = bSelected;

	if (bEnableSelection)
	{
		for (auto& child : pParent->pChild)
		{
			/*if (child->IsSelected())
			{
				child->UnsetIsSelected();
			}*/
		}
		if (!wasselected) SetIsSelected();
		else UnsetIsSelected();
	}
	return true;
}

const bool InitiativeListButtons::AlternateInteract()
{
	if (IsHidden()) return true;
	UpdateLog(L"InitiativeListButtons::AlternateInteract()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	if (bEnableSelection)
	{
		for (auto& child : pParent->pChild)
		{
			child->UnsetIsSelected();
		}
		SetIsSelected();
	}
	return true;
}

void NextTurnButtons::Draw()
{
	if (!pGeometry) return;
	gfx->FillGeometry(gfx->GetCompatibleTarget(), pGeometry, D2D1::ColorF(1.0f, 1.0f, 0.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), 2.0f);
}

const bool NextTurnButtons::Interact()
{
	if (IsHidden()) return true;
	UpdateLog(L"NextTurnButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	if (PointInRect())
		if (pSideMenu) static_cast<SideMenu*>(pSideMenu)->NextTurn();
	return false;
}

const bool NextTurnButtons::AlternateInteract()
{
	return true;
}

void PreviousTurnButtons::Draw()
{
	if (!pGeometry) return;
	gfx->FillGeometry(gfx->GetCompatibleTarget(), pGeometry, D2D1::ColorF(1.0f, 1.0f, 0.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), 2.0f);
}

const bool PreviousTurnButtons::Interact()
{
	if (IsHidden()) return true;
	UpdateLog(L"PreviousTurnButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	if (PointInRect())
	{
		if (pSideMenu) static_cast<SideMenu*>(pSideMenu)->PreviousTurn();
			return false;
	}
	return true;
}

const bool AttachObjectButtons::Interact()
{
	if (IsHidden()) return true;
	if (!pAttachObject) return true;
	UpdateLog(L"AttachObjectButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	if (PointInRect())
	{
		*pAttachObject ^= true;
		if (*pAttachObject) SetIsSelected(); 
		else UnsetIsSelected();
	}
	return true;
}

const bool PreviousTurnButtons::AlternateInteract()
{
	return true;
}

void PreviousTurnButtons::BuildGeometry()
{
	SafeRelease(&pGeometry);
	if (!SUCCEEDED(gfx->GetFactory()->CreatePathGeometry(&pGeometry)))
	{
		pGeometry = nullptr;
		return;
	}
	
	ID2D1GeometrySink* pSink = nullptr;
	if (SUCCEEDED(pGeometry->Open(&pSink)))
	{
		pSink->BeginFigure(D2D1::Point2F(m_Dest.right, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.33f), D2D1_FIGURE_BEGIN_FILLED);
		pSink->AddLine(D2D1::Point2F(m_Dest.right - (m_Dest.right - m_Dest.left) * 0.20f, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.33f));
		pSink->AddLine(D2D1::Point2F(m_Dest.right - (m_Dest.right - m_Dest.left) * 0.20f, m_Dest.top));
		pSink->AddLine(D2D1::Point2F(m_Dest.left, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.5f));
		pSink->AddLine(D2D1::Point2F(m_Dest.right - (m_Dest.right - m_Dest.left) * 0.20f, m_Dest.bottom));
		pSink->AddLine(D2D1::Point2F(m_Dest.right - (m_Dest.right - m_Dest.left) * 0.20f, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.66f));
		pSink->AddLine(D2D1::Point2F(m_Dest.right, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.66f));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		pSink->Close();
		SafeRelease(&pSink);
		return;
	}
	SafeRelease(&pGeometry);
}

void NextTurnButtons::BuildGeometry()
{
	SafeRelease(&pGeometry);
	if (!SUCCEEDED(gfx->GetFactory()->CreatePathGeometry(&pGeometry)))
	{
		pGeometry = nullptr;
		return;
	}

	ID2D1GeometrySink* pSink = nullptr;
	if (SUCCEEDED(pGeometry->Open(&pSink)))
	{
		pSink->BeginFigure(D2D1::Point2F(m_Dest.left, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.33f), D2D1_FIGURE_BEGIN_FILLED);
		pSink->AddLine(D2D1::Point2F(m_Dest.left + (m_Dest.right - m_Dest.left) * 0.20f, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.33f));
		pSink->AddLine(D2D1::Point2F(m_Dest.left + (m_Dest.right - m_Dest.left) * 0.20f, m_Dest.top));
		pSink->AddLine(D2D1::Point2F(m_Dest.right, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.5f));
		pSink->AddLine(D2D1::Point2F(m_Dest.left + (m_Dest.right - m_Dest.left) * 0.20f, m_Dest.bottom));
		pSink->AddLine(D2D1::Point2F(m_Dest.left + (m_Dest.right - m_Dest.left) * 0.20f, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.66f));
		pSink->AddLine(D2D1::Point2F(m_Dest.left, m_Dest.top + (m_Dest.bottom - m_Dest.top) * 0.66f));
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
		pSink->Close();
		SafeRelease(&pSink);
		return;
	}
	SafeRelease(&pGeometry);
}

const bool ExitButtons::Interact()
{
	if (IsHidden()) return true;
	UpdateLog(L"ExitButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	if (PointInRect())
	{
		if (pExit) *pExit = true;
		return false;
	}
	return true;
}

const bool NewButtons::Interact()
{
	if (IsHidden()) return true;
	UpdateLog(L"NewButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	if (PointInRect())
	{
		if (pNew) *pNew = true;
		return false;
	}
	return true;
}

const bool SaveButtons::Interact()
{
	if (IsHidden()) return true;
	UpdateLog(L"SaveButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	if (PointInRect())
	{
		WPARAM wParam = ID_SAVE_CMD;
		LPARAM lParam = {};
		SendMessage(hWnd, WM_COMMAND, wParam, lParam);
	}
	return true;
}

const bool LoadButtons::Interact()
{
	if (IsHidden()) return true;
	UpdateLog(L"LoadButtons::Interact()", L"Buttons.cpp", static_cast<int32_t>(__LINE__));
	if (PointInRect())
	{
		WPARAM wParam = ID_OPEN_CMD;
		LPARAM lParam = {};
		SendMessage(hWnd, WM_COMMAND, wParam, lParam);
	}
	return true;
}

const bool LockToGridButton::Interact()
{
	if (IsHidden()) return true;
	if (!Interact(*pMouseCoordinates)) return false;
	return true;
}

const bool LockToGridButton::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	UpdateLog(L"LockToGridButton::Interact()", L"Buttons.cpp", static_cast<uint32_t>(__LINE__));
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				return child->Interact(p);				
			}
		}
	}
	
	if (*pLockToGrid)
	{
		*pLockToGrid = false;
		bSelected = false;
	}
	else
	{
		*pLockToGrid = true;
		bSelected = true;
	}

	return true;
}

const bool ToggleInitiativeButton::Interact(const D2D1_POINT_2F p)
{
	if (IsHidden()) return true;
	UpdateLog(L"ToggleInitiativeButton::Interact(p)", L"Buttons.cpp", static_cast<uint32_t>(__LINE__));
	for (auto& child : pChild)
	{
		if (child)
		{
			if (child->PointInRect(p))
			{
				return child->Interact(p);
			}
		}
	}

	if (*pLockToGrid)
	{
		*pLockToGrid = false;
		bSelected = true;
	}
	else
	{
		*pLockToGrid = true;
		bSelected = false;
	}

	return true;
}