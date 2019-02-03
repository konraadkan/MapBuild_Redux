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
			return;
		}
	}
	for (auto& section : pMenuSections)
	{
		if (section->PointInRect())
			section->Interact();
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

SideMenu::SideMenu(const D2D1_RECT_F targetDest, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p) : Buttons(graphics, Transform, area, p)
{
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

	/*pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, m_Dest, 0.0f, L"workarea", false));
	pMenuSections.back()->pSubsections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, m_Dest, 0.0f, L"Options", false));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->pSubsections.back()->UpdateChildPositions();*/
	pMenuSections.push_back(new MenuSection(gfx, Transform, area, pMouseCoordinates, D2D1::RectF(m_Dest.left, 0,m_Dest.right, 96), 0.0f, L"Options", false));
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Lock to Grid", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Grid on Top", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle PC Colors", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Add Custom Colors", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle Initiative", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Toggle Keep Aspect", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->AddChild(new Buttons(gfx, Transform, area, pMouseCoordinates, L"Turn Counter", GetNextButtonRect(MenuItemType::Options), D2D1::ColorF(0.0f, 0.0f, 0.0f), static_cast<InteractObjects*>(this), true));
	pMenuSections.back()->UpdateChildPositions();
	
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