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
	void SwapBuffer();	
	unsigned long GetMaxBitmapSize() { return m_RenderTarget->GetMaximumBitmapSize(); }
	float GetOutputTextWidth(const wchar_t* string, D2D1_SIZE_F maxsize, bool bFormalSmall = false);
	float GetOutputTextHeight(const wchar_t* string, D2D1_SIZE_F maxsize, bool bFormatSmall = false);
	bool ResizeCompatibleRenderTarget(D2D1_SIZE_F newsize);
	const D2D1_SIZE_F GetCompatibleTargetSize() { return m_CompatibleTargetSize; }

	ID2D1HwndRenderTarget* GetRenderTarget() { return m_RenderTarget; }
	ID2D1BitmapRenderTarget* GetCompatibleTarget() { return pCompatibleTarget; }
	ID2D1Factory* GetFactory() { return m_Factory; }
public:
	template<typename T> void BeginDraw(T target) { target->BeginDraw(); }
	template<typename T> void EndDraw(T target) { target->EndDraw(); }
	template<typename T> void ClearScreen(T target, D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 1.0f)) {	target->Clear(color); }
	template<typename T> bool DrawCircle(T target, D2D1_POINT_2F center, float radius, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->DrawEllipse(D2D1::Ellipse(center, radius, radius), m_Brush, thickness);
		return true;
	}
	template<typename T> bool FillCircle(T target, D2D1_POINT_2F center, float radius, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f))
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->FillEllipse(D2D1::Ellipse(center, radius, radius), m_Brush);
		return true;
	}
	template<typename T> bool DrawLine(T target, D2D1_POINT_2F p1, D2D1_POINT_2F p2, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->DrawLine(p1, p2, m_Brush, thickness);
		return true;
	}
	template<typename T> bool DrawRect(T target, D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->DrawRectangle(area, m_Brush, thickness);
		return true;
	}
	template<typename T> bool DrawRoundedRect(T target, D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float radiusX = 1.0f, float radiusY = 1.0f, float thickness = 1.0f)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->DrawRoundedRectangle(D2D1::RoundedRect(area, radiusX, radiusY), m_Brush, thickness);
		return true;
	}
	template<typename T> bool FillRect(T target, D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f))
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->FillRectangle(area, m_Brush);
		return true;
	}
	template<typename T> bool FillRoundedRect(T target, D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float radiusX = 1.0f, float radiusY = 1.0f)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->FillRoundedRectangle(D2D1::RoundedRect(area, radiusX, radiusY), m_Brush);
		return true;
	}
	template<typename T> bool OutputText(T target, const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT p_alignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);

		DWRITE_PARAGRAPH_ALIGNMENT p = m_WriteFormat->GetParagraphAlignment();
		m_WriteFormat->SetParagraphAlignment(p_alignment);
		m_WriteFormat->SetTextAlignment(alignment);

		float temp = GetOutputTextWidth(text, D2D1::SizeF((targetArea.right - targetArea.left) * 2.5f, targetArea.bottom - targetArea.top));
		if (temp <= (targetArea.right - targetArea.left))
		{
			if (temp >= 0.0f) target->DrawTextA(text, lstrlenW(text), m_WriteFormat, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		}
		else
		{
			float scale = (targetArea.right - targetArea.left) / temp;
			D2D1::Matrix3x2F oMatrix = D2D1::Matrix3x2F::Identity();
			target->GetTransform(&oMatrix);
			D2D1_RECT_F trect = D2D1::RectF(targetArea.left, targetArea.top, targetArea.left + temp, targetArea.bottom);
			target->SetTransform(oMatrix * D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f)));
			OutputText(target, text, trect, color, alignment, p_alignment);
			target->SetTransform(oMatrix);
		}

		m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_WriteFormat->SetParagraphAlignment(p);
		return true;
	}
	template<typename T> bool OutputTextSmall(T target, const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT p_alignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);

		DWRITE_PARAGRAPH_ALIGNMENT p = m_WriteFormat->GetParagraphAlignment();
		m_WriteFormatSmall->SetParagraphAlignment(p_alignment);
		m_WriteFormatSmall->SetTextAlignment(alignment);

		float temp = GetOutputTextWidth(text, D2D1::SizeF((targetArea.right - targetArea.left) * 2.5f, targetArea.bottom - targetArea.top), true);
		if (temp <= (targetArea.right - targetArea.left))
		{
			if (temp >= 0.0f) target->DrawTextA(text, lstrlenW(text), m_WriteFormatSmall, targetArea, m_Brush, D2D1_DRAW_TEXT_OPTIONS_NONE, DWRITE_MEASURING_MODE_NATURAL);
		}
		else
		{
			float scale = (targetArea.right - targetArea.left) / temp;
			D2D1::Matrix3x2F oMatrix = D2D1::Matrix3x2F::Identity();
			target->GetTransform(&oMatrix);
			D2D1_RECT_F trect = D2D1::RectF(targetArea.left, targetArea.top, targetArea.left + temp, targetArea.bottom);
			target->SetTransform(oMatrix * D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f)));
			OutputText(target, text, trect, color, alignment, p_alignment);
			target->SetTransform(oMatrix);
		}

		m_WriteFormatSmall->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_WriteFormatSmall->SetParagraphAlignment(p);
		return true;
	}
	template<typename T> void RestoreTransform(T target) { target->SetTransform(D2D1::Matrix3x2F::Identity()); }
	template<typename T> void ApplyScale(T target, D2D1_SIZE_F size, D2D1_POINT_2F center)
	{
		D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Identity();
		target->GetTransform(&transform);
		target->SetTransform(transform * D2D1::Matrix3x2F::Scale(size, center));
	}
	template<typename T> void ApplyTranslation(T target, D2D1_SIZE_F offset)
	{
		D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Identity();
		target->GetTransform(&transform);
		target->SetTransform(transform * D2D1::Matrix3x2F::Translation(offset));
	}
	template<typename T> const D2D1_POINT_2F GetTransformedPoint(T target, D2D1_POINT_2F p)
	{
		D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Identity();
		target->GetTransform(&transform);
		transform.Invert();
		return transform.TransformPoint(p);
	}
};