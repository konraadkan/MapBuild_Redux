#include "SideMenu.h"
#include "BaseLevel.h"

void SideMenu::Draw()
{
	if (bBuildMode) DrawBuildMode();
	if (!bBuildMode) DrawInitiativeMode();

	if (bBuildMode)
	{
		for (auto& c : pMenuSections)
		{
			c->Draw();
		}
	}
	else
	{
		pOptionsMenu->Draw();
		if (InitiativeList) InitiativeList->Draw();
	}
}

void SideMenu::DrawBuildMode()
{
	for (auto& child : BuildModeObjects)
	{
		if (child) 
			if(child->IsBack()) child->Draw();
	}
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, D2D1::ColorF(0.0f, 0.0f, 0.0f), m_Radius.width, m_Radius.height, fThickness);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_Color, m_Radius.width, m_Radius.height);
	for (auto& child : BuildModeObjects)
	{
		if (child)
			if (!child->IsBack()) child->Draw();
	}
}

void SideMenu::DrawInitiativeMode()
{
	for (auto& child : InitiativeModeObjects)
	{
		if (child)
			if (child->IsBack()) child->Draw();
	}
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), m_Dest, D2D1::ColorF(0.0f, 0.0f, 0.0f), m_Radius.width, m_Radius.height, fThickness);
	gfx->FillRoundedRect(gfx->GetCompatibleTarget(), m_Dest, m_Color, m_Radius.width, m_Radius.height);
	for (auto& child : InitiativeModeObjects)
	{
		if (child)
			if (!child->IsBack()) child->Draw();
	}
}

const bool SideMenu::AlternateInteract()
{
	std::vector<InteractObjects*>* vIo = bBuildMode ? &BuildModeObjects : &InitiativeModeObjects;
	for (auto& child : *vIo)
	{
		if (child->PointInRect())
		{
			if (!_wcsicmp(child->GetLabel(), L"ShowHide"))
			{
				if (child->PointInSector(Sector::West)) bHide ? SetUnhidden() : SetHidden();
			}
			else if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
			{
				ChangeMode();
			}
			else if (!child->Interact()) return false;
		}
	}
	for (size_t i = 0; i < pMenuSections.size(); i++)
	{
		if (pMenuSections.at(i)->PointInRect())
		{
			if (bBuildMode) if (!pMenuSections.at(i)->AlternateInteract()) return false;
		}
	}
	return true;
}

const bool SideMenu::AlternateInteract(const D2D1_POINT_2F p)
{
	std::vector<InteractObjects*>* vIo = bBuildMode ? &BuildModeObjects : &InitiativeModeObjects;
	for (auto& child : *vIo)
	{
		if (child->PointInRect(p))
		{
			if (!_wcsicmp(child->GetLabel(), L"ShowHide"))
			{
				if (child->PointInSector(Sector::West)) bHide ? SetUnhidden() : SetHidden();
			}
			else if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
			{
				ChangeMode();
			}
			else if (!child->Interact()) return false;
		}
	}
	for (size_t i = 0; i < pMenuSections.size(); i++)
	{
		if (pMenuSections.at(i)->PointInRect(p))
		{
			if (bBuildMode) if (!pMenuSections.at(i)->AlternateInteract()) return false;
		}
	}
	return true;
}

const bool SideMenu::Interact()
{
	std::vector<InteractObjects*>* vIo = bBuildMode ? &BuildModeObjects : &InitiativeModeObjects;
	for (auto& child : *vIo)
	{
		if (child->PointInRect())
		{
			if (!_wcsicmp(child->GetLabel(), L"ShowHide"))
			{
				if (child->PointInSector(Sector::West)) bHide ? SetUnhidden() : SetHidden();
			}
			else if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
			{
				ChangeMode();
			}
			else if (!child->Interact()) return false;
		}
	}
	for (size_t i = 0; i < pMenuSections.size(); i++)
	{
		if (pMenuSections.at(i)->PointInRect())
		{
			if (!_wcsicmp(pMenuSections.at(i)->GetLabel(), L"Options"))
			{
				for (auto& child : pMenuSections.at(i)->pChild)
				{
					if (child->PointInRect())
					{
						if (!_wcsicmp(child->GetLabel(), L"Lock To Grid"))
						{
							if (pBaseLevel) static_cast<BaseLevel*>(pBaseLevel)->ToggleLockToGrid();
						}
						else if (!_wcsicmp(child->GetLabel(), L"Toggle Keep Aspect"))
						{
							if (pBaseLevel) static_cast<BaseLevel*>(pBaseLevel)->ToggleKeepAspect();
						}
						else if (!_wcsicmp(child->GetLabel(), L"Grid on Top"))
						{
							if (pBaseLevel) static_cast<BaseLevel*>(pBaseLevel)->ToggleGridOnTop();
						}
						else if (!_wcsicmp(child->GetLabel(), L"Toggle Initiative"))
						{
							ChangeMode();
						}
					}
				}
			}
			if (bBuildMode) if (!pMenuSections.at(i)->Interact()) return false;
		}
	}
	return true;
}

const bool SideMenu::Interact(const D2D1_POINT_2F p)
{
	std::vector<InteractObjects*>* vIo = bBuildMode ? &BuildModeObjects : &InitiativeModeObjects;
	for (auto& child : *vIo)
	{
		if (child->PointInRect(p))
		{
			if (!_wcsicmp(child->GetLabel(), L"ShowHide"))
			{
				if (child->PointInSector(Sector::West, p)) bHide ? SetUnhidden() : SetHidden();
			}
			else if (!_wcsicmp(child->GetLabel(), L"Options"))
			{
				if (pBaseLevel)
				{
					for (auto c : child->pChild)
					{
						if (c->PointInRect(p))
						{
							c->Interact(p);
							if (!_wcsicmp(c->GetLabel(), L"Lock To Grid")) static_cast<BaseLevel*>(pBaseLevel)->ToggleLockToGrid();
							else if (!_wcsicmp(c->GetLabel(), L"Toggle Keep Aspect")) static_cast<BaseLevel*>(pBaseLevel)->ToggleKeepAspect();
						}
					}
				}
			}
			else if (!child->Interact(p)) return false;
			return true;
		}
	}
	return true;
}

bool SideMenu::PointInRect(const D2D1_POINT_2F p)
{
	m_ExpandedDest = D2D1::RectF(m_Dest.left - pChild.front()->GetSize().width, m_Dest.top, m_Dest.right, m_Dest.bottom);
	return (p.x >= m_ExpandedDest.left && p.x <= m_ExpandedDest.right && p.y >= m_ExpandedDest.top && p.y <= m_ExpandedDest.bottom);
}

const D2D1_RECT_F SideMenu::GetNextButtonRect(const MenuItemType ItemType)
{
	//returns where to place the next button based on where previous buttons are currently located
	return CategoryStartPoints[static_cast<int>(ItemType)];
}

void SideMenu::UpdateNextButtonRect(MenuItemType type)
{	
	bool bShiftDown = false;
	D2D1_SIZE_F menusize = D2D1::SizeF(GetMenuItemWidth(type), GetMenuItemHeight(type));
	CategoryStartPoints[static_cast<int>(type)].left = CategoryStartPoints[static_cast<int>(type)].right + 2.0f;
	CategoryStartPoints[static_cast<int>(type)].right = CategoryStartPoints[static_cast<int>(type)].left + menusize.width;

	if (CategoryStartPoints[static_cast<int>(type)].right >= m_Dest.right)
	{
		bShiftDown = true;
		CategoryStartPoints[static_cast<int>(type)].left = m_Dest.left + 0.2f;
		CategoryStartPoints[static_cast<int>(type)].right = m_Dest.left + GetMenuItemWidth(static_cast<MenuItemType>(type));
		CategoryStartPoints[static_cast<int>(type)].top = CategoryStartPoints[static_cast<int>(type)].bottom + 2.0f;
		CategoryStartPoints[static_cast<int>(type)].bottom = CategoryStartPoints[static_cast<int>(type)].top + menusize.height;
	}

	if (bShiftDown)
	{
		for (int i = static_cast<int>(type) + 1; i < static_cast<int>(MenuItemType::Error); i++)
		{
			CategoryStartPoints[i].top += menusize.height;
			CategoryStartPoints[i].bottom += menusize.height;
		}
	}
}

SideMenu::SideMenu(const D2D1_RECT_F targetDest, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, std::vector< std::vector<SpritePointer*>>** const ppRoom, std::vector<SpritePointer*>** const ppLayer,
	std::vector< std::vector< std::vector<SpritePointer*>>>** const ppRL, std::vector<bool>* const VisibleRooms, std::vector< std::vector<bool>>* const VisibleLayers, SpritePointer** const ppsprite, std::vector< std::vector< std::vector<std::unique_ptr<Wall>>>>** const ppW,
	std::vector< std::vector<std::unique_ptr<Wall>>>** const ppSWR, std::vector<std::unique_ptr<Wall>>** const ppSWL) : ppSelectedWallRoom(ppSWR), ppSelectedWallLayer(ppSWL), pSelectedRoom(ppRoom), pSelectWallRoomsandLayers(ppW), pSelectedLayer(ppLayer), vSelectRoomsandLayers(ppRL), ppSelectedSprite(ppsprite), Buttons(graphics, Transform, area, p)
{
	pVisibleLayers = VisibleLayers;
	pVisibleRooms = VisibleRooms;

	//Options Section
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, targetDest.top + 5.0f));
	FillRect(CategoryStartPoints.back(), OptionMenuSize);
	//Main Category
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, CategoryStartPoints.back().bottom + SeperationDistance));
	FillRect(CategoryStartPoints.back(), MainMenuSize);
	//Sub Category
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, CategoryStartPoints.back().bottom + SeperationDistance));
	FillRect(CategoryStartPoints.back(), SubMenuSize);
	//Sizes
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, CategoryStartPoints.back().bottom + SeperationDistance));
	FillRect(CategoryStartPoints.back(), SizeMenuSize);
	//Items
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, CategoryStartPoints.back().bottom + SeperationDistance));
	FillRect(CategoryStartPoints.back(), ItemMenuSize);
	//Rooms
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, CategoryStartPoints.back().bottom + SeperationDistance));
	FillRect(CategoryStartPoints.back(), RoomMenuSize);
	//Rooms Check Boxes
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, CategoryStartPoints.back().bottom + 3.0f));
	FillRect(CategoryStartPoints.back(), RoomCheckBoxMenuSize);
	//Layers
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, CategoryStartPoints.back().bottom + SeperationDistance));
	FillRect(CategoryStartPoints.back(), LayerMenuSize);
	//Layer Check Box
	CategoryStartPoints.push_back(D2D1::RectF(targetDest.left + 0.2f, CategoryStartPoints.back().bottom + 3.0f));
	FillRect(CategoryStartPoints.back(), LayerCheckBoxMenuSize);

	SetDest(targetDest);
	SetColor(D2D1::ColorF(0.75f, 0.80f, 0.82f));
	pChild.push_back(new ClassShapes(ShapeTypes::Ellipses, gfx, false, Transform, pClientRect, pMouseCoordinates, true));
	AddInitiativeObject(pChild.back());
	AddBuildObject(pChild.back());
	pChild.back()->SetRadius(D2D1::SizeF(15.0f, (m_Dest.bottom + m_Dest.top) * 0.5f));
	pChild.back()->SetCenter(D2D1::Point2F(m_Dest.left, (m_Dest.bottom + m_Dest.top) * 0.5f));
	pChild.back()->SetFill();
	pChild.back()->SetColor(D2D1::ColorF(0.0f, 0.2f, 0.8f, 0.95f));
	pChild.back()->SetLabel(L"ShowHide");
	pChild.back()->pChild.push_back(new ClassShapes(ShapeTypes::Custom, gfx, false, Transform, pClientRect, pMouseCoordinates));
	std::queue<D2D1_POINT_2F> ps;
	ps.push(D2D1::Point2F(m_Dest.left - 12.0f, (m_Dest.bottom + m_Dest.top) * 0.5f));
	ps.push(D2D1::Point2F(m_Dest.left - 3.0f, (m_Dest.bottom + m_Dest.top) * 0.5f - 15.0f));
	ps.push(D2D1::Point2F(m_Dest.left - 3.0f, (m_Dest.bottom + m_Dest.top) * 0.5f + 15.0f));
	pChild.back()->pChild.back()->BuildCustomShape(ps, D2D1::ColorF(1.0f, 1.0f, 1.0f));
	pChild.back()->pChild.back()->SetLabel(L"ShowHideTriangle");
	pChild.back()->pChild.back()->SetFill();
	mRealRect = D2D1::RectF(m_Dest.left - 15.0f, m_Dest.top, m_Dest.right, m_Dest.bottom);
	
	pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f,m_Dest.right + (m_Dest.right - m_Dest.left), OptionMenuSize.height + 3.0f), 0.0f, L"Options", false));
	pOptionsMenu = pMenuSections.back();
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true, true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Grid on Top", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle PC Colors", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Add Custom Colors", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle Initiative", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle Keep Aspect", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true, true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Turn Counter", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->SetBorderStyle(BorderStyle::Solid);
	//pOptionsMenu->SetHidden();

	D2D1_RECT_F LastRect = pMenuSections.back()->GetTranslatedRect();
	pMenuSections.back()->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, 0.0f));
	
	pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + (m_Dest.right - m_Dest.left), RoomMenuSize.height + 3.0f), 0.0f, L"Room"));
	pMenuSections.back()->SetBorderStyle(BorderStyle::Dotted);
	pMenuSections.back()->pParent = this;
	pMenuSections.back()->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, LastRect.bottom + SeperationDistance));
	pRoomsMenu = pMenuSections.back();
	pRoomsMenu->SetSelectedRoomPointer(pSelectedRoom);
	pRoomsMenu->SetSelectedLayerPointer(pSelectedLayer);
	pRoomsMenu->vSelectRoomsandLayers = vSelectRoomsandLayers;
	pRoomsMenu->SetSelectedWallRoomPointer(ppSelectedWallRoom);
	pRoomsMenu->SetSelectedWallLayerPointer(ppSelectedWallLayer);
	pRoomsMenu->pSelectWallRoomsandLayers = pSelectWallRoomsandLayers;

	pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + RoomMenuSize.width, m_Dest.top + 3.0f + RoomMenuSize.height), 0.0f, L"AddRemoveRoom"));
	pMenuSections.back()->pParent = this;
	pMenuSections.back()->SetTranslation(D2D1::SizeF(-RoomMenuSize.width, 0.0f));
	pAddRemoveRooms = pMenuSections.back();
	pAddRemoveRooms->SetSelectedLayerPointer(pSelectedLayer);
	pAddRemoveRooms->SetSelectedRoomPointer(pSelectedRoom);
	pAddRemoveRooms->vSelectRoomsandLayers = vSelectRoomsandLayers;
	pAddRemoveRooms->SetSelectedWallRoomPointer(ppSelectedWallRoom);
	pAddRemoveRooms->SetSelectedWallLayerPointer(ppSelectedWallLayer);
	pAddRemoveRooms->pSelectWallRoomsandLayers = pSelectWallRoomsandLayers;
	pAddRemoveRooms->AddChild(new AddItem(true, gfx, Transform, area, pMouseCoordinates, L"+", D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + RoomMenuSize.width, m_Dest.top + RoomMenuSize.height + 3.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), this), RoomMenuSize, RoomCheckBoxMenuSize.height + 2.0f);

	pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + LayerMenuSize.width, m_Dest.top + 3.0f + LayerMenuSize.height), 0.0f, L"AddRemoveLayer"));
	pMenuSections.back()->pParent = this;
	pMenuSections.back()->SetTranslation(D2D1::SizeF(-LayerMenuSize.width, 0.0f));
	pAddRemoveLayers = pMenuSections.back();
	pAddRemoveLayers->SetSelectedLayerPointer(pSelectedLayer);
	pAddRemoveLayers->SetSelectedRoomPointer(pSelectedRoom);
	pAddRemoveLayers->vSelectRoomsandLayers = vSelectRoomsandLayers;
	pAddRemoveLayers->SetSelectedWallRoomPointer(ppSelectedWallRoom);
	pAddRemoveLayers->SetSelectedWallLayerPointer(ppSelectedWallLayer);
	pAddRemoveLayers->pSelectWallRoomsandLayers = pSelectWallRoomsandLayers;
	pAddRemoveLayers->AddChild(new AddItem(false, gfx, Transform, area, pMouseCoordinates, L"+", D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + LayerMenuSize.width, m_Dest.top + LayerMenuSize.height + 3.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), this), LayerMenuSize, 2.0f);
}

void SideMenu::CreateRoomButton(D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area)
{
	if (!pRoomsMenu) return;
	if (!pVisibleRooms) return;
	pRoomsMenu->AddChild(new RoomLayerBox(gfx, Transform, area, pMouseCoordinates, std::to_wstring(pRoomsMenu->pChild.size()).c_str(), D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + (m_Dest.right - m_Dest.left), LayerMenuSize.height + 3.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(pRoomsMenu), true, true, D2D1::ColorF(1.0f, 0.0f, 0.0f), pRoomsMenu->pChild.size(), 0, pVisibleRooms), RoomMenuSize, RoomCheckBoxMenuSize.height + 2.0f);

	//build rect for selection check box
	D2D1_RECT_F rect = pRoomsMenu->pChild.back()->GetRect();
	float w = (rect.right - rect.left) * 0.25f;
	rect.left += w;
	rect.right -= w;
	rect.top = rect.bottom + 1.0f;
	rect.bottom = rect.top + RoomCheckBoxMenuSize.height;

	pRoomsMenu->pChild.back()->pChild.push_back(new Checkbox(gfx, Transform, area, pMouseCoordinates, L" ", rect, D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(pRoomsMenu->pChild.back()), true, pRoomsMenu->pChild.size() - 1 ? false : true));
	pRoomsMenu->pChild.back()->pChild.back()->SetInvertTransformPointer(pRoomsMenu->pChild.back()->GetInvTransforms());

	D2D1_SIZE_F AddItemTranslation = D2D1::SizeF();
	if (pRoomsMenu->pChild.back()->GetRect().right + RoomMenuSize.width > pRoomsMenu->GetRect().right)
	{
		AddItemTranslation = D2D1::SizeF(pRoomsMenu->Getdx(), pRoomsMenu->Getdy() + pRoomsMenu->pChild.back()->GetRect().bottom + RoomCheckBoxMenuSize.height);
		pRoomsMenu->SetDest(D2D1::RectF(pRoomsMenu->GetRect().left, pRoomsMenu->GetRect().top, pRoomsMenu->GetRect().right, pRoomsMenu->pChild.back()->GetRect().bottom + 1.0f));
		for (size_t w = 0; w < pLayersMenu.size(); w++)
		{
			pLayersMenu.at(w)->ShiftTranslation(D2D1::SizeF(0.0f, RoomMenuSize.height + RoomCheckBoxMenuSize.height + SeperationDistance + 1.0f));
		}
	}
	else
	{
		AddItemTranslation = D2D1::SizeF(pRoomsMenu->Getdx() + (pRoomsMenu->pChild.back()->GetRect().right - pRoomsMenu->GetRect().left), pRoomsMenu->Getdy() + pRoomsMenu->pChild.back()->GetRect().top);
		for (auto& w : pLayersMenu)
			w->SetTranslation(D2D1::SizeF(pRoomsMenu->Getdx(), pRoomsMenu->Getdy() + pRoomsMenu->pChild.back()->GetRect().bottom + RoomCheckBoxMenuSize.height + SeperationDistance + 1.0f));
	}
	pRoomsMenu->ResizeDest();
	pAddRemoveRooms->SetTranslation(AddItemTranslation);
}

void SideMenu::CreateLayer(size_t uRoomNumber)
{
	if (uRoomNumber >= pLayersMenu.size())
	{
		while (pLayersMenu.size() >= uRoomNumber)
		{
			CreateLayerMenuSection();
		}
	}
}

void SideMenu::CreateLayerButton(D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, size_t uRoomNumber)
{
	if (!pRoomsMenu) return;
	if (!pVisibleLayers) return;
	if (!pVisibleRooms) return;
	if (pLayersMenu.empty() || uRoomNumber >= pLayersMenu.size()) return;
	pLayersMenu[uRoomNumber]->AddChild(new RoomLayerBox(gfx, Transform, area, pMouseCoordinates, std::to_wstring(pLayersMenu[uRoomNumber]->pChild.size()).c_str(), D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + (m_Dest.right - m_Dest.left), LayerMenuSize.height + 3.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(pLayersMenu[uRoomNumber]), true, true, D2D1::ColorF(1.0f, 0.0f, 0.0f), uRoomNumber, pLayersMenu[uRoomNumber]->pChild.size(), pVisibleRooms, pVisibleLayers, false), LayerMenuSize, LayerCheckBoxMenuSize.height + 2.0f);

	if (uRoomNumber) pLayersMenu[uRoomNumber]->SetHidden();

	//build rect for selection check box
	D2D1_RECT_F rect = pLayersMenu[uRoomNumber]->pChild.back()->GetRect();
	float w = (rect.right - rect.left) * 0.25f;
	rect.left += w;
	rect.right -= w;
	rect.top = rect.bottom + 1.0f;
	rect.bottom = rect.top + LayerCheckBoxMenuSize.height;

	pLayersMenu[uRoomNumber]->pChild.back()->pChild.push_back(new Checkbox(gfx, Transform, area, pMouseCoordinates, L" ", rect, D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(pLayersMenu[uRoomNumber]->pChild.back()), true, pLayersMenu[uRoomNumber]->pChild.size() - 1 ? false : true, D2D1::ColorF(1.0f, 0.0f, 0.0f), false));
	pLayersMenu[uRoomNumber]->pChild.back()->pChild.back()->SetInvertTransformPointer(pLayersMenu[uRoomNumber]->pChild.back()->GetInvTransforms());
	pLayersMenu[uRoomNumber]->ResizeDest();
	pAddRemoveLayers->SetTranslation(D2D1::SizeF(pLayersMenu[uRoomNumber]->Getdx() + (pLayersMenu[uRoomNumber]->pChild.back()->GetRect().right - pLayersMenu[uRoomNumber]->GetRect().left), pLayersMenu[uRoomNumber]->Getdy() + pLayersMenu[uRoomNumber]->pChild.back()->GetRect().top));
}

void SideMenu::CreateLayerMenuSection()
{
	pMenuSections.push_back(new MenuSection(gfx, pTransforms, pClientRect, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + (m_Dest.right - m_Dest.left), m_Dest.top + LayerMenuSize.height + 3.0f), 0.0f, std::to_wstring(pRoomsMenu->pChild.size()).c_str()));
	pMenuSections.back()->SetBorderStyle(BorderStyle::Solid);
	D2D1_RECT_F rr = pRoomsMenu->GetRect();
	float dist = -pRoomsMenu->GetTranslatedRect().top;
	for (auto& player : pLayersMenu)
		player->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, dist + SeperationDistance + (rr.bottom - rr.top)));
	pMenuSections.back()->pParent = this;
	pMenuSections.back()->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, dist + SeperationDistance + (rr.bottom - rr.top)));
	pLayersMenu.push_back(pMenuSections.back());
	pLayersMenu.back()->SetSelectedRoomPointer(pSelectedRoom);
	pLayersMenu.back()->SetSelectedLayerPointer(pSelectedLayer);
	pLayersMenu.back()->SetSelectedWallLayerPointer(ppSelectedWallLayer);
	pLayersMenu.back()->SetSelectedWallRoomPointer(ppSelectedWallRoom);
	pLayersMenu.back()->vSelectRoomsandLayers = vSelectRoomsandLayers;
	pLayersMenu.back()->pSelectWallRoomsandLayers = pSelectWallRoomsandLayers;
}

void SideMenu::AddSeparation(const MenuItemType ItemType, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, const D2D1_COLOR_F color)
{
	pChild.push_back(new ClassShapes(ShapeTypes::Rectangles, gfx, false, Transform, area, pMouseCoordinates ));
	pChild.back()->SetDest(D2D1::RectF(m_Dest.left, GetNextButtonRect(ItemType).bottom + 2.0f, m_Dest.right, GetNextButtonRect(ItemType).bottom + 6.0f));
	pChild.back()->SetFill();
	pChild.back()->SetColor(color);
}

void SideMenu::WheelUp()
{
	for (auto& child : pChild)
	{
		if (child->PointInRect())
			child->WheelUp();
	}
}

void SideMenu::WheelDown()
{
	for (auto& child : pChild)
	{
		if (child->PointInRect())
			child->WheelDown();
	}
}

void SideMenu::Unload()
{
	while (pChild.size())
	{
		SafeDelete(&pChild.back());
		pChild.pop_back();
	}
}

void SideMenu::AddInitiativeObject(InteractObjects* const Io)
{
	InitiativeModeObjects.push_back(Io);
}

void SideMenu::AddBuildObject(InteractObjects* const Io)
{
	BuildModeObjects.push_back(Io);
}

const unsigned int SideMenu::GetSelectedRoomNumber()
{
	for (unsigned int i = 0; i < pRoomsMenu->pChild.size(); i++)
	{
		if (pRoomsMenu->pChild.at(i)->pChild.front()->IsSelected())
			return i;
	}
	return 0;
}

void SideMenu::SetRoom(size_t uRoomNumber)
{
	if (!pSelectedRoom || !pSelectedLayer || !ppSelectedWallLayer || !ppSelectedWallRoom)
	{
		MessageBoxW(nullptr, L"Could not update room pointer.", L"Error", MB_OK | MB_ICONERROR);
	}
	if (uRoomNumber < (*vSelectRoomsandLayers)->size())
	{
		*pSelectedRoom = &((*vSelectRoomsandLayers)->at(uRoomNumber));
		*pSelectedLayer = &((*pSelectedRoom)->front());
		*ppSelectedWallRoom = &(*pSelectWallRoomsandLayers)->at(uRoomNumber);
		*ppSelectedWallLayer = &(*ppSelectedWallRoom)->front();
	}
	else
	{
		MessageBoxW(nullptr, L"Unable to update room pointer.", L"Error", MB_OK | MB_ICONERROR);
	}
}

void SideMenu::SetLayer(size_t uLayerNumber)
{
	if (!pSelectedRoom || !pSelectedLayer || !ppSelectedWallLayer || !ppSelectedWallRoom)
	{
		MessageBoxW(nullptr, L"Could not update room pointer.", L"Error", MB_OK | MB_ICONERROR);
	}
	if (uLayerNumber < (*pSelectedRoom)->size())
	{
		*pSelectedLayer = &((*pSelectedRoom)->at(uLayerNumber));
		*ppSelectedWallLayer = &(*ppSelectedWallRoom)->at(uLayerNumber);
	}
	else
	{
		MessageBoxW(nullptr, L"Unable to update layer pointer.", L"Error", MB_OK | MB_ICONERROR);
	}
}

const unsigned int SideMenu::GetSelectedLayerNumber()
{
	return GetSelectedLayerNumber(GetSelectedRoomNumber());
}

const unsigned int SideMenu::GetSelectedLayerNumber(const unsigned int uRoom)
{
	for (unsigned int i = 0; i < pLayersMenu[uRoom]->pChild.size(); i++)
	{
		if (pLayersMenu[uRoom]->pChild.at(i)->pChild.front()->IsSelected())
			return i;
	}
	return 0;
}

void SideMenu::RealignAddLayerButton()
{
	RealignAddLayerButton(GetSelectedRoomNumber());
}

void SideMenu::RealignAddLayerButton(unsigned int uRoomNumber)
{
	D2D1_SIZE_F newtranslation = D2D1::SizeF();	

	if (pLayersMenu[uRoomNumber]->pChild.back()->GetRect().right + LayerMenuSize.width > pLayersMenu[uRoomNumber]->GetRect().right)
	{
		newtranslation.width = pLayersMenu[uRoomNumber]->Getdx();
		newtranslation.height = pLayersMenu[uRoomNumber]->Getdy() + pLayersMenu[uRoomNumber]->pChild.back()->GetRect().bottom + LayerCheckBoxMenuSize.height;
	}
	else
	{
		newtranslation.width = pLayersMenu[uRoomNumber]->Getdx() + pLayersMenu[uRoomNumber]->pChild.back()->GetRect().right - pLayersMenu[uRoomNumber]->GetRect().left;
		newtranslation.height = pLayersMenu[uRoomNumber]->Getdy() + pLayersMenu[uRoomNumber]->pChild.back()->GetRect().top;
	}
	pAddRemoveLayers->SetTranslation(newtranslation);
	if (CategoryMenu)
	{
		CategoryMenu->SetTranslation(D2D1::SizeF(-GetSize().width, pLayersMenu[GetSelectedRoomNumber()]->GetTranslatedRectNotInv().bottom + 2.0f));
		RealignCategories();
	}
}

void SideMenu::BuildCategories(std::vector<PiecesW>* const wPieces)
{
	if (!CategoryMenu)
	{
		CategoryMenu = new MenuSection(gfx, pTransforms, pClientRect, pMouseCoordinates, D2D1::RectF(pClientRect->right, pClientRect->top + 3.0f, pClientRect->right + GetSize().width, pClientRect->top + MainMenuSize.height + 3.0f), 0.0f, L"MainCategories");
		pMenuSections.push_back(CategoryMenu);
	}
		
	for (auto& piece : *wPieces)
	{
		bool foundit = false;
		for (unsigned int i = 0; i < CategoryMenu->pChild.size(); i++)
		{			
			if (!_wcsicmp(CategoryMenu->pChild.at(i)->GetLabel(), piece.GetType().c_str()))
			{
				foundit = true;
			}
		}
		if (!foundit)
		{
			CategoryMenu->AddChild(new TypeButtons(&bWallMenuSelected, false, gfx, pTransforms, pClientRect, pMouseCoordinates, piece.GetType().c_str(), D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), MainMenuSize, 2.0f);
			CategoryMenu->CreateSubsection(piece.GetType().c_str(), D2D1::RectF(pClientRect->right, pClientRect->top + 3.0f, pClientRect->right + GetSize().width, pClientRect->top + MainMenuSize.height + 3.0f));
			CategoryMenu->SetTranslation(D2D1::SizeF(-GetSize().width, pLayersMenu[GetSelectedRoomNumber()]->GetTranslatedRectNotInv().bottom + 2.0f));
			CategoryMenu->SetBorderStyle(BorderStyle::Solid);
			CategoryMenu->vSubsections.back()->SetTranslation(D2D1::SizeF(0.0f, CategoryMenu->GetSize().height + 5.0f));
			CategoryMenu->vSubsections.back()->UpdateInvTranforms(CategoryMenu->GetTransforms() * CategoryMenu->vSubsections.back()->GetTransforms());
			CategoryMenu->vSubsections.back()->SetHidden();
			CategoryMenu->vSubsections.back()->pParent = this;
		}
	}
}

void SideMenu::BuildSubcategories(std::vector<PiecesW>* const wPieces)
{
	if (!CategoryMenu)
		BuildCategories(wPieces);
	for (auto& piece : *wPieces)
	{
		MenuSection* targetSubmenu = CategoryMenu->FindSubmenuSection(piece.GetType().c_str());
		if (!targetSubmenu)
		{
			CategoryMenu->CreateSubsection(piece.GetType().c_str(), D2D1::RectF(pClientRect->right, pClientRect->top + 3.0f, pClientRect->right + GetSize().width, pClientRect->top + MainMenuSize.height + 3.0f));
			targetSubmenu = CategoryMenu->vSubsections.back();
			targetSubmenu->pParent = this;
		}
		MenuSection* targetSubSubmenu = targetSubmenu->FindSubmenuSection(piece.GetSubMenu().c_str());
		if (!targetSubSubmenu)
		{
			targetSubmenu->AddChild(new SubsectionButtons(gfx, pTransforms, pClientRect, pMouseCoordinates, piece.GetSubMenu().c_str(), D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubmenu), true), SubMenuSize, 2.0f);
			targetSubmenu->CreateSubsection(piece.GetSubMenu().c_str(), D2D1::RectF(pClientRect->right, pClientRect->top + 3.0f, pClientRect->right + GetSize().width, pClientRect->top + MainMenuSize.height + 3.0f), true, ItemMenuSize.height);
			targetSubmenu->SetBorderStyle(BorderStyle::Dotted);
			targetSubSubmenu = targetSubmenu->vSubsections.back();
		}
		targetSubmenu->SetTranslation(D2D1::SizeF(0.0f, CategoryMenu->GetSize().height + 5.0f));
		targetSubmenu->UpdateInvTranforms(CategoryMenu->GetTransforms() * CategoryMenu->vSubsections.back()->GetTransforms());
		targetSubSubmenu->AddChild(new SpriteItemButtons(&vInitativeList, gfx, pTransforms, pClientRect, pMouseCoordinates, piece.GetName().c_str(), D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), &piece, ppSelectedSprite, true), ItemMenuSize);
		targetSubSubmenu->SetTranslation(D2D1::SizeF(0.0f, targetSubmenu->GetSize().height + 5.0f));
		targetSubSubmenu->UpdateInvTranforms(CategoryMenu->GetTransforms() * targetSubmenu->GetTransforms() * targetSubmenu->vSubsections.back()->GetTransforms());
		targetSubSubmenu->SetHidden();
		targetSubSubmenu->pParent = targetSubmenu;
		targetSubSubmenu->SetDestBottom(targetSubSubmenu->pChild.back()->GetRect().bottom + targetSubSubmenu->pChild.back()->GetSize().height);
		static_cast<SpriteItemButtons*>(targetSubSubmenu->pChild.back())->SetMatrixPointer(&targetSubSubmenu->AllTransforms);
	}
	for (auto& piece : *wPieces)
	{
		MenuSection* targetSubmenu = CategoryMenu->FindSubmenuSection(piece.GetType().c_str());
		MenuSection* targetSubSubmenu = targetSubmenu->FindSubmenuSection(piece.GetSubMenu().c_str());
		targetSubSubmenu->SetTranslation(D2D1::SizeF(0.0f, targetSubmenu->GetSize().height + 5.0f));
		targetSubSubmenu->UpdateInvTranforms(CategoryMenu->GetTransforms() * targetSubmenu->GetTransforms() * targetSubmenu->vSubsections.back()->GetTransforms());
	}
}

void SideMenu::BuildWallMenu(std::vector<PiecesW>* const wPieces)
{
	CategoryMenu->AddChild(new TypeButtons(&bWallMenuSelected, true, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Walls", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), MainMenuSize, 2.0f);
	CategoryMenu->CreateSubsection(L"Walls", D2D1::RectF(pClientRect->right, pClientRect->top + 3.0f, pClientRect->right + GetSize().width, pClientRect->top + MainMenuSize.height + 3.0f));
	CategoryMenu->vSubsections.back()->SetTranslation(D2D1::SizeF(0.0f, CategoryMenu->GetSize().height + 5.0f));
	CategoryMenu->vSubsections.back()->UpdateInvTranforms(CategoryMenu->GetTransforms() * CategoryMenu->vSubsections.back()->GetTransforms());
	CategoryMenu->vSubsections.back()->pParent = this;
	MenuSection* targetSubmenu = CategoryMenu->vSubsections.back();
	targetSubmenu->SetHidden();

	//Wall Type options
	targetSubmenu->CreateSubsection(L"Solid Color", D2D1::RectF(pClientRect->right, pClientRect->top + 3.0f, pClientRect->right + GetSize().width, pClientRect->top + MainMenuSize.height + 3.0f), true, ItemMenuSize.height);
	targetSubmenu->AddChild(new WallSubsectionButtons(&bUseTexture, false, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Solid Color", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubmenu), true), SubMenuSize, 2.0f);
	targetSubmenu->SetBorderStyle(BorderStyle::Dotted);
	MenuSection* targetSubSubmenu = targetSubmenu->vSubsections.back();

	targetSubSubmenu->AddChild(new ColorButtons(D2D1::ColorF(0.0f, 0.0f, 0.0f), ppSelectedSprite, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Black", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), true), ItemMenuSize);
	targetSubSubmenu->AddChild(new ColorButtons(D2D1::ColorF(1.0f, 1.0f, 1.0f), ppSelectedSprite, gfx, pTransforms, pClientRect, pMouseCoordinates, L"White", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), true), ItemMenuSize);
	targetSubSubmenu->AddChild(new ColorButtons(D2D1::ColorF(0.6f, 0.6f, 0.6f), ppSelectedSprite, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Gray", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), true), ItemMenuSize);
	targetSubSubmenu->AddChild(new ColorButtons(D2D1::ColorF(0.58f, 0.0f, 0.83f), ppSelectedSprite, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Purple", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), true), ItemMenuSize);
	targetSubSubmenu->AddChild(new ColorButtons(D2D1::ColorF(1.0f, 0.0f, 0.0f), ppSelectedSprite, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Reg", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), true), ItemMenuSize);
	targetSubSubmenu->AddChild(new ColorButtons(D2D1::ColorF(0.0f, 0.0f, 1.0f), ppSelectedSprite, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Blue", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), true), ItemMenuSize);
	targetSubSubmenu->AddChild(new ColorButtons(D2D1::ColorF(0.0f, 1.0f, 0.0f), ppSelectedSprite, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Green", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), true), ItemMenuSize);
	targetSubSubmenu->SetTranslation(D2D1::SizeF(0.0f, targetSubmenu->GetSize().height + 5.0f));
	targetSubSubmenu->UpdateInvTranforms(CategoryMenu->GetTransforms() * targetSubmenu->GetTransforms() * targetSubmenu->vSubsections.back()->GetTransforms());
	targetSubSubmenu->SetHidden();
	targetSubSubmenu->pParent = targetSubmenu;
	targetSubSubmenu->SetDestBottom(targetSubSubmenu->pChild.back()->GetRect().bottom + targetSubSubmenu->pChild.back()->GetSize().height);
	static_cast<ColorButtons*>(targetSubSubmenu->pChild.back())->SetMatrixPointer(&targetSubSubmenu->AllTransforms);

	targetSubmenu->CreateSubsection(L"Texture", D2D1::RectF(pClientRect->right, pClientRect->top + 3.0f, pClientRect->right + GetSize().width, pClientRect->top + MainMenuSize.height + 3.0f), true, ItemMenuSize.height);
	targetSubmenu->AddChild(new WallSubsectionButtons(&bUseTexture, true, gfx, pTransforms, pClientRect, pMouseCoordinates, L"Texture", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubmenu), true), SubMenuSize, 2.0f);
	targetSubSubmenu = targetSubmenu->vSubsections.back();

	for (auto& piece : *wPieces)
	{
		if (!_wcsicmp(piece.GetType().c_str(), L"Texture"))
		{
			targetSubSubmenu->AddChild(new SpriteItemButtons(&vInitativeList, gfx, pTransforms, pClientRect, pMouseCoordinates, piece.GetName().c_str(), D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(targetSubSubmenu), &piece, ppSelectedSprite, true), ItemMenuSize);
			targetSubSubmenu->SetTranslation(D2D1::SizeF(0.0f, targetSubmenu->GetSize().height + 5.0f));
			targetSubSubmenu->UpdateInvTranforms(CategoryMenu->GetTransforms() * targetSubmenu->GetTransforms() * targetSubmenu->vSubsections.back()->GetTransforms());
			targetSubSubmenu->SetHidden();
			targetSubSubmenu->pParent = targetSubmenu;
			targetSubSubmenu->SetDestBottom(targetSubSubmenu->pChild.back()->GetRect().bottom + targetSubSubmenu->pChild.back()->GetSize().height);
			static_cast<SpriteItemButtons*>(targetSubSubmenu->pChild.back())->SetMatrixPointer(&targetSubSubmenu->AllTransforms);
		}		
	}
}

const D2D1_COLOR_F SideMenu::GetSelectedWallColor()
{
	for (auto& cat : CategoryMenu->vSubsections)
	{
		if (_wcsicmp(cat->GetLabel(), L"Walls"))
			continue;
		for (auto& child : cat->vSubsections)
		{
			for (auto& c : child->pChild)
			{
				if (c->IsSelected())
					return static_cast<ColorButtons*>(c)->GetDrawColor();
			}
		}
	}
	return D2D1::ColorF(0.0f, 0.0f, 0.0f);
}

void SideMenu::RealignCategories()
{
	if (!CategoryMenu) return;
	
	for (auto& category : CategoryMenu->vSubsections)
	{
		category->UpdateInvTranforms(CategoryMenu->GetTransforms() * category->GetTransforms());
		if (category->vSubsections.size())
			RealignSubcategory(category, CategoryMenu->GetTransforms() * category->GetTransforms());
	}
}
void SideMenu::RealignSubcategory(MenuSection* const subcategory, const D2D1::Matrix3x2F parentTransforms)
{
	if (!subcategory) return;
	for (auto& category : subcategory->vSubsections)
	{
		category->UpdateInvTranforms(parentTransforms * category->GetTransforms());
		if (category->vSubsections.size())
			RealignSubcategory(category, parentTransforms * category->GetTransforms());
	}
}

const MeasurementMenu::SizeMenuType SideMenu::GetSizeMenuType()
{
	for (auto& category : CategoryMenu->pChild)
	{
		if (category->IsSelected())
		{
			if (!_wcsicmp(category->GetLabel(), L"Walls"))
			{
				return MeasurementMenu::SizeMenuType::ThicknessSize;
			}
			break;
		}
	}
	return MeasurementMenu::SizeMenuType::CreatureSize;
}

void SideMenu::BuildInitativeList()
{
	SafeDelete(&InitiativeList);
	D2D1_RECT_F LastRect = pOptionsMenu->GetTranslatedRect();
	InitiativeList = new MenuSection(gfx, pTransforms, pClientRect, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + (m_Dest.right - m_Dest.left), InitativeMenuSize.height + 3.0f), InitativeMenuSize.height, L"Initative", true);
	InitiativeList->SetBorderStyle(BorderStyle::Invisible);
	InitiativeList->pParent = this;
	InitiativeList->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, LastRect.bottom + SeperationDistance));
	for (auto& piece : vInitativeList)
	{
		//D2D1_RECT_F lastrect = InitiativeModeObjects
		InitiativeList->AddChild(new InitiativeListButtons(nullptr, gfx, pTransforms, pClientRect, pMouseCoordinates, piece->GetName().c_str(), D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(InitiativeList), piece, nullptr, true, false, D2D1::ColorF(1.0f, 0.0f, 0.0f, 0.6f), DWRITE_TEXT_ALIGNMENT_LEADING), InitativeMenuSize);
		static_cast<InitiativeListButtons*>(InitiativeList->pChild.back())->SetMatrixPointer(&InitiativeList->AllTransforms);
	}
}