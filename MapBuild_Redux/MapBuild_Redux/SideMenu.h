#pragma once
#include "MenuSection.h"
#include "Buttons.h"
#include "Shapes.h"
#include "TurnCounter.h"

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
		RoomCategory,
		RoomCategoryCheckBox,
		LayerCategory,
		LayerCategoryCheckBox,
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
	const D2D1_SIZE_F InitativeMenuSize = D2D1::SizeF(477.0f, 125.0f);
	const D2D1_SIZE_F MainMenuSize = D2D1::SizeF(94.0f, 32.0f);
	const D2D1_SIZE_F SubMenuSize = D2D1::SizeF(94.0f, 32.0f);
	const D2D1_SIZE_F SizeMenuSize = D2D1::SizeF(94.0f, 32.0f);
	const D2D1_SIZE_F ItemMenuSize = D2D1::SizeF(94.0f, 125.0f);
	const D2D1_SIZE_F RoomMenuSize = D2D1::SizeF(18.0f, 18.0f);
	const D2D1_SIZE_F RoomCheckBoxMenuSize = D2D1::SizeF(9.0f, 9.0f);
	const D2D1_SIZE_F LayerMenuSize = D2D1::SizeF(18.0f, 18.0f);
	const D2D1_SIZE_F LayerCheckBoxMenuSize = D2D1::SizeF(9.0f, 9.0f);
	D2D1_RECT_F mRealRect = D2D1::RectF();
	const float SeperationDistance = 5.0f;
	bool bBuildMode = true;
	bool bAttachObject = false;
	char* Buffer = nullptr;
	size_t BufferSize = 0;
public:
	MenuSection* pAddRemoveRooms = nullptr;
	MenuSection* pAddRemoveLayers = nullptr;
	MenuSection* pOptionsMenu = nullptr;
	MenuSection* pRoomsMenu = nullptr;
	MenuSection* pRoomsCheckMenu = nullptr;
	std::vector<MenuSection*> pLayersMenu;
	MenuSection* pLayersCheckMenu = nullptr;
	MenuSection* CategoryMenu = nullptr;
	MenuSection* SubcategoryMenu = nullptr;
	MenuSection* pSizeMenu = nullptr;
	MenuSection* ItemMenu = nullptr;
	MenuSection* InitiativeList = nullptr;
	PiecesW* pFirstPieceW = nullptr;
	HWND hWnd;
private:
	std::vector<D2D1_RECT_F> CategoryStartPoints;	
	std::vector<InteractObjects*> InitiativeModeObjects;
	std::vector<InteractObjects*> BuildModeObjects;
	std::vector<MenuSection*> pMenuSections;
	const D2D1_RECT_F GetNextButtonRect(const MenuItemType ItemType);
	void UpdateNextButtonRect(const MenuItemType ItemType);
	void AddSeparation(const MenuItemType ItemType, D2D1::Matrix3x2F* const transform, D2D1_RECT_F* const area, const D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f));
	void DrawBuildMode();
	void DrawInitiativeMode();
	void AddInitiativeObject(InteractObjects* const Io);
	void AddBuildObject(InteractObjects* const Io);
	bool bWallMenuSelected = false;
	bool bAoeMenuSelected = false;
	bool bUseTexture = false;
	bool bShowPieceColors = false;
	bool bNew = false;
	bool* const pExit;	
public:
	SideMenu(bool* const showcounter, TurnCounter** const TurnCounter, const HWND hwnd, bool* const Exit, bool* const pLockToGrid, bool* const pGridOnTop, bool* const pKeepAspect, const D2D1_RECT_F targetDest, Graphics* const graphics, D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, D2D1_POINT_2F* const p,
		std::vector< std::vector<SpritePointer*>>** const ppRoom, std::vector<SpritePointer*>** const ppLayer, std::vector< std::vector< std::vector<SpritePointer*>>>** const ppRL,
		std::vector<bool>* const VisibleRooms, std::vector< std::vector<bool>>* const VisibleLayers, SpritePointer** const ppsprite,
		std::vector< std::vector< std::vector<std::unique_ptr<Wall>>>>** const ppW, std::vector< std::vector<std::unique_ptr<Wall>>>** const ppSWR, std::vector<std::unique_ptr<Wall>>** const ppSWL);
	~SideMenu()
	{
		SafeDeleteArray(&Buffer);
		while (!pMenuSections.empty())
		{
			SafeDelete(&pMenuSections.back());
			pMenuSections.pop_back();
		}

		while (!InitiativeModeObjects.empty())
		{
			InitiativeModeObjects.back() = nullptr;
			InitiativeModeObjects.pop_back();
		}

		while (!BuildModeObjects.empty())
		{
			BuildModeObjects.back() = nullptr;
			BuildModeObjects.pop_back();
		}
		
		while (pChild.size())
		{
			SafeDelete(&pChild.back());
			pChild.pop_back();
		}
	}
	SideMenu(const SideMenu&) = delete;
	SideMenu& operator=(const SideMenu&) = delete;
	void Draw() override;
	const uint32_t GetTopOfTheRoundPosition();
	const bool Interact(const D2D1_POINT_2F p) override;
	const bool Interact() override;
	const bool AlternateInteract() override;
	const bool AlternateInteract(const D2D1_POINT_2F p) override;
	const AoeSpritePointer::AoeTypes GetSelectedAoeType() { return SelectedAoeType; }
	bool PointInRect(const D2D1_POINT_2F p) override;
	void WheelUp() override;
	void WheelDown() override;
	void Unload() override;
	void ChangeMode() override { bBuildMode ^= true; }
	void ToggleColors() { bShowPieceColors ^= true; }
	void SetUseColors() { bShowPieceColors = true; }
	void UnsetUseColors() { bShowPieceColors = false; }
	void SetInvTransform(D2D1::Matrix3x2F* const inv) { pInvTransforms = inv; }
	const bool IsInRealRect() { return PointInTargetRect(mRealRect); }
	const bool IsBuildMode() { return bBuildMode; }
	void SetIsBuildMode() { bBuildMode = true; }
	void UnsetIsBuildMode() { bBuildMode = false; }
	void SetAttachObject() { bAttachObject = true; }
	void UnsetAttachObject() { bAttachObject = false; }
	void SetSelectedRoomPointer(std::vector< std::vector<SpritePointer*>>** const p) { pSelectedRoom = p; }
	void SetSelectedLayerPointer(std::vector<SpritePointer*>** const p) { pSelectedLayer = p; }
	void SetSelectedWallRoomPointer(std::vector< std::vector<std::unique_ptr<Wall>>>** const p) { ppSelectedWallRoom = p; for (auto& sections : pMenuSections) sections->SetSelectedWallRoomPointer(p); }
	void SetSelectedWallLayerPointer(std::vector<std::unique_ptr<Wall>>** const p) { ppSelectedWallLayer = p; for (auto& sections : pMenuSections) sections->SetSelectedWallLayerPointer(p); }
	void SetWallRoomandLayerPointer(std::vector < std::vector< std::vector<std::unique_ptr<Wall>>>>** const p) { pSelectWallRoomsandLayers = p; for (auto& sections : pMenuSections) sections->pSelectWallRoomsandLayers = p; }
	void CreateRoomButton(D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area);
	void CreateLayerButton(D2D1::Matrix3x2F* const Transform, D2D1_RECT_F* const area, size_t uRoomNumber);
	void CreateLayerMenuSection();
	void CreateLayer(size_t uRoomNumber);
	void RealignAddLayerButton();
	void RealignAddLayerButton(uint32_t uRoomNumber);
	const uint32_t GetSelectedRoomNumber();
	const uint32_t GetSelectedLayerNumber(const uint32_t uRoom);
	const uint32_t GetSelectedLayerNumber();
	void SetRoom(size_t uRoomNumber) override;
	void SetLayer(size_t uLayerNumber) override;
	void BuildCategories(std::vector<PiecesW>* const wPieces);
	void BuildSubcategories(std::vector<PiecesW>* const wPieces);
	void BuildWallMenu(std::vector<PiecesW>* const wPieces);
	void FillWithColorButtons(MenuSection* const targetSubSubmenu);
	void BuildAoeMenu(std::vector<PiecesW>* const wPieces);
	void RealignCategories();
	void RealignSubcategory(MenuSection* const subcategory, const D2D1::Matrix3x2F parentTranforms);
	const D2D1_COLOR_F GetSelectedWallColor();
	const D2D1_COLOR_F GetSelectedAoeColor();
	const bool StringIsShapeType(const std::wstring name, const AoeSpritePointer::AoeTypes type);
	const AoeSpritePointer::AoeTypes StringToShapeType(const std::wstring name);
	const MeasurementMenu::SizeMenuType GetSizeMenuType();
	const bool WallSelected() { return bWallMenuSelected; }
	const bool AoeSelected() { return bAoeMenuSelected; }
	const bool UseTexture() { return bUseTexture; }
	const bool ShowPieceColors() { return bShowPieceColors; }
	const bool IsAttachObject() { return bAttachObject; }
	const bool CreateNew() { return bNew; }
	const uint32_t GetInitativeListSize() { return static_cast<uint32_t>(vInitativeList.size()); }
	void BuildInitativeList();
	void NextTurn();
	void PreviousTurn();
	void RemoveFromMenuSection(MenuSection* const p);
	
	std::vector< std::vector<SpritePointer*>>** pSelectedRoom = nullptr;
	std::vector< std::vector<std::unique_ptr<Wall>>>** ppSelectedWallRoom = nullptr;
	std::vector<SpritePointer*>** pSelectedLayer = nullptr;
	std::vector<std::unique_ptr<Wall>>** ppSelectedWallLayer = nullptr;
	std::vector< std::vector< std::vector<SpritePointer*>>>** vSelectRoomsandLayers = nullptr;
	std::vector< std::vector< std::vector<std::unique_ptr<Wall>>>>** pSelectWallRoomsandLayers = nullptr;
	std::vector<bool>* pVisibleRooms = nullptr;
	std::vector< std::vector<bool>>* pVisibleLayers = nullptr;
	MenuSection* pSelectedCategory = nullptr;
	MenuSection* pSelectedSubcategory = nullptr;	
	SpritePointer** ppSelectedSprite = nullptr;
	void* pBaseLevel = nullptr;
	AoeSpritePointer::AoeTypes SelectedAoeType = AoeSpritePointer::AoeTypes::Invalid;
	std::vector<PiecesW*> vInitativeList;
	TurnCounter** ppTurnCounter = nullptr;
	bool* pShowCounter = nullptr;
};