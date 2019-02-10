#include "SideMenu.h"

void SideMenu::Draw()
{
	if (bBuildMode) DrawBuildMode();
	if (!bBuildMode) DrawInitiativeMode();

	for (auto& c : pMenuSections)
	{
		c->Draw();
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

void SideMenu::Interact()
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
			else child->Interact();
		}
	}
	for (size_t i = 0; i < pMenuSections.size(); i++)
	{
		if (pMenuSections.at(i)->PointInRect())
			pMenuSections.at(i)->Interact();
	}
}

void SideMenu::Interact(const D2D1_POINT_2F p)
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

SideMenu::SideMenu(const D2D1_RECT_F targetDest, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p, std::vector< std::vector<SpritePointer*>>** const ppRoom, std::vector<SpritePointer*>** const ppLayer, std::vector< std::vector< std::vector<SpritePointer*>>>** const ppRL, std::vector<bool>* const VisibleRooms, std::vector< std::vector<bool>>* const VisibleLayers) : pSelectedRoom(ppRoom), pSelectedLayer(ppLayer), vSelectRoomsandLayers(ppRL), Buttons(graphics, Transform, area, p)
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

	/*pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, m_Dest, 0.0f, L"workarea", false));
	pMenuSections.back()->pSubsections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, m_Dest, 0.0f, L"Options", false));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->UpdateChildPositions();*/
	pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f,m_Dest.right + (m_Dest.right - m_Dest.left), OptionMenuSize.height + 3.0f), 0.0f, L"Options", false));
	pOptionsMenu = pMenuSections.back();
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Grid on Top", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle PC Colors", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Add Custom Colors", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle Initiative", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle Keep Aspect", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Turn Counter", D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true), OptionMenuSize);
	//pMenuSections.back()->UpdateChildPositions();
	pChild.push_back(new ClassShapes(ShapeTypes::Lines, gfx, false, Transform, pClientRect, pMouseCoordinates));
	pChild.back()->SetDest(D2D1::RectF(m_Dest.left, pMenuSections.back()->pChild.back()->GetRect().bottom + 2.0f, m_Dest.right, pMenuSections.back()->pChild.back()->GetRect().bottom + 6.0f));
	static_cast<ClassShapes*>(pChild.back())->AddPoint(D2D1::Point2F(pChild.back()->GetRect().left, (pChild.back()->GetRect().bottom + pChild.back()->GetRect().top) * 0.5f));
	static_cast<ClassShapes*>(pChild.back())->AddPoint(D2D1::Point2F(pChild.back()->GetRect().right, (pChild.back()->GetRect().bottom + pChild.back()->GetRect().top) * 0.5f));
	pChild.back()->SetFill();
	BuildModeObjects.push_back(pChild.back());
	InitiativeModeObjects.push_back(pChild.back());
	D2D1_RECT_F LastRect = pMenuSections.back()->GetTranslatedRect();
	pMenuSections.back()->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, 0.0f));
	
	pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + (m_Dest.right - m_Dest.left), RoomMenuSize.height + 3.0f), 0.0f, L"Room"));
	pMenuSections.back()->pParent = this;
	pMenuSections.back()->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, LastRect.bottom + SeperationDistance));
	pRoomsMenu = pMenuSections.back();
	pRoomsMenu->SetSelectedRoomPointer(pSelectedRoom);
	pRoomsMenu->SetSelectedLayerPointer(pSelectedLayer);
	pRoomsMenu->vSelectRoomsandLayers = vSelectRoomsandLayers;

	//pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + RoomMenuSize.width, RoomMenuSize.height + 3.0f), 0.0f, L"AddRemoveRoom"));
	pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + RoomMenuSize.width, m_Dest.top + 3.0f + RoomMenuSize.height), 0.0f, L"AddRemoveRoom"));
	pMenuSections.back()->pParent = this;
	pMenuSections.back()->SetTranslation(D2D1::SizeF(-RoomMenuSize.width, 0.0f));
	pAddRemoveRooms = pMenuSections.back();
	pAddRemoveRooms->SetSelectedLayerPointer(pSelectedLayer);
	pAddRemoveRooms->SetSelectedRoomPointer(pSelectedRoom);
	pAddRemoveRooms->vSelectRoomsandLayers = vSelectRoomsandLayers;
	pAddRemoveRooms->AddChild(new AddItem(true, gfx, Transform, area, pMouseCoordinates, L"+", D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + RoomMenuSize.width, m_Dest.top + RoomMenuSize.height + 3.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), this), RoomMenuSize, RoomCheckBoxMenuSize.height + 2.0f);
	/*for (int i = 0; i < 30; i++)
	{
		pRoomsMenu->AddChild(new RoomLayerBox(gfx, Transform, area, pMouseCoordinates, std::to_wstring(i).c_str(), D2D1::RectF(), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(pMenuSections.back()), true, true, D2D1::ColorF(1.0f, 0.0f, 0.0f), i, 0, pVisibleRooms), RoomMenuSize, RoomCheckBoxMenuSize.height + 2.0f);
		D2D1_RECT_F rect = pRoomsMenu->pChild.back()->GetRect();
		float w = (rect.right - rect.left) * 0.25f;
		rect.left += w;
		rect.right -= w;
		rect.top = rect.bottom + 1.0f;
		rect.bottom = rect.top + RoomCheckBoxMenuSize.height;
		
		pRoomsMenu->pChild.back()->pChild.push_back(new Checkbox(gfx, Transform, area, pMouseCoordinates, L" ", rect, D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(pRoomsMenu->pChild.back()), true, i ? false : true));
		pRoomsMenu->pChild.back()->pChild.back()->SetInvertTransformPointer(pRoomsMenu->pChild.back()->GetInvTransforms());
		pRoomsMenu->ResizeDest();
	}*/
	/*for (int i = 0; i < 30; i++)
	{
		CreateRoomButton(Transform, area);
	}*/
	
	//D2D1_RECT_F LastRect = pMenuSections.back()->GetRect();	

	//pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(LastRect.left, LastRect.bottom, LastRect.right, LastRect.bottom + 96), 0.0f, L"Main", false));
	//pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Main", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//pMenuSections.back()->pSubsections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::Rect(LastRect.left, LastRect.bottom + 10.0f, LastRect.right, LastRect.bottom + 106.0f), 0.0f, L"Main", false));
	//pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"SKT", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//pMenuSections.back()->pSubsections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::Rect(LastRect.left, LastRect.bottom + 10.0f, LastRect.right, LastRect.bottom + 106.0f), 0.0f, L"SKT", false));
	//pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Other", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//pMenuSections.back()->pSubsections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::Rect(LastRect.left, LastRect.bottom + 10.0f, LastRect.right, LastRect.bottom + 106.0f), 0.0f, L"Other", false));
	//pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Pirate", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//pMenuSections.back()->pSubsections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::Rect(LastRect.left, LastRect.bottom + 10.0f, LastRect.right, LastRect.bottom + 106.0f), 0.0f, L"Pirate", false));
	//pMenuSections.back()->UpdateChildPositions();
	//pMenuSections.back()->pSubsections.back()->UpdateChildPositions();

	//LastRect = pMenuSections.back()->pSubsections.back()->GetRect();

	/*pMenuSections.back()->pSubsections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::Rect(LastRect.left, LastRect.bottom + 10.0f, LastRect.right, LastRect.bottom + 106.0f), 0.0f, L"Main", false));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Main", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"SKT", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Other", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Pirate", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->UpdateChildPositions();

	LastRect = pMenuSections.back()->pSubsections.back()->GetRect();*/

	//for (auto& section : pMenuSections)
	//{
	//	for (auto& child : section->pChild)
	//	{
	//		for (auto& subsecion : section->pSubsections)
	//		{
	//			if (!_wcsicmp(child->GetLabel(), subsecion->GetLabel()))
	//			{
	//				std::wstring t = subsecion->GetLabel();
	//				t.append(L"Pcs");
	//				subsecion->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, t.c_str(), GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//				subsecion->pSubsections.push_back(new MenuSection(gfx, Transform, pClientRect, pMouseCoordinates, D2D1::RectF(LastRect.left, LastRect.bottom, LastRect.right, LastRect.bottom + 96), 0.0f, t.c_str(), false));
	//				subsecion->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, t.c_str(), GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//				subsecion->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Camel", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//				subsecion->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Shoe", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//				subsecion->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Bob", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//				subsecion->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Canada", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//				subsecion->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, t.c_str(), GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	//				subsecion->pSubsections.back()->UpdateChildPositions();					
	//			}
	//			subsecion->UpdateChildPositions();
	//		}			
	//	}
	//}
	/*pMenuSections.back()->vChildObjects.push_back(std::vector<InteractObjects*>());
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test", D2D1::RectF(), D2D1::ColorF(0,0,0), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->vChildObjects.back().push_back(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Test"));
	pMenuSections.back()->UpdateChildPositions(pMenuSections.back()->vChildObjects.back());*/
	
	//add buttons
	/*D2D1_RECT_F t = CategoryStartPoints[static_cast<int>(MenuItemType::Options)];
	pMenuSections.back()->pSubsections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(t.left, t.top, m_Dest.right, t.bottom), OptionMenuSize.height, L"Options"));
	AddInitiativeObject(pMenuSections.back()->pSubsections.back());
	AddBuildObject(pMenuSections.back()->pSubsections.back());
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f,0.0f,0.0f), static_cast<InteractObjects*>(this), true)); UpdateNextButtonRect(MenuItemType::Options);
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Grid on Top", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true)); UpdateNextButtonRect(MenuItemType::Options);
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle PC Colors", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true)); UpdateNextButtonRect(MenuItemType::Options);
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Add Custom Colors", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this))); UpdateNextButtonRect(MenuItemType::Options);
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle Initiative", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true)); UpdateNextButtonRect(MenuItemType::Options);
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle Keep Aspect", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true)); UpdateNextButtonRect(MenuItemType::Options);
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Turn Counter", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true)); UpdateNextButtonRect(MenuItemType::Options);
	AddSeparation(MenuItemType::Options, Transform, area);
	AddInitiativeObject(pChild.back());
	AddBuildObject(pChild.back());
	
	t = CategoryStartPoints[static_cast<int>(MenuItemType::MainCategory)];
	pChild.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(t.left, t.top, m_Dest.right, t.bottom), MainMenuSize.height, L"Main Categories"));
	AddBuildObject(pChild.back());
	pChild.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"PC", GetNextButtonRect(MenuItemType::MainCategory))); UpdateNextButtonRect(MenuItemType::MainCategory);
	AddSeparation(MenuItemType::MainCategory, Transform, area);
	AddBuildObject(pChild.back());
	
	t = CategoryStartPoints[static_cast<int>(MenuItemType::SubCategory)];
	pChild.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(t.left, t.top, m_Dest.right, t.bottom), SubMenuSize.height, L"Sub Categories"));
	AddBuildObject(pChild.back());
	pChild.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Main", GetNextButtonRect(MenuItemType::SubCategory))); UpdateNextButtonRect(MenuItemType::SubCategory);
	AddSeparation(MenuItemType::SubCategory, Transform, area);
	AddBuildObject(pChild.back());

	t = CategoryStartPoints[static_cast<int>(MenuItemType::SizeCategory)];
	pChild.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(t.left, t.top, m_Dest.right, t.bottom), SizeMenuSize.height, L"Size Categories"));
	AddBuildObject(pChild.back());
	pChild.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Fine", GetNextButtonRect(MenuItemType::SizeCategory), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true)); UpdateNextButtonRect(MenuItemType::SizeCategory);
	AddSeparation(MenuItemType::SizeCategory, Transform, area);
	AddBuildObject(pChild.back());

	t = CategoryStartPoints[static_cast<int>(MenuItemType::ItemCategory)];
	pChild.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(t.left, t.top, m_Dest.right, t.bottom), ItemMenuSize.height + 2.0f, L"Items", true));
	AddBuildObject(pChild.back());
	for (size_t i = 0; i < 100; i++)
	{
		pChild.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Sara", GetNextButtonRect(MenuItemType::ItemCategory), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true)); UpdateNextButtonRect(MenuItemType::ItemCategory);
	}
	
	D2D1_RECT_F const temRect = pChild.back()->GetRect();
	pChild.back()->SetDest(D2D1::RectF(temRect.left, temRect.top, temRect.right, area->bottom > temRect.bottom ? area->bottom : temRect.bottom));*/
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

	pRoomsMenu->ResizeDest();
	pAddRemoveRooms->SetTranslation(D2D1::SizeF(pRoomsMenu->Getdx() + (pRoomsMenu->pChild.back()->GetRect().right - pRoomsMenu->GetRect().left), pRoomsMenu->Getdy() + pRoomsMenu->pChild.back()->GetRect().top));
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
}

void SideMenu::CreateLayerMenuSection()
{
	pMenuSections.push_back(new MenuSection(gfx, pTransforms, pClientRect, pMouseCoordinates, D2D1::RectF(m_Dest.right, m_Dest.top + 3.0f, m_Dest.right + (m_Dest.right - m_Dest.left), m_Dest.top + LayerMenuSize.height + 3.0f), 0.0f, std::to_wstring(pRoomsMenu->pChild.size()).c_str()));
	D2D1_RECT_F rr = pRoomsMenu->GetRect();
	float dist = -pRoomsMenu->GetTranslatedRect().top;
	for (auto& player : pLayersMenu)
		player->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, dist + SeperationDistance + (rr.bottom - rr.top)));
	pMenuSections.back()->SetTranslation(D2D1::SizeF(m_Dest.left - m_Dest.right, dist + SeperationDistance + (rr.bottom - rr.top)));
	pLayersMenu.push_back(pMenuSections.back());
	pLayersMenu.back()->SetSelectedRoomPointer(pSelectedRoom);
	pLayersMenu.back()->SetSelectedLayerPointer(pSelectedLayer);
	pLayersMenu.back()->vSelectRoomsandLayers = vSelectRoomsandLayers;
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