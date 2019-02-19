#include "SizeMenu.h"

SizeMenu::SizeMenu(Graphics* const graphics, const D2D1_RECT_F dest, D2D1_POINT_2F* const p) : gfx(graphics), pMouseCoordinates(p)
{
	mSize = D2D1::SizeF(fabs(dest.right - dest.left), fabs(dest.bottom - dest.top));
	SetDest(dest);
	ConfigureShowHide();
	UpdateTransform();
}

SizeMenu::~SizeMenu()
{
	SafeDelete(&ShowHide);
	while (vpChild.size())
	{
		SafeDelete(&vpChild.back());
		vpChild.pop_back();
	}
}

void SizeMenu::ConfigureShowHide()
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

const bool SizeMenu::IsInRealRect()
{
	D2D1_POINT_2F point = mInvTransform.TransformPoint(*pMouseCoordinates);
	return (point.x > mRealDest.left && point.x < mRealDest.right && point.y > mRealDest.top && point.y < mRealDest.bottom);
}

const bool SizeMenu::PointInRect()
{
	return PointInRect(*pMouseCoordinates);
}

const bool SizeMenu::PointInRect(const D2D1_POINT_2F p)
{
	D2D1_POINT_2F point = mInvTransform.TransformPoint(p);
	return (point.x > mDest.left && point.x < mDest.right && point.y > mDest.top && point.y < mDest.bottom);
}

void SizeMenu::SetHidden()
{
	bHidden = true;
	UpdateTransform();
}

void SizeMenu::UnsetHidden()
{
	bHidden = false;
	UpdateTransform();
}

void SizeMenu::ToggleHidden()
{
	bHidden ^= true;
	UpdateTransform();
}

void SizeMenu::UpdateTransform()
{
	if (bHidden)
	{
		mTransform = D2D1::Matrix3x2F::Translation(D2D1::SizeF(0.0f, GetSize().height-25));
	}
	else
	{
		mTransform = D2D1::Matrix3x2F::Identity();
	}
	mInvTransform = mTransform;
	mInvTransform.Invert();
}

void SizeMenu::SetDest(const D2D1_RECT_F dest)
{
	mDest = dest; 
	SetEllipse(D2D1::Ellipse(D2D1::Point2F(mDest.left + (dest.right - dest.left) * 0.5f, dest.top), (dest.right - dest.left) * 0.5f, 15.0f));
	mRealDest = D2D1::RectF(mDest.left, mDest.top - mEllipse.radiusY, mDest.right, mDest.bottom);
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
}

void SizeMenu::BuildMenuFeet()
{//this will be for wall sizes, spell effect areas, and the like

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