#pragma once
#include "Graphics.h"
#include <string>

class TurnCounter
{
private:
	D2D1::Matrix3x2F ShowTransform = D2D1::Matrix3x2F::Identity();
	D2D1::Matrix3x2F HideTransform = D2D1::Matrix3x2F::Identity();
	D2D1_RECT_F mStringRect = D2D1::RectF();
	Graphics* gfx = nullptr;
	D2D1_ELLIPSE mEllpseInfo = {};
	uint32_t* pTurnNumber;
	bool bShow = true;
	D2D1_COLOR_F mFillColor = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.50f);
	float fThickness = 1.0f;
	std::wstring wCounterString = L"0";
public:
	TurnCounter(Graphics* const graphics, const D2D1_ELLIPSE ellipse, uint32_t* const TurnNumber, const D2D1_COLOR_F color, const float thickness = 1.0f);
	void ResetCounter() { if (pTurnNumber) *pTurnNumber = 0u; }
	const uint32_t GetTurnNumber() { if (pTurnNumber) return *pTurnNumber; return 0u; }
	void IncreaseTurnCounter(uint32_t iNumberTurns = 1u) { *pTurnNumber += iNumberTurns; Update(); }
	void DecreaseTurnCounter(uint32_t iNumberTurns = 1u) { if (*pTurnNumber) *pTurnNumber -= iNumberTurns; Update(); }
	void SetHidden() { bShow = false; }
	void UnsetHidden() { bShow = true; }
	void ToggleHidden() { bShow ^= true; }
	const bool IsHidden() { return !bShow; }
	void Draw();
	void Interact();
	const bool PointInEllipse(const D2D1_POINT_2F p);
	void Update();
};