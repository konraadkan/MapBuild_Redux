#pragma once
#include <d2d1_1.h>
#include <queue>
#include "StringMod.h"
#include "Sprite.h"
#include "HPTimer.h"
#include "CreatureSize.h"

class Pieces : public StringMod
{
private:
	template<typename T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			if (*ppT) delete (*ppT);
			(*ppT) = nullptr;
		}
	}
	template<typename T> void SafeDeleteArray(T** ppT)
	{
		if (ppT)
		{
			if (*ppT) delete[](*ppT);
			(*ppT) = nullptr;
		}
	}
protected:
	CreatureSize mSize = CreatureSize::Medium;
	Sprite* pSprite = nullptr;
	Sprite* pPortrait = nullptr;
	Graphics* gfx = nullptr;
	HPTimer* pTimer;
	std::string sType;
	std::string sName;
	std::string sIconPath;
	std::string sSpritePath;
	std::string sSubMenu;
	bool bDetaultInitOrder = false;
	bool bKeepAspect = false;
	bool bKeepIconAspect = false;
	D2D1_COLOR_F BackgroundColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
public:
	Pieces(Graphics* const graphics, HPTimer* const timer) : gfx(graphics), pTimer(timer) {}
	Pieces(std::string Buffer, Graphics* const graphics, HPTimer* const timer) : pTimer(timer), gfx(graphics) { char* p = nullptr; FillPiece(Buffer, p); }
	std::queue<std::string> FillPiece(std::string& Buffer, char*& pos);
	std::queue<std::wstring> FillPiece(char* const Buffer, char*& pos);
public:
	const std::string GetType() { return sType; }
	const std::string GetName() { return sName; }
	const std::string GetIconPath() { return sIconPath; }
	const std::string GetSpritePath() { return sSpritePath; }
	const std::string GetSubMenu() { return sSubMenu; }
	const bool IsDefault() { return bDetaultInitOrder; }
	const bool KeepAspectSprite() { return bKeepAspect; }
	const bool KeepAspectIcon() { return bKeepIconAspect; }
	const D2D1_COLOR_F GetBackgroundColor() { return BackgroundColor; }
public:
	const CreatureSize StringToSize(const std::wstring wsize);
	const CreatureSize StringToSize(const std::string ssize);
	void SetSize(const std::wstring wsize) { mSize = StringToSize(wsize); }
	void SetSize(const std::string ssize) { mSize = StringToSize(ssize); }
	void SetSize(const CreatureSize size) { mSize = size; }
	void SetType(const std::string type) { sType = type; }
	void SetName(const std::string name) { sName = name; }
	void SetIconPath(const std::string spath) { sIconPath = spath; }
	void SetSpritePath(const std::string spath) { sSpritePath = spath; }
	void SetSubMenu(const std::string spath) { sSubMenu = spath; }
	void SetDefault() { bDetaultInitOrder = true; }
	void UnsetDefault() { bDetaultInitOrder = false; }
	void ToggleDefault() { bDetaultInitOrder ^= true; }
	void SetKeepAspectSprite() { bKeepAspect = true; }
	void UnsetKeepAspectSprite() { bKeepAspect = false; }
	void ToggleAspectSprite() { bKeepAspect ^= true; }
	void SetKeepAspectIcon() { bKeepIconAspect = true; }
	void UnsetKeepAspectIcon() { bKeepIconAspect = false; }
	void ToggleAspectIcon() { bKeepIconAspect ^= true; }
	void SetBackgroundColor(const D2D1_COLOR_F color) { memcpy(&BackgroundColor, &color, sizeof(BackgroundColor)); }
	void SetBackgroundR(const float r) { BackgroundColor.r = r; }
	void SetBackgroundG(const float g) { BackgroundColor.g = g; }
	void SetBackgroundB(const float b) { BackgroundColor.b = b; }
	void SetBackgroundA(const float a) { BackgroundColor.a = a; }
	void LoadSprite();
	void LoadPortrait();
	Sprite* const GetSprite() { return pSprite; }
	Sprite* const GetPortrait() { return pPortrait; }
};

class PiecesW : public StringMod
{
private:
	template<typename T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			if (*ppT) delete (*ppT);
			(*ppT) = nullptr;
		}
	}
	template<typename T> void SafeDeleteArray(T** ppT)
	{
		if (ppT)
		{
			if (*ppT) delete[](*ppT);
			(*ppT) = nullptr;
		}
	}
protected:
	CreatureSize mSize = CreatureSize::Medium;
	Sprite* pSprite = nullptr;
	Sprite* pPortrait = nullptr;
	Graphics* gfx = nullptr;
	HPTimer* pTimer;
	std::wstring sType;
	std::wstring sName;
	std::wstring sIconPath;
	std::wstring sSpritePath;
	std::wstring sSubMenu;
	bool bDetaultInitOrder = false;
	bool bKeepAspect = false;
	bool bKeepIconAspect = false;
	D2D1_COLOR_F BackgroundColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
public:
	PiecesW(Graphics* const graphics, HPTimer* const timer) : gfx(graphics), pTimer(timer) {}
	PiecesW(std::wstring Buffer, Graphics* const graphics, HPTimer* const timer) : gfx(graphics), pTimer(timer) { wchar_t* p = nullptr; FillPiece(Buffer, p); }
	std::queue<std::wstring> FillPiece(std::wstring& Buffer, wchar_t*& pos);
	std::queue<std::wstring> FillPiece(wchar_t* const Buffer, wchar_t*& pos);
public:
	const std::wstring GetType() { return sType; }
	const std::wstring GetName() { return sName; }
	const std::wstring GetIconPath() { return sIconPath; }
	const std::wstring GetSpritePath() { return sSpritePath; }
	const std::wstring GetSubMenu() { return sSubMenu; }
	const bool IsDefault() { return bDetaultInitOrder; }
	const bool KeepAspectSprite() { return bKeepAspect; }
	const bool KeepAspectIcon() { return bKeepIconAspect; }
	const D2D1_COLOR_F GetBackgroundColor() { return BackgroundColor; }
public:
	void Convert(Pieces Piece);
	void SetType(const std::wstring type) { sType = type; }
	void SetName(const std::wstring name) { sName = name; }
	void SetIconPath(const std::wstring spath) { sIconPath = spath; }
	void SetSpritePath(const std::wstring spath) { sSpritePath = spath; }
	void SetSubMenu(const std::wstring spath) { sSubMenu = spath; }
	const CreatureSize StringToSize(const std::wstring wsize);
	const CreatureSize StringToSize(const std::string ssize);
	void SetSize(const std::wstring wsize) { mSize = StringToSize(wsize); }
	void SetSize(const std::string ssize) { mSize = StringToSize(ssize); }
	void SetSize(const CreatureSize size) { mSize = size; }
	const CreatureSize GetSize() { return mSize; }
	void SetDefault() { bDetaultInitOrder = true; }
	void UnsetDefault() { bDetaultInitOrder = false; }
	void ToggleDefault() { bDetaultInitOrder ^= true; }
	void SetKeepAspectSprite() { bKeepAspect = true; }
	void UnsetKeepAspectSprite() { bKeepAspect = false; }
	void ToggleAspectSprite() { bKeepAspect ^= true; }
	void SetKeepAspectIcon() { bKeepIconAspect = true; }
	void UnsetKeepAspectIcon() { bKeepIconAspect = false; }
	void ToggleAspectIcon() { bKeepIconAspect ^= true; }
	void SetBackgroundColor(const D2D1_COLOR_F color) { memcpy(&BackgroundColor, &color, sizeof(BackgroundColor)); }
	void SetBackgroundR(const float r) { BackgroundColor.r = r; }
	void SetBackgroundG(const float g) { BackgroundColor.g = g; }
	void SetBackgroundB(const float b) { BackgroundColor.b = b; }
	void SetBackgroundA(const float a) { BackgroundColor.a = a; }
	void LoadSprite();
	void LoadPortrait();
	Sprite* const GetSprite() { return pSprite; }
	Sprite* const GetPortrait() { return pPortrait; }
};

struct Location
{
	D2D1_RECT_F mDestSprite = D2D1::RectF();
	D2D1_RECT_F mDestPortrait = D2D1::RectF();
	D2D1_RECT_F mResizedDestSprite = D2D1::RectF();
	unsigned long uLayer = 0;
	unsigned long uRoom = 0;
};

class SpritePointer
{
private:
	Location mLocation;
	PiecesW* const pPiece;
	bool bSelected = false;
	float fOpacity = 1.0f;
	CreatureSize mSize = CreatureSize::Medium;
	bool bKeepAspectRatioSprite = true;
	bool bKeepAspectRatioPortrait = true;
public:
	SpritePointer(PiecesW* const p, const Location loc, bool keepaspectsprite = true, bool keepaspectportrait = true) : 
		mLocation(loc), pPiece(p), bKeepAspectRatioSprite(keepaspectsprite), bKeepAspectRatioPortrait(keepaspectportrait) {}
	~SpritePointer() { }
public:
	void MoveDestSprite(const D2D1_SIZE_F changes);
	void MovePortraitSprite(const D2D1_SIZE_F changes);
	void MoveResizedDestSprite(const D2D1_SIZE_F changes);
	void SetDestSpritePosition(const D2D1_POINT_2F point);
	void SetDestResizedSpritePosition(const D2D1_POINT_2F point);
	void SetPortraitPosition(const D2D1_POINT_2F point);
	void SetDestSprite(const D2D1_RECT_F d, bool ApplyRebuild = true);
	void SetDestPortrait(const D2D1_RECT_F d);
	const D2D1_RECT_F RectToAspectSprite(const D2D1_RECT_F d);
	const D2D1_RECT_F RectToAspectPortrait(const D2D1_RECT_F d);
	void SetLayer(const unsigned long layer) { mLocation.uLayer = layer; }
	void SetRoom(const unsigned long room) { mLocation.uRoom = room; }
	void SetSelected() { bSelected = true; }
	void SetOpacity(const float opacity) { fOpacity = opacity; }
	void SetCreatureSize(const CreatureSize csize) { mSize = csize; BuildResizedDestSprite(); }
	const CreatureSize GetCreatureSize() { return mSize; }
	void UnsetSelected() { bSelected = false; }
	void ToggleSelected() { bSelected ^= true; }
	void DrawSprite(Graphics* const gfx, bool back = true);
	void DrawPortrait(Graphics* const gfx, bool back = true);
	void BuildResizedDestSprite();
	void SetKeepAspectSprite() { bKeepAspectRatioSprite = true; }
	void UnsetKeepAspectSprite() { bKeepAspectRatioSprite = false; }
	void ToggleAspectRatioSprite() { bKeepAspectRatioSprite ^= true; }
	void SetKeepAspectPortrait() { bKeepAspectRatioPortrait = true; }
	void UnsetKeepAspectPortrait() { bKeepAspectRatioPortrait = false; }
	void ToggleAspectRatioPortrait() { bKeepAspectRatioPortrait ^= true; }
public:
	const bool IsSelected() { return bSelected; }
	const bool IsKeepAspectSprite() { return bKeepAspectRatioSprite; }
	const bool IsKeepAspectPortrait() { return bKeepAspectRatioPortrait; }
	const bool PointInSprite(D2D1_POINT_2F p) { return (p.x > mLocation.mDestSprite.left && p.x < mLocation.mDestSprite.right && p.y > mLocation.mDestSprite.top && p.y < mLocation.mDestSprite.bottom); }
	const D2D1_RECT_F GetDestSprite() { return mLocation.mDestSprite; }
	const D2D1_RECT_F GetDestResizedSprite() { return mLocation.mResizedDestSprite; }
	const D2D1_RECT_F GetDestPortrait() { return mLocation.mDestPortrait; }
	const D2D1_SIZE_F GetSpriteFrameSize() { return pPiece->GetSprite()->GetFrameSize(); }
	const D2D1_SIZE_F GetPortraitFrameSize() { return pPiece->GetPortrait()->GetFrameSize(); }
	const unsigned long GetLayer() { return mLocation.uLayer; }
	const unsigned long GetRoom() { return mLocation.uRoom; }
	const Location GetLocation() { return mLocation; }
	PiecesW* const GetPieces() { return pPiece; }
	Sprite* const GetSprite() { return pPiece->GetSprite(); }
	Sprite* const GetPortrait() { return pPiece->GetPortrait(); }
};