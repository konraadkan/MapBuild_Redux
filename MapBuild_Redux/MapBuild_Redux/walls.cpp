#include "Walls.h"

void Wall::BuildGeometry()
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
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
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
		gfx->FillCircle(gfx->GetCompatibleTarget(), vPoints.front(), fThickness, mColor);
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
	if (vPoints.size() > 1) gfx->DrawLine(gfx->GetCompatibleTarget(), vPoints.back(), *pMouseCoordinates, D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.5f), fThickness);
}

void Wall::SetGeometry()
{
	if (vGeometryPathHistory.empty()) return;

	pSetGeometry = vGeometryPathHistory.back();
	vGeometryPathHistory.pop_back();
	while (vGeometryPathHistory.size()) RemoveLastGeometry();
	while (vPoints.size()) RemoveLastPoint();
}