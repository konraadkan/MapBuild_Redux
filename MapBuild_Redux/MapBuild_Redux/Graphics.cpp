#include "Graphics.h"

Graphics::Graphics()
{
	m_Factory = NULL;
	m_RenderTarget = NULL;
	m_Brush = NULL;
}

Graphics::~Graphics()
{
	while (pGeometryPaths.size())
	{
		SafeRelease(&pGeometryPaths.back());
		pGeometryPaths.pop_back();
	}
	SafeRelease(&m_Factory);
	SafeRelease(&m_RenderTarget);
	SafeRelease(&m_WriteFactory);
	SafeRelease(&m_WriteFormat);
	SafeRelease(&m_Brush);
	SafeRelease(&m_WriteFormatSmall);
	SafeRelease(&pCompatibleTarget);
}

bool Graphics::ResizeCompatibleRenderTarget(D2D1_SIZE_F newsize)
{
	if (!newsize.height && !newsize.width) return false;
	ID2D1BitmapRenderTarget* oCompatibleTarget = pCompatibleTarget;
	pCompatibleTarget = nullptr;
	if (!SUCCEEDED(m_RenderTarget->CreateCompatibleRenderTarget(newsize, &pCompatibleTarget)))
	{
		pCompatibleTarget = oCompatibleTarget;
		oCompatibleTarget = nullptr;
		return false;
	}
	SafeRelease(&oCompatibleTarget);
	m_CompatibleTargetSize = newsize;
	return true;
}

bool Graphics::Init(HWND hWnd, D2D1_SIZE_F CompatibleRenderSize)
{
	HRESULT result = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_Factory);
	if (result != S_OK) return false;

	RECT rect;
	GetClientRect(hWnd, &rect);

	result = m_Factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWnd, D2D1::SizeU(rect.right, rect.bottom), D2D1_PRESENT_OPTIONS_NONE), &m_RenderTarget);
	if (result != S_OK) return false;

	result = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_WriteFactory));
	if (result != S_OK) return false;

	result = m_WriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_CONDENSED, 64.0f, L"en-us", &m_WriteFormat);
	if (result != S_OK) return false;

	result = m_WriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_CONDENSED, 16.0f, L"en-us", &m_WriteFormatSmall);
	if (result != S_OK) return false;

	result = m_RenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f), &m_Brush);
	if (result != S_OK) return false;

	if (!CompatibleRenderSize.width && !CompatibleRenderSize.height)
	{
		m_CompatibleTargetSize.width = static_cast<float>(rect.right) * 2.0f;
		m_CompatibleTargetSize.height = static_cast<float>(rect.bottom) * 2.0f;
	}
	result = m_RenderTarget->CreateCompatibleRenderTarget(m_CompatibleTargetSize, &pCompatibleTarget);
	if (result != S_OK) return false;

	return true;
}

float Graphics::GetOutputTextWidth(const wchar_t* string, D2D1_SIZE_F maxsize, bool bFormatSmall)
{
	IDWriteTextLayout* textlayout = NULL;
	if (!SUCCEEDED(m_WriteFactory->CreateTextLayout(string, lstrlenW(string), (bFormatSmall ? m_WriteFormatSmall : m_WriteFormat), maxsize.width, maxsize.height, &textlayout))) return -1;
	DWRITE_TEXT_METRICS metrics = {};
	float result = 0.0f;
	if (SUCCEEDED(textlayout->GetMetrics(&metrics)))
	{
		result = metrics.widthIncludingTrailingWhitespace;
	}
	SafeRelease(&textlayout);
	return result;
}

float Graphics::GetOutputTextHeight(const wchar_t* string, D2D1_SIZE_F maxsize, bool bFormatSmall)
{
	IDWriteTextLayout* textlayout = NULL;
	if (!SUCCEEDED(m_WriteFactory->CreateTextLayout(string, lstrlenW(string), (bFormatSmall ? m_WriteFormatSmall : m_WriteFormat), maxsize.width, maxsize.height, &textlayout))) return -1;
	DWRITE_TEXT_METRICS metrics = {};
	float result = 0.0f;
	if (SUCCEEDED(textlayout->GetMetrics(&metrics)))
	{
		result = metrics.height;
	}
	SafeRelease(&textlayout);
	return result;
}

void Graphics::SwapBuffer()
{
	if (!pCompatibleTarget) return;

	ID2D1Bitmap* tBitmap = NULL;
	pCompatibleTarget->GetBitmap(&tBitmap);
	if (tBitmap)
	{
		m_RenderTarget->DrawBitmap(tBitmap, D2D1::RectF(0.0f, 0.0f, m_CompatibleTargetSize.width, m_CompatibleTargetSize.height), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0.0f, 0.0f, m_CompatibleTargetSize.width, m_CompatibleTargetSize.height));
	}
	SafeRelease(&tBitmap);
}

bool Graphics::BuildCustomGeometry(std::queue<D2D1_POINT_2F> points, D2D1_COLOR_F color, bool fill, float thickness, D2D1_FIGURE_BEGIN figurebegin, D2D1_FIGURE_END figureend)
{
	ID2D1PathGeometry* pG = nullptr;
	pGeometryPaths.push_back(pG);

	if (!SUCCEEDED(m_Factory->CreatePathGeometry(&pGeometryPaths.back())))
	{
		pGeometryPaths.pop_back();
		return false;
	}
	ID2D1GeometrySink* pSink = nullptr;
	if (SUCCEEDED(pGeometryPaths.back()->Open(&pSink)))
	{
		pSink->BeginFigure(points.front(), figurebegin);
		points.pop();
		while (points.size())
		{
			pSink->AddLine(points.front());
			points.pop();
		}
		pSink->EndFigure(figureend);
		pSink->Close();
		SafeRelease(&pSink);

		CustomGeometryDetails cgd;
		cgd.bFill = fill;
		cgd.Color = color;
		cgd.thickness = thickness;
		GeometryDetails.push_back(cgd);
		
		return true;
	}
	SafeRelease(&pGeometryPaths.back());
	pGeometryPaths.pop_back();
	return false;
}

bool Graphics::BuildCustomGeometry(std::vector<ID2D1PathGeometry*>& pGeometry, std::queue<D2D1_POINT_2F> points, D2D1_COLOR_F color, bool fill, float thickness, D2D1_FIGURE_BEGIN figurebegin, D2D1_FIGURE_END figureend)
{
	ID2D1PathGeometry* pG = nullptr;
	pGeometry.push_back(pG);

	if (!SUCCEEDED(m_Factory->CreatePathGeometry(&pGeometry.back())))
	{
		pGeometry.pop_back();
		return false;
	}
	ID2D1GeometrySink* pSink = nullptr;
	if (SUCCEEDED(pGeometry.back()->Open(&pSink)))
	{
		pSink->BeginFigure(points.front(), figurebegin);
		points.pop();
		while (points.size())
		{
			pSink->AddLine(points.front());
			points.pop();
		}
		pSink->EndFigure(figureend);
		pSink->Close();
		SafeRelease(&pSink);
		
		return true;
	}
	SafeRelease(&pGeometryPaths.back());
	pGeometryPaths.pop_back();
	return false;
}

bool Graphics::PointInRect(const D2D1_POINT_2F p, const D2D1_RECT_F rect)
{
	return p.x >= rect.left && p.x <= rect.right && p.y >= rect.top && p.y <= rect.bottom;
}

bool Graphics::RectOverlap(const D2D1_RECT_F r1, const D2D1_RECT_F r2, bool bFirst)
{
	if (r1.bottom >= r2.bottom && r1.top <= r2.top && r1.right >= r2.right && r1.left <= r2.left)
		return true;
	if (r1.bottom >= r2.bottom && r1.top <= r2.top)
	{
		if (r1.left >= r2.left && r1.left <= r2.right) return true;
		if (r1.right >= r2.left && r1.right <= r2.right) return true;
	}
	if (r1.left <= r2.left && r1.right >= r2.right)
	{
		if (r1.top >= r2.top && r1.top <= r2.bottom) return true;
		if (r1.bottom >= r2.top && r2.bottom <= r2.bottom) return true;
	}
	if (PointInRect(D2D1::Point2F(r1.left, r1.bottom), r2)) return true;
	if (PointInRect(D2D1::Point2F(r1.left, r1.top), r2)) return true;
	if (PointInRect(D2D1::Point2F(r1.right, r1.bottom), r2)) return true;
	if (PointInRect(D2D1::Point2F(r1.right, r1.top), r2)) return true;
	if (bFirst)	if (RectOverlap(r2, r1, false)) return true;
	return false;
}

const D2D1_RECT_F Graphics::TransformRect(const D2D1_RECT_F rect, const D2D1::Matrix3x2F Transforms)
{
	D2D1::Matrix3x2F temp = Transforms;
	D2D1_POINT_2F tl = D2D1::Point2F(rect.left, rect.top);
	D2D1_POINT_2F tr = D2D1::Point2F(rect.right, rect.bottom);

	tl = temp.TransformPoint(tl);
	tr = temp.TransformPoint(tr);

	return D2D1::RectF(tl.x, tl.y, tr.x, tr.y);
}