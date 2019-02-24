#pragma once
#include <vector>
#include <memory>
#include "Graphics.h"
#include "SafeReleaseMemory.h"

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
public:
	Wall(Graphics* const graphics, D2D1_POINT_2F* const MouseCoordinates) : pMouseCoordinates(MouseCoordinates), gfx(graphics) {}
	~Wall() { while (vGeometryPathHistory.size()) RemoveLastGeometry(); }
public:
	void AddPoint(const D2D1_POINT_2F p) { vPoints.push_back(p); if (vPoints.size() > 1) BuildGeometry(); }
	void RemoveLastPoint() { if (vPoints.size()) vPoints.pop_back(); RemoveLastGeometry(); }
	void RemoveLastGeometry();
	void BuildGeometry(const bool bClose = false);
	void Draw();
	void DrawPreview();
	void SetThickness(const float thickness) { fThickness = thickness; fRadius = fThickness * 0.5f; }
	void SetColor(const D2D1_COLOR_F color) { mColor = color; }
	void SetGeometry(const bool bClose = false);
};