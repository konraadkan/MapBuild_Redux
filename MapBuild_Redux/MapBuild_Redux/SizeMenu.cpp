#include "SizeMenu.h"
#include <sstream>

SizeMenu::~SizeMenu()
{
	SafeDelete(&ShowHide);
	while (vpChild.size())
	{
		SafeDelete(&vpChild.back());
		vpChild.pop_back();
	}
}

void MeasurementMenu::ConfigureShowHide()
{
	if (ShowHide) SafeDelete(&ShowHide);

	ShowHide = new ClassShapes(ShapeTypes::Ellipses, gfx, false, &mTransform, &mDest, pMouseCoordinates, true);
	ShowHide->SetCenter(mEllipse.point);
	ShowHide->SetRadiusX(mEllipse.radiusX);
	ShowHide->SetRadiusY(mEllipse.radiusY);
	ShowHide->SetDest(D2D1::RectF(mDest.left, mDest.top - mEllipse.radiusY, mDest.right, mDest.top));
	ShowHide->SetInvertTransformPointer(&mInvTransform);
	ShowHide->SetColor(D2D1::ColorF(0.0f, 1.0f, 0.0f));
	ShowHide->SetFill();

	ShowHide->pChild.push_back(new ClassShapes(ShapeTypes::Custom, gfx, false, &mTransform, &mDest, pMouseCoordinates, true));
	std::queue<D2D1_POINT_2F> ps;
	ps.push(D2D1::Point2F(mEllipse.point.x - 10.0f, mEllipse.point.y - 3.0f));
	ps.push(D2D1::Point2F(mEllipse.point.x, mEllipse.point.y - mEllipse.radiusY + 3.0f));
	ps.push(D2D1::Point2F(mEllipse.point.x + 10.0f, mEllipse.point.y - 3.0f));
	ShowHide->pChild.back()->BuildCustomShape(ps, D2D1::ColorF(1.0f, 1.0f, 1.0f));
	ShowHide->pChild.back()->SetFill();
}

void MeasurementMenu::SetDest(const D2D1_RECT_F dest)
{
	mDest = dest;
	SetEllipse(D2D1::Ellipse(D2D1::Point2F(mDest.left + (dest.right - dest.left) * 0.5f, dest.top), (dest.right - dest.left) * 0.5f, 15.0f));
	mRealDest = D2D1::RectF(mDest.left, mDest.top - mEllipse.radiusY, mDest.right, mDest.bottom);
}

void SizeMenu::Draw()
{
	D2D1::Matrix3x2F temp;
	gfx->GetCompatibleTarget()->GetTransform(&temp);
	if (IsHidden()) gfx->GetCompatibleTarget()->SetTransform(mTransform);
	if (!gfx) return;
	ShowHide->Draw();
	
	gfx->FillRect(gfx->GetCompatibleTarget(), mDest, D2D1::ColorF(0.75f, 0.75f, 0.75f));

	for (auto& child : vpChild)
	{
		if (!child) continue;
		child->Draw();
	}

	gfx->DrawRect(gfx->GetCompatibleTarget(), mDest, D2D1::ColorF(0.0f, 0.0f, 0.0f), 2.0f);
	gfx->GetCompatibleTarget()->SetTransform(temp);
}

const bool MeasurementMenu::IsInRealRect()
{
	D2D1_POINT_2F point = mInvTransform.TransformPoint(*pMouseCoordinates);
	return (point.x > mRealDest.left && point.x < mRealDest.right && point.y > mRealDest.top && point.y < mRealDest.bottom);
}

const bool MeasurementMenu::PointInRect()
{
	return PointInRect(*pMouseCoordinates);
}

const bool MeasurementMenu::PointInRect(const D2D1_POINT_2F p)
{
	D2D1_POINT_2F point = mInvTransform.TransformPoint(p);
	return (point.x > mDest.left && point.x < mDest.right && point.y > mDest.top && point.y < mDest.bottom);
}

void MeasurementMenu::SetHidden()
{
	bHidden = true;
	UpdateTransform();
}

void MeasurementMenu::UnsetHidden()
{
	bHidden = false;
	UpdateTransform();
}

void MeasurementMenu::ToggleHidden()
{
	bHidden ^= true;
	UpdateTransform();
}

void MeasurementMenu::UpdateTransform()
{
	if (bHidden)
	{
		mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height - 25));
	}
	else
	{
		mTransform = D2D1::Matrix3x2F::Identity();
	}
	mInvTransform = mTransform;
	mInvTransform.Invert();
}

void AoeSizeMenu::UpdateTransform()
{
	if (bHidden)
	{
		mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height - 25));
	}
	else
	{
		if (pSelectedType)
		{
			switch (*pSelectedType)
			{
			case AoeSpritePointer::AoeTypes::Cone:
				if (pLengthSlider)	mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, -(pLengthSlider->GetDest().bottom - (mDest.bottom - 30.0f))));
				else mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height - 25));
				break;
			case AoeSpritePointer::AoeTypes::Cube:
				if (pLengthSlider)	mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, -(pLengthSlider->GetDest().bottom - (mDest.bottom - 30.0f))));
				else mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height - 25));
				break;
			case AoeSpritePointer::AoeTypes::Cylinder:
				if (pLengthSlider)	mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, -(pRadiusSlider->GetDest().bottom - (mDest.bottom - 30.0f))));
				else mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height - 25));
				break;
			case AoeSpritePointer::AoeTypes::Line:
				if (pLengthSlider)	mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, -(pWidthSlider->GetDest().bottom - (mDest.bottom - 30.0f))));
				else mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height - 25));
				break;
			case AoeSpritePointer::AoeTypes::Sphere:
				if (pLengthSlider)	mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, -(pRadiusSlider->GetDest().bottom - (mDest.bottom - 30.0f))));
				else mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height - 25));
				break;
			default:
				mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height - 25));
			}
		}
		else mTransform = D2D1::Matrix3x2F::Identity();
	}
	mInvTransform = mTransform;
	mInvTransform.Invert();
}

const bool SizeMenu::Interact()
{
	if (ShowHide->PointInRect())
	{
		ToggleHidden();
		return true;
	}
	if (PointInRect())
	{
		for (auto& child : vpChild)
			if (child->PointInRect())
			{
				child->Interact();
				FindSelectedCreatureSize();
			}
		return true;
	}
	return false;
}

void SizeMenu::BuildMenuCreatureSize()
{//this is for the standard size options for creatures
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Fine", D2D1::RectF(mDest.left + 2.0f, mDest.top + 3.0f, mDest.left + 2.0f + 94.0f, mDest.top + 3.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Diminutive", D2D1::RectF(mDest.left + 2.0f, vpChild.back()->GetRect().bottom + 3.0f, mDest.left + 2.0f + 94.0f, vpChild.back()->GetRect().bottom + 3.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Tiny", D2D1::RectF(vpChild.back()->GetRect().right + 2.0f, mDest.top + 3.0f, vpChild.back()->GetRect().right + 2.0f + 94.0f, mDest.top + 2.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Small", D2D1::RectF(vpChild.back()->GetRect().left, vpChild.back()->GetRect().bottom + 3.0f, vpChild.back()->GetRect().right, vpChild.back()->GetRect().bottom + 3.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Medium", D2D1::RectF(vpChild.back()->GetRect().right + 2.0f, mDest.top + 3.0f, vpChild.back()->GetRect().right + 2.0f + 94.0f, vpChild.back()->GetRect().bottom), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Large", D2D1::RectF(vpChild.back()->GetRect().right + 2.0f, mDest.top + 3.0f, vpChild.back()->GetRect().right + 2.0f + 94.0f, mDest.top + 3.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Huge", D2D1::RectF(vpChild.back()->GetRect().left, vpChild.back()->GetRect().bottom + 3.0f, vpChild.back()->GetRect().right, vpChild.back()->GetRect().bottom + 3.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Gargantuan", D2D1::RectF(vpChild.back()->GetRect().right + 2.0f, mDest.top + 3.0f, vpChild.back()->GetRect().right + 2.0f + 94.0f, mDest.top + 2.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"Colossal", D2D1::RectF(vpChild.back()->GetRect().left, vpChild.back()->GetRect().bottom + 3.0f, vpChild.back()->GetRect().right, vpChild.back()->GetRect().bottom + 3.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"1x", D2D1::RectF(mDest.left + 2.0f, vpChild.back()->GetRect().bottom + 3.0f, mDest.left + 2.0f + 94.0f, vpChild.back()->GetRect().bottom + 3.0f + 32.0f), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"2x", D2D1::RectF(vpChild.back()->GetRect().right + 2.0f, vpChild.back()->GetRect().top, vpChild.back()->GetRect().right + 2.0f + 94.0f, vpChild.back()->GetRect().bottom), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"3x", D2D1::RectF(vpChild.back()->GetRect().right + 2.0f, vpChild.back()->GetRect().top, vpChild.back()->GetRect().right + 2.0f + 94.0f, vpChild.back()->GetRect().bottom), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"4x", D2D1::RectF(vpChild.back()->GetRect().right + 2.0f, vpChild.back()->GetRect().top, vpChild.back()->GetRect().right + 2.0f + 94.0f, vpChild.back()->GetRect().bottom), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
	vpChild.push_back(new SizeMenuButtons(this, gfx, &mTransform, &mDest, pMouseCoordinates, L"5x", D2D1::RectF(vpChild.back()->GetRect().right + 2.0f, vpChild.back()->GetRect().top, vpChild.back()->GetRect().right + 2.0f + 94.0f, vpChild.back()->GetRect().bottom), D2D1::ColorF(0.0f, 0.0f, 0.0f), nullptr, true, false));
}

void SizeMenu::FindSelectedCreatureSize()
{
	for (auto& child : vpChild)
	{
		if (child->IsSelected())
		{
			mCreatureSize = WstringToCreatureSize(child->GetLabel());
			break;
		}
	}
}

const CreatureSize SizeMenu::WstringToCreatureSize(const std::wstring wSize)
{
	if (!_wcsicmp(wSize.c_str(), L"Fine"))
		return CreatureSize::Fine;
	if (!_wcsicmp(wSize.c_str(), L"Diminutive"))
		return CreatureSize::Diminutive;
	if (!_wcsicmp(wSize.c_str(), L"Tiny"))
		return CreatureSize::Tiny;
	if (!_wcsicmp(wSize.c_str(), L"Small"))
		return CreatureSize::Small;
	if (!_wcsicmp(wSize.c_str(), L"Medium"))
		return CreatureSize::Medium;
	if (!_wcsicmp(wSize.c_str(), L"Large"))
		return CreatureSize::Large;
	if (!_wcsicmp(wSize.c_str(), L"Huge"))
		return CreatureSize::Huge;
	if (!_wcsicmp(wSize.c_str(), L"Gargantuan"))
		return CreatureSize::Gargantuan;
	if (!_wcsicmp(wSize.c_str(), L"Colossal"))
		return CreatureSize::Colossal;
	if (!_wcsicmp(wSize.c_str(), L"1x"))
		return CreatureSize::OneX;
	if (!_wcsicmp(wSize.c_str(), L"2x"))
		return CreatureSize::TwoX;
	if (!_wcsicmp(wSize.c_str(), L"3x"))
		return CreatureSize::ThreeX;
	if (!_wcsicmp(wSize.c_str(), L"4x"))
		return CreatureSize::FourX;
	if (!_wcsicmp(wSize.c_str(), L"5x"))
		return CreatureSize::FiveX;
	return CreatureSize::Medium;
}

void SizeMenu::SetSelectedCreatureSize(const CreatureSize size)
{
	mCreatureSize = size;
	for (auto& child : vpChild)
	{
		if (WstringToCreatureSize(child->GetLabel()) == size)
		{
			child->Interact();
			break;
		}
	}
}

ThicknessMenu::~ThicknessMenu()
{
	SafeDelete(&pSliderBar);
	SafeDelete(&ShowHide);
	while (vpChild.size())
	{
		SafeDelete(&vpChild.back());
		vpChild.pop_back();
	}
}

void ThicknessMenu::Draw()
{
	D2D1::Matrix3x2F temp;
	gfx->GetCompatibleTarget()->GetTransform(&temp);
	if (IsHidden()) gfx->GetCompatibleTarget()->SetTransform(mTransform);
	if (!gfx) return;
	ShowHide->Draw();

	gfx->FillRect(gfx->GetCompatibleTarget(), mDest, D2D1::ColorF(0.75f, 0.75f, 0.75f));	
	std::wostringstream out;
	out.precision(4);
	out << std::fixed << pSliderBar->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), mTextDest);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), mTextDest, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), mPreviewDest, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->FillCircle(gfx->GetCompatibleTarget(), mPreviewCenter, fPreviewCirlceRadius);
	if (pSliderBar) pSliderBar->Draw();
	for (auto& child : vpChild)
	{
		if (!child) continue;
		child->Draw();
	}

	gfx->DrawRect(gfx->GetCompatibleTarget(), mDest, D2D1::ColorF(0.0f, 0.0f, 0.0f), 2.0f);
	gfx->GetCompatibleTarget()->SetTransform(temp);
}

const bool ThicknessMenu::Interact()
{
	if (ShowHide->PointInRect())
	{
		ToggleHidden();
		return true;
	}
	if (PointInRect())
	{
		for (auto& child : vpChild)
			if (child->PointInRect())
			{
				child->Interact();
			}
		return true;
	}
	return false;
}

const bool AoeSizeMenu::Interact()
{
	if (ShowHide->PointInRect())
	{
		ToggleHidden();
		return true;
	}
	if (PointInRect())
	{
		//check children
		for (auto& child : vpChild)
			if (child->PointInRect())
			{
				child->Interact();
			}
		return true;
	}
	return false;
}

void AoeSizeMenu::DrawConeMenu()
{
	std::wostringstream out;
	out.precision(4);
	out << std::fixed << pOpacitySlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), OpacityTextBox, D2D1::ColorF(0,0,0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());
	out.precision(0);
	out << std::fixed << pLengthSlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), LengthTextBox, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());

	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Opacity", D2D1::RectF(pOpacitySlider->GetDest().left, mDest.top + 2.0f, pOpacitySlider->GetDest().right, pOpacitySlider->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Length (Feet)", D2D1::RectF(pOpacitySlider->GetDest().left, pOpacitySlider->GetDest().bottom, pOpacitySlider->GetDest().right, pLengthSlider->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	if (pOpacitySlider) pOpacitySlider->Draw();
	if (pLengthSlider) pLengthSlider->Draw();
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), OpacityTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), LengthTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
}

void AoeSizeMenu::DrawCubeMenu()
{
	std::wostringstream out;
	out.precision(4);
	out << std::fixed << pOpacitySlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), OpacityTextBox, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());
	out.precision(0);
	out << std::fixed << pLengthSlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), LengthTextBox, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());

	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Opacity", D2D1::RectF(pOpacitySlider->GetDest().left, mDest.top + 2.0f, pOpacitySlider->GetDest().right, pOpacitySlider->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Length (Feet)", D2D1::RectF(pOpacitySlider->GetDest().left, pOpacitySlider->GetDest().bottom, pOpacitySlider->GetDest().right, pLengthSlider->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	if (pOpacitySlider) pOpacitySlider->Draw();
	if (pLengthSlider) pLengthSlider->Draw();
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), OpacityTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), LengthTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
}

void AoeSizeMenu::DrawCylinderMenu()
{
	DrawSphereMenu();
}

void AoeSizeMenu::DrawLineMenu()
{
	std::wostringstream out;
	out.precision(4);
	out << std::fixed << pOpacitySlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), OpacityTextBox, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());
	out.precision(0);
	out << std::fixed << pLengthSlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), LengthTextBox, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());
	out.precision(0);
	out << std::fixed << pWidthSlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), WidthTextBox, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());

	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Opacity", D2D1::RectF(pOpacitySlider->GetDest().left, mDest.top + 2.0f, pOpacitySlider->GetDest().right, pOpacitySlider->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Length (Feet)", D2D1::RectF(pOpacitySlider->GetDest().left, pOpacitySlider->GetDest().bottom, pOpacitySlider->GetDest().right, pLengthSlider->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Width (Feet)", D2D1::RectF(pOpacitySlider->GetDest().left, pLengthSlider->GetDest().bottom, pOpacitySlider->GetDest().right, pWidthSlider->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	if (pOpacitySlider) pOpacitySlider->Draw();
	if (pLengthSlider) pLengthSlider->Draw();
	if (pWidthSlider) pWidthSlider->Draw();
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), OpacityTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), LengthTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), WidthTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
}

void AoeSizeMenu::DrawSphereMenu()
{
	std::wostringstream out;
	out.precision(4);
	out << std::fixed << pOpacitySlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), OpacityTextBox, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());
	out.precision(0);
	out << std::fixed << pRadiusSlider->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), RadiusTextBox, D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	out.str(std::wstring());

	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Opacity", D2D1::RectF(pOpacitySlider->GetDest().left, mDest.top + 2.0f, pOpacitySlider->GetDest().right, pOpacitySlider->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	gfx->OutputTextSmall(gfx->GetCompatibleTarget(), L"Radius (Feet)", D2D1::RectF(pOpacitySlider->GetDest().left, pOpacitySlider->GetDest().bottom, pOpacitySlider->GetDest().right, pRadiusSlider ->GetDest().top), D2D1::ColorF(0, 0, 0), DWRITE_TEXT_ALIGNMENT_CENTER), DWRITE_PARAGRAPH_ALIGNMENT_FAR;
	if (pOpacitySlider) pOpacitySlider->Draw();
	if (pRadiusSlider) pRadiusSlider->Draw();
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), OpacityTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), RadiusTextBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
}

void AoeSizeMenu::Draw()
{
	if (CurrentSelectedType != *pSelectedType)
	{
		CurrentSelectedType = *pSelectedType;
		UpdateTransform();
	}
	D2D1::Matrix3x2F temp;
	gfx->GetCompatibleTarget()->GetTransform(&temp);
	gfx->GetCompatibleTarget()->SetTransform(mTransform);
	if (!gfx) return;
	ShowHide->Draw();

	gfx->FillRect(gfx->GetCompatibleTarget(), mDest, D2D1::ColorF(0.75f, 0.75f, 0.75f));
	gfx->DrawRect(gfx->GetCompatibleTarget(), mDest, D2D1::ColorF(0.0f, 0.0f, 0.0f), 2.0f);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), PreviewBox, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	switch (*pSelectedType)
	{
	case AoeSpritePointer::AoeTypes::Cone:
		DrawConeMenu();
		break;
	case AoeSpritePointer::AoeTypes::Cube:
		DrawCubeMenu();
		break;
	case AoeSpritePointer::AoeTypes::Cylinder:
		DrawCylinderMenu();
		break;
	case AoeSpritePointer::AoeTypes::Line:
		DrawLineMenu();
		break;
	case AoeSpritePointer::AoeTypes::Sphere:
		DrawSphereMenu();
		break;
	}

	gfx->GetCompatibleTarget()->SetTransform(temp);
	/*D2D1::Matrix3x2F temp;
	gfx->GetCompatibleTarget()->GetTransform(&temp);
	if (IsHidden()) gfx->GetCompatibleTarget()->SetTransform(mTransform);
	if (!gfx) return;
	ShowHide->Draw();

	gfx->FillRect(gfx->GetCompatibleTarget(), mDest, D2D1::ColorF(0.75f, 0.75f, 0.75f));
	std::wostringstream out;
	out.precision(4);
	out << std::fixed << pSliderBar->GetSize();
	gfx->OutputText(gfx->GetCompatibleTarget(), out.str().c_str(), mTextDest);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), mTextDest, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->DrawRoundedRect(gfx->GetCompatibleTarget(), mPreviewDest, D2D1::ColorF(0.0f, 0.0f, 0.0f), 5.0f, 5.0f, 2.0f);
	gfx->FillCircle(gfx->GetCompatibleTarget(), mPreviewCenter, fPreviewCirlceRadius);
	if (pSliderBar) pSliderBar->Draw();
	for (auto& child : vpChild)
	{
		if (!child) continue;
		child->Draw();
	}

	gfx->DrawRect(gfx->GetCompatibleTarget(), mDest, D2D1::ColorF(0.0f, 0.0f, 0.0f), 2.0f);
	gfx->GetCompatibleTarget()->SetTransform(temp);*/
}

void ThicknessMenu::UpdateSlider()
{
	pSliderBar->UpdateSliderPosition(*pMouseCoordinates);
	fPreviewCirlceRadius = pSliderBar->GetSize() * 0.5f;
}

void ThicknessMenu::JumpPosition()
{
	if (pMouseCoordinates->x > pSliderBar->GetSliderPosition().right)
	{
		pSliderBar->ShiftSlider(10.0f);
		pSliderBar->UpdateSize();
	}
	else if (pMouseCoordinates->x < pSliderBar->GetSliderPosition().left)
	{
		pSliderBar->ShiftSlider(-10.0f);
		pSliderBar->UpdateSize();
	}
}

void AoeSizeMenu::UpdateSlider()
{
	if (pOpacitySlider->IsSelected())
	{		
		pOpacitySlider->UpdateSliderPosition(mInvTransform.TransformPoint(*pMouseCoordinates));
	}
	if (pLengthSlider->IsSelected())
	{
		pLengthSlider->UpdateSliderPosition(mInvTransform.TransformPoint(*pMouseCoordinates));
	}
	if (pWidthSlider->IsSelected())
	{
		pWidthSlider->UpdateSliderPosition(mInvTransform.TransformPoint(*pMouseCoordinates));
	}
	if (pRadiusSlider->IsSelected())
	{
		pRadiusSlider->UpdateSliderPosition(mInvTransform.TransformPoint(*pMouseCoordinates));
	}
}

void AoeSizeMenu::JumpPosition()
{

}