#pragma once
#include "MenuSection.h"
#include "Buttons.h"
#include "Shapes.h"

class SideMenu : public Buttons
{
private:
	enum class MenuItemType
	{
		Options,
		MainCategory,
		SubCategory,
		SizeCategory,
		ItemCategory,
		Error
	};
	void FillRect(D2D1_RECT_F& rect, const D2D1_SIZE_F size)
	{
		rect.right = rect.left + size.width;
		rect.bottom = rect.top + size.height;
	}
	const float GetMenuItemWidth(const MenuItemType type)
	{
		switch (type)
		{
		case MenuItemType::Options:
			return OptionMenuSize.width;
		case MenuItemType::MainCategory:
			return MainMenuSize.width;
		case MenuItemType::SubCategory:
			return SubMenuSize.width;
		case MenuItemType::SizeCategory:
			return SizeMenuSize.width;
		case MenuItemType::ItemCategory:
			return ItemMenuSize.width;
		default:
			return 0.0f;
		}
	}
	const float GetMenuItemHeight(const MenuItemType type)
	{
		switch (type)
		{
		case MenuItemType::Options:
			return OptionMenuSize.height;
		case MenuItemType::MainCategory:
			return MainMenuSize.height;
		case MenuItemType::SubCategory:
			return SubMenuSize.height;
		case MenuItemType::SizeCategory:
			return SizeMenuSize.height;
		case MenuItemType::ItemCategory:
			return ItemMenuSize.height;
		default:
			return -1.0f;
		}
	}
private:
	const D2D1_SIZE_F OptionMenuSize = D2D1::SizeF(94.0f, 32.0f);
	const D2D1_SIZE_F MainMenuSize = D2D1::SizeF(94.0f, 32.0f);
	const D2D1_SIZE_F SubMenuSize = D2D1::SizeF(94.0f, 32.0f);
	const D2D1_SIZE_F SizeMenuSize = D2D1::SizeF(94.0f, 32.0f);
	const D2D1_SIZE_F ItemMenuSize = D2D1::SizeF(94.0f, 54.0f);
	const float SeperationDistance = 10.0f;
private:
	std::vector<D2D1_RECT_F> CategoryStartPoints;
	const D2D1_RECT_F GetNextButtonRect(const MenuItemType ItemType);
	void UpdateNextButtonRect(const MenuItemType ItemType);
	void AddSeparation(const MenuItemType ItemType, D2D1::Matrix3x2F* const transform, D2D1_RECT_F* const area, const D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f));
public:
	SideMenu(const D2D1_RECT_F targetDest, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p);
	~SideMenu()
	{
		while (pChild.size())
		{
			SafeDelete(&pChild.back());
			pChild.pop_back();
		}
	}
	void Draw() override;
	void Interact(const D2D1_POINT_2F p) override;
	void Interact() override;
	bool PointInRect(const D2D1_POINT_2F p) override;
	void WheelUp() override;
	void WheelDown() override;
	void Unload() override;
};