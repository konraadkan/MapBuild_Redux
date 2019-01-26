#include "Graphics.h"

Graphics::Graphics()
{
	m_Factory = NULL;
	m_RenderTarget = NULL;
	m_Brush = NULL;
}

Graphics::~Graphics()
{
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

	result = m_WriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 64.0f, L"en-us", &m_WriteFormat);
	if (result != S_OK) return false;

	result = m_WriteFactory->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"en-us", &m_WriteFormatSmall);
	if (result != S_OK) return false;

	result = m_RenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f), &m_Brush);
	if (result != S_OK) return false;

	if (!CompatibleRenderSize.width && !CompatibleRenderSize.height)
	{
		m_CompatibleTargetSize.width = static_cast<float>(rect.right);
		m_CompatibleTargetSize.height = static_cast<float>(rect.bottom);
	}
	result = m_RenderTarget->CreateCompatibleRenderTarget(m_CompatibleTargetSize, &pCompatibleTarget);
	if (result != S_OK) return false;
	return true;
}

void Graphics::ClearScreen(D2D1_COLOR_F color)
{
	m_RenderTarget->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f));
	pCompatibleTarget->Clear(color);
}

bool Graphics::DrawCircle(D2D1_POINT_2F center, float radius, D2D1_COLOR_F color, float thickness)
{
	if (!m_Brush) return false;
	m_Brush->SetColor(color);
	//m_RenderTarget->DrawEllipse(D2D1::Ellipse(center, radius, radius), m_Brush, thickness);
	pCompatibleTarget->DrawEllipse(D2D1::Ellipse(center, radius, radius), m_Brush, thickness);
	return true;
}

bool Graphics::FillCircle(D2D1_POINT_2F center, float radius, D2D1_COLOR_F color)
{
	if (!m_Brush) return false;
	m_Brush->SetColor(color);
	//m_RenderTarget->FillEllipse(D2D1::Ellipse(center, radius, radius), m_Brush);
	pCompatibleTarget->FillEllipse(D2D1::Ellipse(center, radius, radius), m_Brush);
	return true;
}

bool Graphics::DrawLine(D2D1_POINT_2F p1, D2D1_POINT_2F p2, D2D1_COLOR_F color, float thickness)
{
	if (!m_Brush) return false;
	m_Brush->SetColor(color);
	//m_RenderTarget->DrawLine(p1, p2, m_Brush, thickness);
	pCompatibleTarget->DrawLine(p1, p2, m_Brush, thickness);
	return true;
}

bool Graphics::DrawRoundedRect(D2D1_RECT_F area, D2D1_COLOR_F color, float radiusX, float radiusY, float thickness)
{
	if (!m_Brush) return false;
	m_Brush->SetColor(color);
	//m_RenderTarget->DrawRoundedRectangle(D2D1::RoundedRect(area, radiusX, radiusY), m_Brush, thickness);
	pCompatibleTarget->DrawRoundedRectangle(D2D1::RoundedRect(area, radiusX, radiusY), m_Brush, thickness);
	return true;
}

bool Graphics::FillRoundedRect(D2D1_RECT_F area, D2D1_COLOR_F color, float radiusX, float radiusY)
{
	if (!m_Brush) return false;
	m_Brush->SetColor(color);
	//m_RenderTarget->FillRoundedRectangle(D2D1::RoundedRect(area, radiusX, radiusY), m_Brush);
	pCompatibleTarget->FillRoundedRectangle(D2D1::RoundedRect(area, radiusX, radiusY), m_Brush);
	return true;
}

bool Graphics::DrawRect(D2D1_RECT_F area, D2D1_COLOR_F color, float thickness)
{
	if (!m_Brush) return false;
	m_Brush->SetColor(color);
	//m_RenderTarget->DrawRectangle(area, m_Brush, thickness);
	pCompatibleTarget->DrawRectangle(area, m_Brush, thickness);
	return true;
}


bool Graphics::FillRect(D2D1_RECT_F area, D2D1_COLOR_F color)
{
	if (!m_Brush) return false;
	m_Brush->SetColor(color);
	//m_RenderTarget->FillRectangle(area, m_Brush);
	pCompatibleTarget->FillRectangle(area, m_Brush);
	return true;
}

bool Graphics::OutputText(const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color, DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT p_alignment, bool useRenderTarget)
{
	/* original version in case something goes wrong while trying to make scaling version**
	if (!m_Brush) return false;
	m_Brush->SetColor(color);

	DWRITE_PARAGRAPH_ALIGNMENT p = m_WriteFormat->GetParagraphAlignment();
	m_WriteFormat->SetParagraphAlignment(p_alignment);
	m_WriteFormat->SetTextAlignment(alignment);

	//m_RenderTarget->DrawTextA(text, lstrlenW(text), m_WriteFormat, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	pCompatibleTarget->DrawTextA(text, lstrlenW(text), m_WriteFormat, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);

	m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_WriteFormat->SetParagraphAlignment(p);
	return true;
	*/
	if (!m_Brush) return false;
	m_Brush->SetColor(color);

	DWRITE_PARAGRAPH_ALIGNMENT p = m_WriteFormat->GetParagraphAlignment();
	m_WriteFormat->SetParagraphAlignment(p_alignment);
	m_WriteFormat->SetTextAlignment(alignment);

	float temp = GetOutputTextWidth(text, D2D1::SizeF((targetArea.right - targetArea.left) * 2.5f, targetArea.bottom - targetArea.top));
	if (temp <= (targetArea.right - targetArea.left))
	{
		if (temp >= 0.0f) useRenderTarget ? m_RenderTarget->DrawTextA(text, lstrlenW(text), m_WriteFormat, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL) : pCompatibleTarget->DrawTextA(text, lstrlenW(text), m_WriteFormat, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	}
	else
	{
		float scale = (targetArea.right - targetArea.left) / temp;
		D2D1::Matrix3x2F oMatrix = D2D1::Matrix3x2F::Identity();
		useRenderTarget ? m_RenderTarget->GetTransform(&oMatrix) : pCompatibleTarget->GetTransform(&oMatrix);
		D2D1_RECT_F trect = D2D1::RectF(targetArea.left, targetArea.top, targetArea.left + temp, targetArea.bottom);
		useRenderTarget ? m_RenderTarget->SetTransform(oMatrix * D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f))) : pCompatibleTarget->SetTransform(oMatrix * D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f)));
		OutputText(text, trect, color, alignment, p_alignment, useRenderTarget);
		useRenderTarget ? m_RenderTarget->SetTransform(oMatrix) : pCompatibleTarget->SetTransform(oMatrix);
	}

	m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_WriteFormat->SetParagraphAlignment(p);
	return true;
}

bool Graphics::OutputTextRenderTarget(const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color, DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT p_alignment)
{
	if (!m_Brush) return false;
	m_Brush->SetColor(color);

	DWRITE_PARAGRAPH_ALIGNMENT p = m_WriteFormat->GetParagraphAlignment();
	m_WriteFormat->SetParagraphAlignment(p_alignment);
	m_WriteFormat->SetTextAlignment(alignment);

	//m_RenderTarget->DrawTextA(text, lstrlenW(text), m_WriteFormat, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	pCompatibleTarget->DrawTextA(text, lstrlenW(text), m_WriteFormat, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);

	m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_WriteFormat->SetParagraphAlignment(p);
	return true;
}

bool Graphics::OutputTextSmall(const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color, DWRITE_TEXT_ALIGNMENT alignment, DWRITE_PARAGRAPH_ALIGNMENT p_alignment, bool useRenderTarget)
{
	/*
	if (!m_Brush) return false;
	m_Brush->SetColor(color);

	DWRITE_PARAGRAPH_ALIGNMENT p = m_WriteFormat->GetParagraphAlignment();
	m_WriteFormatSmall->SetParagraphAlignment(p_alignment);
	m_WriteFormatSmall->SetTextAlignment(alignment);

	//m_RenderTarget->DrawTextA(text, lstrlenW(text), m_WriteFormatSmall, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	pCompatibleTarget->DrawTextA(text, lstrlenW(text), m_WriteFormatSmall, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);

	m_WriteFormatSmall->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_WriteFormatSmall->SetParagraphAlignment(p);
	return true;
	*/
	if (!m_Brush) return false;
	m_Brush->SetColor(color);

	DWRITE_PARAGRAPH_ALIGNMENT p = m_WriteFormat->GetParagraphAlignment();
	m_WriteFormatSmall->SetParagraphAlignment(p_alignment);
	m_WriteFormatSmall->SetTextAlignment(alignment);

	float temp = GetOutputTextWidth(text, D2D1::SizeF((targetArea.right - targetArea.left) * 2.5f, targetArea.bottom - targetArea.top), true);
	if (temp <= (targetArea.right - targetArea.left))
	{
		if (temp >= 0.0f) useRenderTarget ? m_RenderTarget->DrawTextA(text, lstrlenW(text), m_WriteFormatSmall, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL) : pCompatibleTarget->DrawTextA(text, lstrlenW(text), m_WriteFormatSmall, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
	}
	else
	{
		float scale = (targetArea.right - targetArea.left) / temp;
		D2D1::Matrix3x2F oMatrix = D2D1::Matrix3x2F::Identity();
		useRenderTarget ? m_RenderTarget->GetTransform(&oMatrix) : pCompatibleTarget->GetTransform(&oMatrix);
		D2D1_RECT_F trect = D2D1::RectF(targetArea.left, targetArea.top, targetArea.left + temp, targetArea.bottom);
		useRenderTarget ? m_RenderTarget->SetTransform(oMatrix * D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f))) : pCompatibleTarget->SetTransform(oMatrix * D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f)));
		OutputText(text, trect, color, alignment, p_alignment, useRenderTarget);
		useRenderTarget ? m_RenderTarget->SetTransform(oMatrix) : pCompatibleTarget->SetTransform(oMatrix);
	}

	m_WriteFormatSmall->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_WriteFormatSmall->SetParagraphAlignment(p);
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