#pragma once
#include <vector>
#include <memory>
#include "Graphics.h"
#include "SafeReleaseMemory.h"
#include "Pieces.h"

class Wall : public SafeReleaseMemory
{
private:
	std::vector<ID2D1PathGeometry*> vGeometryPathHistory;
	float fThickness = 1.0f;
	float fRadius = 0.5f;
	D2D1_COLOR_F mColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);
	std::vector<D2D1_POINT_2F> vPoints;
	D2D1_POINT_2F* const pMouseCoordinates = nullptr;
	Graphics* const gfx;
	ID2D1PathGeometry* pSetGeometry = nullptr;
	bool bUseTexture = false;
	bool bIsClosed = false;
	ID2D1BitmapBrush* pBitmapBrush = nullptr;
	SpritePointer* pTexture = nullptr;
	D2D1_SIZE_F* pGridSquareSize;
	void* const pBaseLevel;
public:
	Wall(void* const BaseLevel, D2D1_SIZE_F* const GridSquareSize, Graphics* const graphics, D2D1_POINT_2F* const MouseCoordinates, const bool useTexture) : pBaseLevel(BaseLevel), pGridSquareSize(GridSquareSize), bUseTexture(useTexture), pMouseCoordinates(MouseCoordinates), gfx(graphics) {}
	~Wall() { while (vGeometryPathHistory.size()) RemoveLastGeometry(); SafeRelease(&pBitmapBrush); SafeRelease(&pSetGeometry); SafeDelete(&pTexture); }
	Wall(const Wall&) = delete;
	Wall& operator=(const Wall&) = delete;
public:
	void AddPoint(const D2D1_POINT_2F p) { vPoints.push_back(p); if (vPoints.size() > 1) BuildGeometry(); }
	void RemoveLastPoint() { if (vPoints.size()) vPoints.pop_back(); RemoveLastGeometry(); }
	void RemoveLastGeometry();
	void BuildGeometry(const bool bClose = false);
	void Draw();
	void DrawPreview();
	void DrawPreview(const D2D1_POINT_2F p);
	void SetThickness(const float thickness) { fThickness = thickness; fRadius = fThickness * 0.5f; }
	void SetColor(const D2D1_COLOR_F color) { mColor = color; }
	void SetGeometry(const bool bClose = false);
	void SetUseTexture() { bUseTexture = true; }
	void UnsetUseTexture() { bUseTexture = false; }	
	void SetTexture(SpritePointer* const p);
	const bool PointTouching(const D2D1_POINT_2F p);
	const bool PointsEmpty() { return vPoints.empty(); }
	const uint32_t CalcSaveSize();
	const char* GetSaveInformation() { return CreateSaveInformation(); }
	const std::vector<char> GetSaveInformationV() { return CreateSaveInformationV(); }
	const char* CreateSaveInformation();
	const std::vector<char> CreateSaveInformationV();
	const bool LoadSaveBuffer(const char* Buffer);
	const bool LoadSaveBuffer(ReceiveData& rd);
private:
	void SetTexture(ID2D1Bitmap* const bitmap);
};