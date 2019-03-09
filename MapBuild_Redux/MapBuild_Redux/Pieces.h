#pragma once
#include <d2d1_1.h>
#include <queue>
#include <atomic>
#include "StringMod.h"
#include "Sprite.h"
#include "HPTimer.h"
#include "CreatureSize.h"
#include "SafeReleaseMemory.h"

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
	D2D1_SIZE_F* pGridSize = nullptr;
public:
	Pieces(Graphics* const graphics, HPTimer* const timer, D2D1_SIZE_F* const gridsize) : gfx(graphics), pTimer(timer), pGridSize(gridsize) {}
	Pieces(std::string Buffer, Graphics* const graphics, HPTimer* const timer, D2D1_SIZE_F* const gridsize) : pTimer(timer), gfx(graphics), pGridSize(gridsize) { char* p = nullptr; FillPiece(Buffer, p); }
	~Pieces() { }
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
	const CreatureSize GetSize() { return mSize; }
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
	void LoadSpriteM(std::atomic<uint32_t>& numberfinished);
	void LoadPortrait();
	void LoadPortraitM(std::atomic<uint32_t>& numberfinished);
	void UnloadSprite() { if (pSprite) SafeDelete(&pSprite); }
	void UnloadPortrait() { if (pPortrait) SafeDelete(&pPortrait); }
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
	D2D1_SIZE_F* pGridSize = nullptr;
public:
	PiecesW(Graphics* const graphics, HPTimer* const timer, D2D1_SIZE_F* const gridsize) : gfx(graphics), pTimer(timer), pGridSize(gridsize) {}
	PiecesW(std::wstring Buffer, Graphics* const graphics, HPTimer* const timer, D2D1_SIZE_F* const gridsize) : gfx(graphics), pTimer(timer), pGridSize(gridsize) { wchar_t* p = nullptr; FillPiece(Buffer, p); }
	~PiecesW() { }
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
	void LoadSpriteM(std::atomic<uint32_t>& numberfinished);
	void LoadPortrait();
	void LoadPortraitM(std::atomic<uint32_t>& numberfinished);
	void UnloadSprite() { if (pSprite) SafeDelete(&pSprite); }
	void UnloadPortrait() { if (pPortrait) SafeDelete(&pPortrait); }
	Sprite* const GetSprite() { return pSprite; }
	Sprite* const GetPortrait() { return pPortrait; }
	const uint32_t CalcSaveSize();
	const char* GetSaveBuffer() { return BuildSaveBuffer(); }
	const char* BuildSaveBuffer();
	const bool LoadSaveBuffer(const char* Buffer);
};

struct Location
{
	D2D1_RECT_F mDestSprite = D2D1::RectF();
	D2D1_RECT_F mDestPortrait = D2D1::RectF();
	D2D1_RECT_F mResizedDestSprite = D2D1::RectF();
	D2D1_RECT_F mDestTag = D2D1::RectF();
	unsigned long uLayer = 0;
	unsigned long uRoom = 0;
};

class SpritePointer : public SafeReleaseMemory
{
protected:
	Graphics* gfx = nullptr;
	Location mLocation;
	PiecesW* pPiece = nullptr;
	char* LoadedPiece = nullptr;
	bool bSelected = false;
	float fOpacity = 1.0f;
	CreatureSize mSize = CreatureSize::Medium;
	wchar_t wTag[2] = L"\0";
	bool bKeepAspectRatioSprite = true;
	bool bKeepAspectRatioPortrait = true;
	D2D1_SIZE_F* pGridSquareSize = nullptr;
public:
	SpritePointer(Graphics* const graphics, PiecesW* const p, const Location loc, D2D1_SIZE_F* const GridSquareSize, bool keepaspectsprite = true, bool keepaspectportrait = true) : 
		gfx(graphics), mLocation(loc), pPiece(p), bKeepAspectRatioSprite(keepaspectsprite), bKeepAspectRatioPortrait(keepaspectportrait), pGridSquareSize(GridSquareSize) { mLocation.mDestTag = CalcDestTag(); }
	~SpritePointer() { for (auto& child : vSpriteChild) SafeDelete(&child); for (auto& child : vPortraitChild) SafeDelete(&child); SafeDeleteArray(&LoadedPiece); }
	SpritePointer(const SpritePointer&) = delete;
	SpritePointer& operator=(const SpritePointer&) = delete;
public:
	const char* GetPieceBuffer() { return LoadedPiece; }
	void SetPiecePointer(PiecesW* const ptr) { pPiece = ptr; }
	void MoveDestSprite(const D2D1_SIZE_F changes);
	void MovePortraitSprite(const D2D1_SIZE_F changes);
	void MoveResizedDestSprite(const D2D1_SIZE_F changes);
	void SetDestSpritePosition(const D2D1_POINT_2F point);
	void SetDestResizedSpritePosition(const D2D1_POINT_2F point);
	void SetPortraitPosition(const D2D1_POINT_2F point);
	virtual void SetDestSprite(const D2D1_RECT_F d, bool ApplyRebuild = true);
	void SetDestPortrait(const D2D1_RECT_F d);
	void IncreaseCreatureSize();
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
	virtual void DrawSprite(Graphics* const gfx, bool back = true);
	void DrawPortrait(Graphics* const gfx, bool back = true);
	void BuildResizedDestSprite();
	void ResizeSprite();
	void SetKeepAspectSprite() { bKeepAspectRatioSprite = true; }
	void UnsetKeepAspectSprite() { bKeepAspectRatioSprite = false; }
	void ToggleAspectRatioSprite() { bKeepAspectRatioSprite ^= true; }
	void SetKeepAspectPortrait() { bKeepAspectRatioPortrait = true; }
	void UnsetKeepAspectPortrait() { bKeepAspectRatioPortrait = false; }
	void ToggleAspectRatioPortrait() { bKeepAspectRatioPortrait ^= true; }
	void AddSpriteChild(PiecesW* const piece);
	void AddAoeSpriteChild(SpritePointer* const p);
	void AddPortraitChild(PiecesW* const piece);
	void RemoveChildren();
	void AddTag(wchar_t w);
	virtual const char* GetSaveInformation();
	virtual const char* CreateSaveInformation();
	virtual const uint32_t CalcSaveSize();
	virtual const bool LoadSaveBuffer(const char* Buffer);
	const D2D1_RECT_F CalcDestTag();
public:
	const bool IsSelected() { return bSelected; }
	const bool IsKeepAspectSprite() { return bKeepAspectRatioSprite; }
	const bool IsKeepAspectPortrait() { return bKeepAspectRatioPortrait; }
	virtual const bool PointInSprite(D2D1_POINT_2F p) { return (p.x > mLocation.mDestSprite.left && p.x < mLocation.mDestSprite.right && p.y > mLocation.mDestSprite.top && p.y < mLocation.mDestSprite.bottom); }
	const D2D1_RECT_F GetDestSprite() { return mLocation.mDestSprite; }
	const D2D1_RECT_F GetDestTag() { return mLocation.mDestTag; }
	const D2D1_RECT_F GetDestResizedSprite() { return mLocation.mResizedDestSprite; }
	const D2D1_RECT_F GetDestPortrait() { return mLocation.mDestPortrait; }
	const D2D1_SIZE_F GetSpriteFrameSize() { return pPiece->GetSprite()->GetFrameSize(); }
	const D2D1_SIZE_F GetPortraitFrameSize() { return pPiece->GetPortrait()->GetFrameSize(); }
	const D2D1_SIZE_F GetDestResizedSpriteSize() { return D2D1::SizeF(mLocation.mResizedDestSprite.right - mLocation.mResizedDestSprite.left, mLocation.mResizedDestSprite.bottom - mLocation.mResizedDestSprite.top); }
	const unsigned long GetLayer() { return mLocation.uLayer; }
	const unsigned long GetRoom() { return mLocation.uRoom; }
	const Location GetLocation() { return mLocation; }
	PiecesW* const GetPieces() { return pPiece; }
	Sprite* const GetSprite() { return pPiece->GetSprite(); }
	Sprite* const GetPortrait() { return pPiece->GetPortrait(); }
	std::vector<SpritePointer*> vSpriteChild;
	std::vector<SpritePointer*> vPortraitChild;
};

class AoeSpritePointer : public SpritePointer
{
private:
	const float PI = 3.141592658f;
protected:
	D2D1::Matrix3x2F RotationMatrix = D2D1::Matrix3x2F::Identity();
	D2D1::Matrix3x2F TranslationMatrix = D2D1::Matrix3x2F::Identity();
	D2D1_COLOR_F mEdgeColor;
	D2D1_COLOR_F mFillColor;
	float fRadius = 0.0f;
	float fWidth = 0.0f;
	float fLength = 0.0f;
	float fThickness = 1.0f;
	float fRotation = 0.0f;
	D2D1_POINT_2F mCenter = D2D1::Point2F();
	D2D1_SIZE_F mBitmapSize = D2D1::SizeF();
	ID2D1Bitmap* pAoeBitmap = nullptr;
	D2D1_SIZE_F mTranslation = D2D1::SizeF();
public:
	enum class AoeTypes
	{
		Cone,
		Cube,
		Sphere,
		Line,
		Cylinder,
		Invalid
	} mType;
public:
	AoeSpritePointer(Graphics* const graphics, const AoeTypes type, D2D1_COLOR_F EdgeColor, D2D1_COLOR_F FillColor, PiecesW* const p, const Location loc, D2D1_SIZE_F* const GridSquareSize) : mEdgeColor(EdgeColor), mFillColor(FillColor), mType(type), SpritePointer(graphics, p, loc, GridSquareSize, false, false) {}
	~AoeSpritePointer() { SafeRelease(&pAoeBitmap); }
	void SetRadius(const float rad) { fRadius = rad; }
	void SetWidth(const float width) { fWidth = width; }
	void SetThickness(const float thickness) { fThickness = thickness; }
	void SetLength(const float len) { fLength = len; }
	void SetFillColor(const D2D1_COLOR_F c) { mFillColor = c; }
	void SetEdgeColor(const D2D1_COLOR_F c) { mEdgeColor = c; }
	void SetRotation(const float r) { fRotation = r * (180.0f / PI); }
	void SetTranslation(const D2D1_SIZE_F dist) { TranslationMatrix = D2D1::Matrix3x2F::Translation(dist); }
	void SetStartPoint(const D2D1_POINT_2F p);
	void SetDestSprite(const D2D1_RECT_F d, bool ApplyRebuild = true) override
	{ 
		//mLocation.mDestSprite = d; 
		SetStartPoint(D2D1::Point2F(d.left, d.top));
	}
	void UpdateRotationMatrix();
	const D2D1_COLOR_F GetFillColor() { return mFillColor; }
	const D2D1_COLOR_F GetEdgeColor() { return mEdgeColor; }
	const float GetRotation() { return fRotation; }
	const float GetThickness() { return fThickness; }
	const float GetRadius() { return fRadius; }
	const float GetWidth() { return fWidth; }
	const float GetLength() { return fLength; }
	const D2D1_POINT_2F GetCenter() { return mCenter; }
	const ID2D1Bitmap* GetAoeBitmap() { return pAoeBitmap; }
	const D2D1::Matrix3x2F GetTransformsMatrix() { return RotationMatrix * TranslationMatrix; }
	void BuildShape();
	void BuildCone();
	void BuildSphere();
	void BuildLine();
	void BuildCube();
	void Draw();
	void DrawSprite(Graphics* const gfx, bool back = true) override { Draw(); }
	const bool PointInSprite(const D2D1_POINT_2F p) override;
	const bool PointInCone(const D2D1_POINT_2F p);
	const bool PointInSphere(const D2D1_POINT_2F p);
	const bool PointInCylinder(const D2D1_POINT_2F p);
	const bool PointInCube(const D2D1_POINT_2F p);
	const bool PointInLine(const D2D1_POINT_2F p);

	const char* GetSaveInformation() override;
	const char* CreateSaveInformation() override;
	const bool LoadSaveBuffer(const char* Buffer) override;
	const uint32_t CalcSaveSize() override;
};