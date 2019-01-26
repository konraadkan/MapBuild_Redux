#pragma once
#include "Level.h"

class BaseLevel : public Level
{
private:
	D2D1_POINT_2F Center = {};
	D2D1_SIZE_F Scale = D2D1::SizeF(1.0f, 1.0f);
	D2D1_SIZE_F Offset = D2D1::SizeF();
	D2D1_POINT_2F* pMouseCoordinate = nullptr;
	D2D1_POINT_2F TranslatedCoordinates = D2D1::Point2F();
public:
	BaseLevel(Graphics* const graphics, D2D1_POINT_2F* const pMousePosition);
	~BaseLevel();
	void Load(Keyboard* const keyboard) override;
	void Unload() override;
	void Render() override;
	void ProcessEvents(double dDelta) override;
	void Update(double dDelta) override;	
};