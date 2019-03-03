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
		if (bUseTexture && pBitmapBrush)
		{
			gfx->DrawGeometry(gfx->GetCompatibleTarget(), pSetGeometry, pBitmapBrush);
		}
		else gfx->DrawGeometry(gfx->GetCompatibleTarget(), pSetGeometry, mColor, fThickness);
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
	if (bClose | bUseTexture) BuildGeometry(true);

	pSetGeometry = vGeometryPathHistory.back();
	vGeometryPathHistory.pop_back();
	while (vGeometryPathHistory.size()) RemoveLastGeometry();
	//while (vPoints.size()) RemoveLastPoint(); need the points for the ability to erase...
}

void Wall::SetTexture(ID2D1Bitmap* const bitmap)
{
	SafeRelease(&pBitmapBrush);
	D2D1_BITMAP_BRUSH_PROPERTIES brushProperties = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
	if (FAILED(gfx->GetCompatibleTarget()->CreateBitmapBrush(bitmap, brushProperties, &pBitmapBrush)))
	{
		pBitmapBrush = nullptr;
	}
	bUseTexture = true;
}

const bool Wall::PointTouching(const D2D1_POINT_2F p)
{
	for (auto point : vPoints)
	{
		if (static_cast<int>(p.x) == static_cast<int>(point.x))
		{
			if (static_cast<int>(p.y) == static_cast<int>(point.y))
			{
				return true;
			}
		}
	}
	return false;
}