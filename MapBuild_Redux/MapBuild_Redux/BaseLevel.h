#pragma once
#include "Level.h"
#include "SideMenu.h"
#include <vector>

class BaseLevel : public Level
{
private:
	std::vector<InteractObjects*> IObjects;
	D2D1_POINT_2F Center = D2D1::Point2F();
	D2D1_SIZE_F GridSquareSize = D2D1::SizeF(64.0f, 64.0f);
	D2D1_SIZE_F Scale = D2D1::SizeF(1.0f, 1.0f);
	D2D1_SIZE_F ScaleSpeed = D2D1::SizeF(0.05f, 0.05f);
	D2D1_SIZE_F Offset = D2D1::SizeF();
	D2D1_POINT_2F* pMouseCoordinate = nullptr;
	D2D1_POINT_2F TranslatedCoordinates = D2D1::Point2F();
	D2D1_POINT_2F MenuCoordinates = D2D1::Point2F();
	D2D1_SIZE_F WindowSize = D2D1::SizeF();
	D2D1_SIZE_F ScreenSize = D2D1::SizeF();
	D2D1_RECT_F m_ClientWindow = D2D1::RectF();
	float RotationAngle = 0.0f;
	float RotationSpeed = 30.0f;
	float MovementSpeed = 100.0f;
	D2D1_POINT_2F RotationCenter = D2D1::Point2F();	
	bool bShowSideMenu = true;
	D2D1_COLOR_F GridBackgroundColor = D2D1::ColorF(0.75f, 0.75f, 0.75f);
	D2D1::Matrix3x2F Transforms = D2D1::Matrix3x2F::Identity();
public:
	BaseLevel(Graphics* const graphics, D2D1_POINT_2F* const pMousePosition, int WindowX, int WindowY);
	~BaseLevel();
	void Load(Keyboard* const keyboard, Mouse* const mouse) override;
	void Unload() override;
	void Render() override;
	void ProcessEvents(double dDelta) override;
	void Update(double dDelta) override;
private:
	void DrawSideMenu();
	void ProcessKeyboardEvents(double dDelta);
	void ProcessMouseEvents(double dDelta);
	int afps = 0;
public:
	SideMenu* pSideMenu = nullptr;
};