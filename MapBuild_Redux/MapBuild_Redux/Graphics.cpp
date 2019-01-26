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