#include "Walls.h"

void Wall::BuildGeometry(const bool bClose)
{
	ID2D1PathGeometry* pG = nullptr;
	if (!SUCCEEDED(gfx->GetFactory()->CreatePathGeometry(&pG)))
	{
		pG = nullptr;
		return;
	}
	vGeometryPathHistory.push_back(pG);

	ID2D1GeometrySink* pSink = nullptr;
	if (SUCCEEDED(pG->Open(&pSink)))
	{
		pSink->BeginFigure(vPoints.front(), D2D1_FIGURE_BEGIN_FILLED);
		for (auto& p : vPoints)
		{
			if (&p == &vPoints.front()) continue;
			pSink->AddLine(p);
		}
		bClose ? pSink->EndFigure(D2D1_FIGURE_END_CLOSED) : pSink->EndFigure(D2D1_FIGURE_END_OPEN);
		pSink->Close();
		SafeRelease(&pSink);
		return;
	}
	RemoveLastGeometry();
}

void Wall::RemoveLastGeometry()
{
	if (vGeometryPathHistory.empty()) return;

	SafeRelease(&vGeometryPathHistory.back());
	vGeometryPathHistory.pop_back();
}

void Wall::Draw()
{
	if (pSetGeometry)
	{
		gfx->DrawGeometry(gfx->GetCompatibleTarget(), pSetGeometry, mColor, fThickness);
		return;
	}

	if (vPoints.empty()) return;
	if (vPoints.size() == 1)
		gfx->FillCircle(gfx->GetCompatibleTarget(), vPoints.front(), fRadius, mColor);
	else
	{
		if (vGeometryPathHistory.size())
		{
			gfx->DrawGeometry(gfx->GetCompatibleTarget(), vGeometryPathHistory.back(), mColor, fThickness);
		}
	}
}

void Wall::DrawPreview()
{
	Draw();
	if (vPoints.size()) gfx->DrawLine(gfx->GetCompatibleTarget(), vPoints.back(), *pMouseCoordinates, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f), fThickness);
}

void Wall::DrawPreview(const D2D1_POINT_2F p)
{
	Draw();
	if (vPoints.size()) gfx->DrawLine(gfx->GetCompatibleTarget(), vPoints.back(), p, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f), fThickness);
}

void Wall::SetGeometry(const bool bClose)
{
	if (vGeometryPathHistory.empty()) return;
	if (bClose) BuildGeometry(true);

	pSetGeometry = vGeometryPathHistory.back();
	vGeometryPathHistory.pop_back();
	while (vGeometryPathHistory.size()) RemoveLastGeometry();
	while (vPoints.size()) RemoveLastPoint();
}