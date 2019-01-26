#pragma once

#include<Windows.h>
#include<d2d1_1.h>
#include<dwrite.h>
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "d2d1.lib")

class Graphics
{
private:
	template<class T> void SafeRelease(T** ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}
private:
	ID2D1Factory* m_Factory = NULL;
	ID2D1HwndRenderTarget* m_RenderTarget = NULL;
	ID2D1SolidColorBrush* m_Brush = NULL;
	IDWriteFactory* m_WriteFactory = NULL;
	IDWriteTextFormat* m_WriteFormat = NULL;
	IDWriteTextFormat* m_WriteFormatSmall = NULL;
	ID2D1BitmapRenderTarget* pCompatibleTarget = NULL;

	D2D1_SIZE_F m_CompatibleTargetSize = D2D1::SizeF();
public:
	Graphics();
	~Graphics();

	bool Init(HWND hWnd, D2D1_SIZE_F CompatibleRenderSize = D2D1::SizeF());
	void BeginDraw() { m_RenderTarget->BeginDraw(); }
	void EndDraw() { m_RenderTarget->EndDraw(); }

	void SwapBuffer();

	template<typename T> void ClearScreenS(T path, D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 1.0f))
	{
		path->Clear(color);
	}
	void ClearScreen(D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 1.0f));
	bool DrawCircle(D2D1_POINT_2F center, float radius, D2D1_COLOR_F color, float thickness = 1.0f);
	bool FillCircle(D2D1_POINT_2F center, float radius, D2D1_COLOR_F color);

	bool DrawLine(D2D1_POINT_2F p1, D2D1_POINT_2F p2, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f);
	bool DrawRect(D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f);
	bool DrawRoundedRect(D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float radiusX = 1.0f, float radiusY = 1.0f, float thickness = 1.0f);
	bool FillRect(D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f));
	bool FillRoundedRect(D2D1_RECT_F area, D2D1_COLOR_F = D2D1::ColorF(0.0f, 0.0f, 0.0f), float radiusX = 1.0f, float radiusY = 1.0f);

	bool OutputText(const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT p_alignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool useRenderTarget = false);
	bool OutputTextRenderTarget(const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT p_alignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	bool OutputTextSmall(const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT p_alignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR, bool useRenderTarget = false);

	unsigned long GetMaxBitmapSize() {	return m_RenderTarget->GetMaximumBitmapSize(); }

	ID2D1HwndRenderTarget* GetRenderTarget() { return m_RenderTarget; }
	ID2D1BitmapRenderTarget* GetCompatibleTarget() { return pCompatibleTarget; }
	ID2D1Factory* GetFactory() { return m_Factory; }

	float GetOutputTextWidth(const wchar_t* string, D2D1_SIZE_F maxsize, bool bFormalSmall = false);
	float GetOutputTextHeight(const wchar_t* string, D2D1_SIZE_F maxsize, bool bFormatSmall = false);

	bool ResizeCompatibleRenderTarget(D2D1_SIZE_F newsize);
};