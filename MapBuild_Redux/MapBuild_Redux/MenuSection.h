#pragma once
#include<vector>
#include "InteractObjects.h"
#include "Pieces.h"

class MenuSection : public InteractObjects
{
private:	
	bool bIsScrollable = false;
	D2D1_SIZE_F EntrySize = D2D1::SizeF(94.0f, 32.0f);
	D2D1::Matrix3x2F Transforms = D2D1::Matrix3x2F::Identity();
	D2D1::Matrix3x2F InvTransforms = D2D1::Matrix3x2F::Identity();
	D2D1_POINT_2F TransformedPoint = D2D1::Point2F();
	float ScrollStep = 100.0f;
	BorderStyle Border = BorderStyle::Invisible;
public:
	MenuSection(Graphics* const graphics, D2D1::Matrix3x2F* const transform, D2D1_RECT_F* const clientrect, D2D1_POINT_2F* const p, D2D1_RECT_F dest, float scrollstep, const wchar_t* label, bool scroll = false) : InteractObjects(graphics, transform, clientrect, p)
	{
		bIsScrollable = scroll;
		ScrollStep = scrollstep;
		SetDest(dest);
		SetLabel(label);
	}
	~MenuSection()
	{
		Unload();
	}
public:
	void ShiftTranslation(D2D1_SIZE_F distance);
	const D2D1_RECT_F GetTranslatedRect() override;
	const D2D1_RECT_F GetTranslatedRectNotInv();
	const float Getdx() { return Transforms.dx; }
	const float Getdy() { return Transforms.dy; }
	const float Getinvdx() { return InvTransforms.dx; }
	const float Getinvdy() { return InvTransforms.dy; }
	D2D1::Matrix3x2F* const GetInvTranforms() { return &InvTransforms; }
	const bool Interact() override;
	void Draw() override;
	void AddChild(InteractObjects* const Iobject, const D2D1_SIZE_F size, float seperation = 0.0f) override;
	void Unload() override;
	void WheelUp() override;
	void WheelDown() override;
	bool PointInRect(const D2D1_POINT_2F p) override;
	bool PointInRect() override;
	void CreateSubsection(const wchar_t* label, bool scroll = false, float scrollsize = 0.0f);
	void CreateSubsection(const wchar_t* label, D2D1_RECT_F dest, bool scroll = false, float scrollsize = 0.0f);
	void SetEntrySize(const D2D1_SIZE_F size) { memcpy(&EntrySize, &size, sizeof(EntrySize)); }
	const D2D1_SIZE_F GetEntrySize() { return EntrySize; }
	void UpdateChildPositions(std::vector<InteractObjects*>& childobjs);
	void UpdateChildPositions();
	void SetTranslation(D2D1_SIZE_F size);
	void UpdateInvTranforms(D2D1::Matrix3x2F transfrom);
	void ResizeDest();
	void ResizeDest(const D2D1_SIZE_F r);
	void SetSelectedRoomPointer(std::vector< std::vector<SpritePointer*>>** const p) { pSelectedRoom = p; }
	void SetSelectedLayerPointer(std::vector<SpritePointer*>** const p) { pSelectedLayer = p; }
	void SetRoom(const size_t pos) override;
	void SetLayer(const size_t pos) override;
	void SetBorderStyle(const BorderStyle bs) override { Border = bs; }
public:
	std::vector<MenuSection*> vSubsections;
	std::vector< std::vector<InteractObjects*>> vChildObjects;
	std::vector< std::vector< std::vector<SpritePointer*>>>** vSelectRoomsandLayers = nullptr;
	std::vector< std::vector<SpritePointer*>>** pSelectedRoom = nullptr;
	std::vector<SpritePointer*>** pSelectedLayer = nullptr;
	MenuSection* const FindSubmenuSection(const wchar_t* wCategoryName);
	const D2D1::Matrix3x2F GetTransforms() { return Transforms; }
};