#pragma once
#include <d2d1_1.h>
#include <queue>
#include "StringMod.h"

class Pieces : public StringMod
{
protected:
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
	Pieces() {}
	Pieces(std::string Buffer) { char* p = nullptr; FillPiece(Buffer, p); }
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
};

class PiecesW : public StringMod
{
protected:
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
	PiecesW() {}
	PiecesW(std::wstring Buffer) { wchar_t* p = nullptr; FillPiece(Buffer, p); }
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
};
