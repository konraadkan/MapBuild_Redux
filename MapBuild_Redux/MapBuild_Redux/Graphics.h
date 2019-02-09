#pragma once

#include<Windows.h>
#include<d2d1_1.h>
#include<dwrite.h>
#include<queue>
#include<vector>
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
	struct CustomGeometryDetails
	{
		D2D1_COLOR_F Color = D2D1::ColorF(0.0f, 0.0f, 0.0f);
		bool bFill = false;
		bool bHide = false;
		float thickness = 1.0f;
	};
private:
	ID2D1Factory* m_Factory = NULL;
	ID2D1HwndRenderTarget* m_RenderTarget = NULL;
	ID2D1SolidColorBrush* m_Brush = NULL;
	IDWriteFactory* m_WriteFactory = NULL;
	IDWriteTextFormat* m_WriteFormat = NULL;
	IDWriteTextFormat* m_WriteFormatSmall = NULL;
	ID2D1BitmapRenderTarget* pCompatibleTarget = NULL;	

	std::vector<ID2D1PathGeometry*> pGeometryPaths;
	std::vector<CustomGeometryDetails> GeometryDetails;

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
	bool BuildCustomGeometry(std::queue<D2D1_POINT_2F> points, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), bool fill = false, float thickness = 1.0f, D2D1_FIGURE_BEGIN figurebegin = D2D1_FIGURE_BEGIN_FILLED, D2D1_FIGURE_END figureend = D2D1_FIGURE_END_CLOSED);
	bool BuildCustomGeometry(std::vector<ID2D1PathGeometry*>& pGeometry, std::queue<D2D1_POINT_2F> points, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), bool fill = false, float thickness = 1.0f, D2D1_FIGURE_BEGIN figurebegin = D2D1_FIGURE_BEGIN_FILLED, D2D1_FIGURE_END figureend = D2D1_FIGURE_END_CLOSED);
	bool RectOverlap(const D2D1_RECT_F r1, const D2D1_RECT_F r2, bool bFirst = true);
	bool PointInRect(const D2D1_POINT_2F p, const D2D1_RECT_F rect);
	const D2D1_RECT_F TransformRect(const D2D1_RECT_F rect, const D2D1::Matrix3x2F Transforms);

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
	template<typename T> bool DrawCircle(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_POINT_2F center, float radius, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		D2D1_RECT_F circlerect = D2D1::RectF(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
		if (!RectOverlap(TransformRect(circlerect, Transforms), ClientArea)) return false;

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
	template<typename T> bool FillCircle(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_POINT_2F center, float radius, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f))
	{
		D2D1_RECT_F circlerect = D2D1::RectF(center.x - radius, center.y - radius, center.x + radius, center.y + radius);
		if (!RectOverlap(TransformRect(circlerect, Transforms), ClientArea)) return false;

		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->FillEllipse(D2D1::Ellipse(center, radius, radius), m_Brush);
		return true;
	}
	template<typename T> bool DrawEllipse(T target, D2D1_POINT_2F center, float radiusX, float radiusY, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->DrawEllipse(D2D1::Ellipse(center, radiusX, radiusY), m_Brush, thickness);
		return true;
	}
	template<typename T> bool DrawEllipse(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_POINT_2F center, float radiusX, float radiusY, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		D2D1_RECT_F circlerect = D2D1::RectF(center.x - radiusX, center.y - radiusY, center.x + radiusX, center.y + radiusY);
		if (!RectOverlap(TransformRect(circlerect, Transforms), ClientArea)) return false;

		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->DrawEllipse(D2D1::Ellipse(center, radiusX, radiusY), m_Brush, thickness);
		return true;
	}
	template<typename T> bool FillEllipse(T target, D2D1_POINT_2F center, float radiusX, float radiusY, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f))
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->FillEllipse(D2D1::Ellipse(center, radiusX, radiusY), m_Brush);
		return true;
	}
	template<typename T> bool FillEllipse(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_POINT_2F center, float radiusX, float radiusY, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f))
	{
		D2D1_RECT_F circlerect = D2D1::RectF(center.x - radiusX, center.y - radiusY, center.x + radiusX, center.y + radiusY);
		if (!RectOverlap(TransformRect(circlerect, Transforms), ClientArea)) return false;

		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->FillEllipse(D2D1::Ellipse(center, radiusX, radiusY), m_Brush);
		return true;
	}
	template<typename T> bool DrawLine(T target, D2D1_POINT_2F p1, D2D1_POINT_2F p2, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->DrawLine(p1, p2, m_Brush, thickness);
		return true;
	}
	template<typename T> bool DrawLine(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_POINT_2F p1, D2D1_POINT_2F p2, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		D2D1_RECT_F rect = D2D1::RectF(p1.x, p1.y, p2.x, p2.y);
		if (!RectOverlap(TransformRect(rect, Transforms), ClientArea)) return false;

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
	template<typename T> bool DrawRect(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		if (!RectOverlap(TransformRect(area, Transforms), ClientArea)) return false;

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
	template<typename T> bool DrawRoundedRect(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float radiusX = 1.0f, float radiusY = 1.0f, float thickness = 1.0f)
	{
		if (!RectOverlap(TransformRect(area, Transforms), ClientArea)) return false;

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
	template<typename T> bool FillRect(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f))
	{
		if (!RectOverlap(TransformRect(area, Transforms), ClientArea)) return false;

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
	template<typename T> bool FillRoundedRect(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_RECT_F area, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float radiusX = 1.0f, float radiusY = 1.0f)
	{
		if (!RectOverlap(TransformRect(area, Transforms), ClientArea)) return false;

		if (!m_Brush) return false;
		m_Brush->SetColor(color);
		target->FillRoundedRectangle(D2D1::RoundedRect(area, radiusX, radiusY), m_Brush);
		return true;
	}
	template<typename T> bool OutputText(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT p_alignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	{
		if (!RectOverlap(TransformRect(targetArea, Transforms), ClientArea)) return false;

		return OutputText(target, text, targetArea, color, alignment, p_alignment);
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
			//target->SetTransform(oMatrix * D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f)));
			target->SetTransform(D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f)) * oMatrix);
			OutputText(target, text, trect, color, alignment, p_alignment);
			target->SetTransform(oMatrix);
		}

		m_WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_WriteFormat->SetParagraphAlignment(p);
		return true;
	}
	template<typename T> bool OutputTextSmall(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, const wchar_t* text, D2D1_RECT_F targetArea, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT p_alignment = DWRITE_PARAGRAPH_ALIGNMENT::DWRITE_PARAGRAPH_ALIGNMENT_NEAR)
	{
		if (!RectOverlap(TransformRect(targetArea, Transforms), ClientArea)) return false;

		return OutputTextSmall(target, text, targetArea, color, alignment, p_alignment);
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
			//target->SetTransform(oMatrix * D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f)));
			target->SetTransform(D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, scale), D2D1::Point2F(targetArea.left, (targetArea.bottom + targetArea.top) * 0.5f)) * oMatrix);
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
	template<typename T> void ApplyRotation(T target, float RotAngle, D2D1_POINT_2F RotCenter)
	{
		D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Identity(); 
		target->GetTransform(&transform);
		target->SetTransform(transform * D2D1::Matrix3x2F::Rotation(RotAngle, RotCenter));
	}
	template<typename T> const D2D1_POINT_2F GetTransformedPoint(T target, D2D1_POINT_2F p)
	{
		D2D1::Matrix3x2F transform = D2D1::Matrix3x2F::Identity();
		target->GetTransform(&transform);
		if (transform.IsInvertible()) transform.Invert();
		return transform.TransformPoint(p);
	}
	template<typename T> void DrawDefaultGrid(T target, const D2D1::Matrix3x2F Transforms, const D2D1_RECT_F ClientArea, D2D1_SIZE_F size, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f)
	{
		for (float x = 0; x <= static_cast<float>(m_CompatibleTargetSize.width); x += size.width)
		{
			DrawLine(target, Transforms, ClientArea, D2D1::Point2F(x, 0.0f), D2D1::Point2F(x, static_cast<float>(m_CompatibleTargetSize.height)), color, thickness);
		}
		for (float y = 0; y <= static_cast<float>(m_CompatibleTargetSize.height); y += size.height)
		{			
			DrawLine(target, Transforms, ClientArea, D2D1::Point2F(0.0f, y), D2D1::Point2F(static_cast<float>(m_CompatibleTargetSize.width), y), color, thickness);
		}
		DrawRect(target, D2D1::RectF(0.0f, 0.0f, m_CompatibleTargetSize.width, m_CompatibleTargetSize.height), color, thickness * 5.0f);
	}
	template<typename T> void DrawCustomGeometry(T target)
	{
		if (!m_Brush) return;
		for (size_t i = 0; i < pGeometryPaths.size(); i++)
		{
			if (GeometryDetails.size() > i)
			{
				if (GeometryDetails[i].bHide) continue;
				m_Brush->SetColor(GeometryDetails[i].Color);
				if (GeometryDetails[i].bFill)
					target->FillGeometry(pGeometryPaths[i], m_Brush);
				else
					target->DrawGeometry(pGeometryPaths[i], m_Brush, GeometryDetails[i].thickness);
			}
			else
			{
				m_Brush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f));
				target->DrawGeometry(pGeometryPaths[i], m_Brush);
			}
		}
	}
	template<typename T> void DrawGeometry(T target, ID2D1PathGeometry* path, D2D1_COLOR_F color = D2D1::ColorF(0.0f, 0.0f, 0.0f), float thickness = 1.0f, bool bFill = false)
	{
		if (!m_Brush) return;
		m_Brush->SetColor(color);
		if (bFill)
		{
			target->FillGeometry(path, m_Brush);
		}
		target->DrawGeometry(path, m_Brush, thickness);
	}
	template<typename T> void DrawBitmap(T target, ID2D1Bitmap* const pBitmap, const D2D1_RECT_F destrect, const float opacity, const D2D1_RECT_F srcrect)
	{
		target->DrawBitmap(pBitmap, &destrect, opacity, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, &srcrect);
	}
};