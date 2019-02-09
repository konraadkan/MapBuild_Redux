#pragma once
#include <d2d1_1.h>
#include <queue>
#include "StringMod.h"
#include "Sprite.h"
#include "HPTimer.h"

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
public:
	SpritePointer(PiecesW* const p, const Location loc) : mLocation(loc), pPiece(p) {}
	~SpritePointer() { }
public:
	void SetDestSprite(const D2D1_RECT_F d) { mLocation.mDestSprite = d; }
	void SetDestPortrait(const D2D1_RECT_F d) { mLocation.mDestPortrait = d; }
	void SetLayer(const unsigned long layer) { mLocation.uLayer = layer; }
	void SetRoom(const unsigned long room) { mLocation.uRoom = room; }
	void SetSelected() { bSelected = true; }
	void SetOpacity(const float opacity) { fOpacity = opacity; }
	void UnsetSelected() { bSelected = false; }
	void ToggleSelected() { bSelected ^= true; }
	void DrawSprite(Graphics* const gfx, bool back = true);
	void DrawPortrait(Graphics* const gfx, bool back = true);
public:
	const bool IsSelected() { return bSelected; }
	const D2D1_RECT_F GetDestSprite() { return mLocation.mDestSprite; }
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