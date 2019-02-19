#include "Pieces.h"

std::queue<std::wstring> Pieces::FillPiece(char* const Buffer, char*& pos)
{
	std::queue<std::wstring> filepaths;
	while (pos < Buffer + strlen(Buffer))
	{
		std::string line = GetLine(pos ? pos : Buffer, pos);
		std::string front, back;
		if (!SplitString(line, front, back, '=')) continue;

		if (!_stricmp(front.c_str(), "Start"))
		{
			if (GetType().size())
			{
				pos -= line.size() + 1;
				break;
			}
			SetType(back);
		}
		else if (!_stricmp(front.c_str(), "Name"))
		{
			SetName(back);
		}
		else if (!_stricmp(front.c_str(), "Icon"))
		{
			SetIconPath(back);
		}
		else if (!_stricmp(front.c_str(), "Sprite"))
		{
			SetSpritePath(back);
		}
		else if (!_stricmp(front.c_str(), "SubMenu"))
		{
			SetSubMenu(back);
		}
		else if (!_stricmp(front.c_str(), "Size"))
		{
			SetSize(back);
		}
		else if (!_stricmp(front.c_str(), "DefaultInitOrder"))
		{
			if (back[0] == 'T' || back[0] == 't')
				SetDefault();
			else
				UnsetDefault();
		}
		else if (!_stricmp(front.c_str(), "KeepAspect"))
		{
			if (back[0] == 'T' || back[0] == 't')
				SetKeepAspectSprite();
			else
				UnsetKeepAspectSprite();
		}
		else if (!_stricmp(front.c_str(), "KeepIconAspect"))
		{
			if (back[0] == 'T' || back[0] == 't')
				SetKeepAspectIcon();
			else
				UnsetKeepAspectIcon();
		}
		else if (!_stricmp(front.c_str(), "R"))
		{
			SetBackgroundR(static_cast<float>(atof(back.c_str()) / 255.0f));
			if (GetBackgroundColor().r)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_stricmp(front.c_str(), "G"))
		{
			SetBackgroundG(static_cast<float>(atof(back.c_str()) / 255.0f));
			if (GetBackgroundColor().g)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_stricmp(front.c_str(), "B"))
		{
			SetBackgroundB(static_cast<float>(atof(back.c_str()) / 255.0f));
			if (GetBackgroundColor().b)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_stricmp(front.c_str(), "A"))
		{
			SetBackgroundA(static_cast<float>(atof(back.c_str())));
		}
		else if (!_stricmp(front.c_str(), "Load") || !_stricmp(front.c_str(), "Exec"))
		{
			std::wstring tfile(back.begin(), back.end());
			filepaths.push(tfile);
		}
	}
	return filepaths;
}

std::queue<std::wstring> PiecesW::FillPiece(wchar_t* const Buffer, wchar_t*& pos)
{
	std::queue<std::wstring> filepaths;
	while (pos <= Buffer + lstrlenW(Buffer))
	{
		std::wstring line = GetLineW(pos ? pos : Buffer, pos);
		std::wstring front, back;
		if (!SplitString(line, front, back, L'=')) continue;

		if (!_wcsicmp(front.c_str(), L"Start"))
		{
			if (GetType().size())
			{
				pos -= (line.size() + 2);
				break;
			}
			SetType(back);
		}
		else if (!_wcsicmp(front.c_str(), L"Name"))
		{
			SetName(back);
		}
		else if (!_wcsicmp(front.c_str(), L"Icon"))
		{
			SetIconPath(back);
		}
		else if (!_wcsicmp(front.c_str(), L"Sprite"))
		{
			SetSpritePath(back);
		}
		else if (!_wcsicmp(front.c_str(), L"SubMenu"))
		{
			SetSubMenu(back);
		}
		else if (!_wcsicmp(front.c_str(), L"Size"))
		{
			SetSize(back);
		}
		else if (!_wcsicmp(front.c_str(), L"DefaultInitOrder"))
		{
			if (back[0] == L'T' || back[0] == L't')
				SetDefault();
			else
				UnsetDefault();
		}
		else if (!_wcsicmp(front.c_str(), L"KeepAspect"))
		{
			if (back[0] == L'T' || back[0] == L't')
				SetKeepAspectSprite();
			else
				UnsetKeepAspectSprite();
		}
		else if (!_wcsicmp(front.c_str(), L"KeepIconAspect"))
		{
			if (back[0] == L'T' || back[0] == L't')
				SetKeepAspectIcon();
			else
				UnsetKeepAspectIcon();
		}
		else if (!_wcsicmp(front.c_str(), L"R"))
		{
			SetBackgroundR(static_cast<float>(_wtof(back.c_str()) / 255.0f));
			if (GetBackgroundColor().r)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_wcsicmp(front.c_str(), L"G"))
		{
			SetBackgroundG(static_cast<float>(_wtof(back.c_str()) / 255.0f));
			if (GetBackgroundColor().g)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_wcsicmp(front.c_str(), L"B"))
		{
			SetBackgroundB(static_cast<float>(_wtof(back.c_str()) / 255.0f));
			if (GetBackgroundColor().b)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_wcsicmp(front.c_str(), L"A"))
		{
			SetBackgroundA(static_cast<float>(_wtof(back.c_str())));
		}
		else if (!_wcsicmp(front.c_str(), L"Load") || !_wcsicmp(front.c_str(), L"Exec"))
		{
			filepaths.push(back);
		}
	}
	return filepaths;
}

std::queue<std::wstring> PiecesW::FillPiece(std::wstring& Buffer, wchar_t*& pos)
{
	std::queue<std::wstring> filepaths;
	while (pos < &Buffer.at(Buffer.size() - 1))
	{
		std::wstring line = GetLineW(pos ? pos : &Buffer.at(0), pos);
		std::wstring front, back;
		if (!SplitString(line, front, back, L'=')) continue;

		if (!_wcsicmp(front.c_str(), L"Start"))
		{
			if (GetType().size())
			{
				break;
			}
			SetType(back);
		}
		else if (!_wcsicmp(front.c_str(), L"Name"))
		{
			SetName(back);
		}
		else if (!_wcsicmp(front.c_str(), L"Icon"))
		{
			SetIconPath(back);
		}
		else if (!_wcsicmp(front.c_str(), L"Sprite"))
		{
			SetSpritePath(back);
		}
		else if (!_wcsicmp(front.c_str(), L"SubMenu"))
		{
			SetSubMenu(back);
		}
		else if (!_wcsicmp(front.c_str(), L"Size"))
		{
			SetSize(back);
		}
		else if (!_wcsicmp(front.c_str(), L"DefaultInitOrder"))
		{
			if (back[0] == L'T' || back[0] == L't')
				SetDefault();
			else
				UnsetDefault();
		}
		else if (!_wcsicmp(front.c_str(), L"KeepAspect"))
		{
			if (back[0] == L'T' || back[0] == L't')
				SetKeepAspectSprite();
			else
				UnsetKeepAspectSprite();
		}
		else if (!_wcsicmp(front.c_str(), L"KeepIconAspect"))
		{
			if (back[0] == L'T' || back[0] == L't')
				SetKeepAspectIcon();
			else
				UnsetKeepAspectIcon();
		}
		else if (!_wcsicmp(front.c_str(), L"R"))
		{
			SetBackgroundR(static_cast<float>(_wtof(back.c_str())));
			if (GetBackgroundColor().r)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_wcsicmp(front.c_str(), L"G"))
		{
			SetBackgroundG(static_cast<float>(_wtof(back.c_str())));
			if (GetBackgroundColor().g)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_wcsicmp(front.c_str(), L"B"))
		{
			SetBackgroundB(static_cast<float>(_wtof(back.c_str())));
			if (GetBackgroundColor().b)
			{
				if (!GetBackgroundColor().a)
					SetBackgroundA(1.0f);
			}
		}
		else if (!_wcsicmp(front.c_str(), L"A"))
		{
			SetBackgroundA(static_cast<float>(_wtof(back.c_str())));
		}
		else if (!_wcsicmp(front.c_str(), L"Load") || !_wcsicmp(front.c_str(), L"Exec"))
		{
			filepaths.push(back);
		}
	}
	return filepaths;
}

void PiecesW::Convert(Pieces Piece)
{
	std::string t = Piece.GetType();
	SetType(std::wstring(t.begin(), t.end()));
	t = Piece.GetName();
	SetName(std::wstring(t.begin(), t.end()));
	t = Piece.GetIconPath();
	SetIconPath(std::wstring(t.begin(), t.end()));
	t = Piece.GetSpritePath();
	SetSpritePath(std::wstring(t.begin(), t.end()));
	t = Piece.GetSubMenu();
	SetSubMenu(std::wstring(t.begin(), t.end()));
	Piece.IsDefault() ? SetDefault() : UnsetDefault();
	Piece.KeepAspectSprite() ? SetKeepAspectSprite() : UnsetKeepAspectSprite();
	Piece.KeepAspectIcon() ? SetKeepAspectIcon() : UnsetKeepAspectIcon();
	SetBackgroundColor(Piece.GetBackgroundColor());
}

const CreatureSize PiecesW::StringToSize(const std::wstring wsize)
{
	if (!_wcsicmp(wsize.c_str(), L"Fine"))
		return CreatureSize::Fine;
	if (!_wcsicmp(wsize.c_str(), L"Diminutive"))
		return CreatureSize::Diminutive;
	if (!_wcsicmp(wsize.c_str(), L"Tiny"))
		return CreatureSize::Tiny;
	if (!_wcsicmp(wsize.c_str(), L"Small"))
		return CreatureSize::Small;
	if (!_wcsicmp(wsize.c_str(), L"Medium"))
		return CreatureSize::Medium;
	if (!_wcsicmp(wsize.c_str(), L"Large"))
		return CreatureSize::Large;
	if (!_wcsicmp(wsize.c_str(), L"Huge"))
		return CreatureSize::Huge;
	if (!_wcsicmp(wsize.c_str(), L"Gargantuan"))
		return CreatureSize::Gargantuan;
	if (!_wcsicmp(wsize.c_str(), L"Colossal"))
		return CreatureSize::Colossal;
	return CreatureSize::Medium;
}

const CreatureSize PiecesW::StringToSize(const std::string ssize)
{
	if (!_stricmp(ssize.c_str(), "Fine"))
		return CreatureSize::Fine;
	if (!_stricmp(ssize.c_str(), "Diminutive"))
		return CreatureSize::Diminutive;
	if (!_stricmp(ssize.c_str(), "Tiny"))
		return CreatureSize::Tiny;
	if (!_stricmp(ssize.c_str(), "Small"))
		return CreatureSize::Small;
	if (!_stricmp(ssize.c_str(), "Medium"))
		return CreatureSize::Medium;
	if (!_stricmp(ssize.c_str(), "Large"))
		return CreatureSize::Large;
	if (!_stricmp(ssize.c_str(), "Huge"))
		return CreatureSize::Huge;
	if (!_stricmp(ssize.c_str(), "Gargantuan"))
		return CreatureSize::Gargantuan;
	if (!_stricmp(ssize.c_str(), "Colossal"))
		return CreatureSize::Colossal;
	return CreatureSize::Medium;
}

const CreatureSize Pieces::StringToSize(const std::wstring wsize)
{
	if (!_wcsicmp(wsize.c_str(), L"Fine"))
		return CreatureSize::Fine;
	if (!_wcsicmp(wsize.c_str(), L"Diminutive"))
		return CreatureSize::Diminutive;
	if (!_wcsicmp(wsize.c_str(), L"Tiny"))
		return CreatureSize::Tiny;
	if (!_wcsicmp(wsize.c_str(), L"Small"))
		return CreatureSize::Small;
	if (!_wcsicmp(wsize.c_str(), L"Medium"))
		return CreatureSize::Medium;
	if (!_wcsicmp(wsize.c_str(), L"Large"))
		return CreatureSize::Large;
	if (!_wcsicmp(wsize.c_str(), L"Huge"))
		return CreatureSize::Huge;
	if (!_wcsicmp(wsize.c_str(), L"Gargantuan"))
		return CreatureSize::Gargantuan;
	if (!_wcsicmp(wsize.c_str(), L"Colossal"))
		return CreatureSize::Colossal;
	return CreatureSize::Medium;
}

const CreatureSize Pieces::StringToSize(const std::string ssize)
{
	if (!_stricmp(ssize.c_str(), "Fine"))
		return CreatureSize::Fine;
	if (!_stricmp(ssize.c_str(), "Diminutive"))
		return CreatureSize::Diminutive;
	if (!_stricmp(ssize.c_str(), "Tiny"))
		return CreatureSize::Tiny;
	if (!_stricmp(ssize.c_str(), "Small"))
		return CreatureSize::Small;
	if (!_stricmp(ssize.c_str(), "Medium"))
		return CreatureSize::Medium;
	if (!_stricmp(ssize.c_str(), "Large"))
		return CreatureSize::Large;
	if (!_stricmp(ssize.c_str(), "Huge"))
		return CreatureSize::Huge;
	if (!_stricmp(ssize.c_str(), "Gargantuan"))
		return CreatureSize::Gargantuan;
	if (!_stricmp(ssize.c_str(), "Colossal"))
		return CreatureSize::Colossal;
	return CreatureSize::Medium;
}

void PiecesW::LoadSprite()
{
	if (GetSpritePath().empty()) return;
	pSprite = new Sprite(GetSpritePath().c_str(), gfx, pTimer);
}

void PiecesW::LoadPortrait()
{
	if (GetIconPath().empty()) return;
	pPortrait = new Sprite(GetIconPath().c_str(), gfx, pTimer);
	if (!pPortrait->IsSuccess())
		SafeDelete(&pPortrait);
}

void Pieces::LoadSprite()
{
	if (GetSpritePath().empty()) return;
	std::wstring SpritePathW(sSpritePath.begin(), sSpritePath.end());
	pSprite = new Sprite(SpritePathW.c_str(), gfx, pTimer);
	if (!pSprite->IsSuccess())
		SafeDelete(&pSprite);
}

void Pieces::LoadPortrait()
{
	if (GetIconPath().empty()) return;
	std::wstring IconPathW(sIconPath.begin(), sIconPath.end());
	pPortrait = new Sprite(IconPathW.c_str(), gfx, pTimer);
	if (!pPortrait->IsSuccess())
		SafeDelete(&pPortrait);	
}

void SpritePointer::DrawSprite(Graphics* const gfx, bool back)
{
	if (!GetSprite()) return;
	if (mLocation.mResizedDestSprite.right - mLocation.mResizedDestSprite.left > 0)
	{
		if (back) gfx->DrawBitmap(gfx->GetCompatibleTarget(), GetSprite()->GetBitmap(), mLocation.mResizedDestSprite, fOpacity, GetSprite()->GetFrame());
		else gfx->DrawBitmap(gfx->GetRenderTarget(), GetSprite()->GetBitmap(), mLocation.mResizedDestSprite, fOpacity, GetSprite()->GetFrame());
	}
	else
	{
		if (back) gfx->DrawBitmap(gfx->GetCompatibleTarget(), GetSprite()->GetBitmap(), mLocation.mDestSprite, fOpacity, GetSprite()->GetFrame());
		else gfx->DrawBitmap(gfx->GetRenderTarget(), GetSprite()->GetBitmap(), mLocation.mDestSprite, fOpacity, GetSprite()->GetFrame());
	}
	GetSprite()->NextFrame();
}

void SpritePointer::DrawPortrait(Graphics* const gfx, bool back)
{
	if (!GetPortrait()) return;
	if (back) gfx->DrawBitmap(gfx->GetCompatibleTarget(), GetPortrait()->GetBitmap(), mLocation.mDestSprite, fOpacity, GetPortrait()->GetFrame());
	else gfx->DrawBitmap(gfx->GetRenderTarget(), GetPortrait()->GetBitmap(), mLocation.mDestSprite, fOpacity, GetPortrait()->GetFrame());
	GetPortrait()->NextFrame();
}

void SpritePointer::BuildResizedDestSprite()
{
	if (mSize == CreatureSize::Medium)
		mLocation.mResizedDestSprite = mLocation.mDestSprite;
	else
	{
		if (mSize < CreatureSize::Medium)
		{
			float sizemod = static_cast<float>(1.0f / pow(2, static_cast<unsigned long>(CreatureSize::Medium) - static_cast<unsigned long>(mSize)));
			mLocation.mResizedDestSprite = D2D1::RectF(mLocation.mDestSprite.left, mLocation.mDestSprite.top,
				mLocation.mDestSprite.left + (mLocation.mDestSprite.right - mLocation.mDestSprite.left) * sizemod,
				mLocation.mDestSprite.top + (mLocation.mDestSprite.bottom - mLocation.mDestSprite.top) * sizemod);
		}
		else
		{
			float sizemod = static_cast<float>(static_cast<unsigned long>(mSize) - static_cast<unsigned long>(CreatureSize::Medium) + 1);
			mLocation.mResizedDestSprite = D2D1::RectF(mLocation.mDestSprite.left, mLocation.mDestSprite.top,
				mLocation.mDestSprite.left + (mLocation.mDestSprite.right - mLocation.mDestSprite.left) * sizemod,
				mLocation.mDestSprite.top + (mLocation.mDestSprite.bottom - mLocation.mDestSprite.top) * sizemod);
		}
	}
}

void SpritePointer::SetDestPortrait(const D2D1_RECT_F d)
{
	if (!IsKeepAspectPortrait())
	{
		mLocation.mDestPortrait = d; 
	}
	else
	{
		mLocation.mDestPortrait = RectToAspectPortrait(d);
	}
}

void SpritePointer::SetDestSprite(const D2D1_RECT_F d, bool ApplyRebuild)
{
	if (!IsKeepAspectSprite())
	{
		mLocation.mDestSprite = d;
		if (ApplyRebuild) BuildResizedDestSprite();
	}
	else
	{
		mLocation.mDestSprite = RectToAspectSprite(d);
		if (ApplyRebuild) BuildResizedDestSprite();
	}
}

const D2D1_RECT_F SpritePointer::RectToAspectSprite(const D2D1_RECT_F d)
{
	float fNumerator = fabs(GetSpriteFrameSize().width);
	float fDenominator = fabs(GetSpriteFrameSize().height);
	if (fNumerator == fDenominator) return d;
	if (!fDenominator) return d;

	if (fNumerator > fDenominator)
	{
		float fRatio = fDenominator / fNumerator;

		D2D1_RECT_F temp;
		temp.left = d.left;
		temp.top = d.top;
		temp.right = d.right;
		temp.bottom = temp.top + (d.bottom - d.top) * fRatio;

		float fdist = d.bottom - temp.bottom;
		temp.top += fdist;
		temp.bottom += fdist;

		return temp;
	}
	else
	{
		float fRatio = fNumerator / fDenominator;

		D2D1_RECT_F temp;
		temp.left = d.left;
		temp.top = d.top;
		temp.right = temp.left + (d.right - d.left) * fRatio;
		temp.bottom = d.bottom;

		float fdist = d.right - temp.right;
		fdist *= 0.5f;
		temp.left += fdist;
		temp.right += fdist;
		return temp;
	}
	return d;
}

const D2D1_RECT_F SpritePointer::RectToAspectPortrait(const D2D1_RECT_F d)
{
	float fNumerator = fabs(GetSpriteFrameSize().width);
	float fDenominator = fabs(GetSpriteFrameSize().height);
	if (fNumerator == fDenominator) return d;
	if (!fDenominator) return d;

	if (fNumerator > fDenominator)
	{
		float fRatio = fDenominator / fNumerator;

		D2D1_RECT_F temp;
		temp.left = d.left;
		temp.top = d.top;
		temp.right = d.right;
		temp.bottom = temp.top + (d.bottom - d.top) * fRatio;

		float fdist = d.bottom - temp.bottom;
		temp.top += fdist;
		temp.bottom += fdist;
		return temp;
	}
	else
	{
		float fRatio = fNumerator / fDenominator;

		D2D1_RECT_F temp;
		temp.left = d.left;
		temp.top = d.top;
		temp.right = temp.left + (d.right - d.left) * fRatio;
		temp.bottom = d.bottom;

		float fdist = d.right - temp.right;
		fdist *= 0.5f;
		temp.left += fdist;
		temp.right += fdist;
		return temp;
	}
	return d;
}

void SpritePointer::MoveDestSprite(const D2D1_SIZE_F changes)
{
	mLocation.mDestSprite.left += changes.width;
	mLocation.mDestSprite.right += changes.width;
	mLocation.mDestSprite.top += changes.height;
	mLocation.mDestSprite.bottom += changes.height;
}

void SpritePointer::MovePortraitSprite(const D2D1_SIZE_F changes)
{
	mLocation.mDestPortrait.left += changes.width;
	mLocation.mDestPortrait.right += changes.width;
	mLocation.mDestPortrait.top += changes.height;
	mLocation.mDestPortrait.bottom += changes.height;
}

void SpritePointer::MoveResizedDestSprite(const D2D1_SIZE_F changes)
{
	mLocation.mResizedDestSprite.left += changes.width;
	mLocation.mResizedDestSprite.right += changes.width;
	mLocation.mResizedDestSprite.top += changes.height;
	mLocation.mResizedDestSprite.bottom += changes.height;
}

void SpritePointer::SetDestSpritePosition(const D2D1_POINT_2F point)
{
	mLocation.mDestSprite.left = point.x;
	mLocation.mDestSprite.top = point.y;
	mLocation.mDestSprite.right = mLocation.mDestSprite.left + GetSpriteFrameSize().width;
	mLocation.mDestSprite.bottom = mLocation.mDestSprite.top + GetSpriteFrameSize().height;
}

void SpritePointer::SetDestResizedSpritePosition(const D2D1_POINT_2F point)
{
	D2D1_SIZE_F size = D2D1::SizeF(mLocation.mResizedDestSprite.right - mLocation.mResizedDestSprite.left, mLocation.mResizedDestSprite.bottom - mLocation.mResizedDestSprite.top);
	mLocation.mResizedDestSprite.left = point.x;
	mLocation.mResizedDestSprite.top = point.y;
	mLocation.mResizedDestSprite.right = mLocation.mResizedDestSprite.left + size.width;
	mLocation.mResizedDestSprite.bottom = mLocation.mResizedDestSprite.top + size.height;
}

void SpritePointer::SetPortraitPosition(const D2D1_POINT_2F point)
{
	mLocation.mDestPortrait.left = point.x;
	mLocation.mDestPortrait.top = point.y;
	mLocation.mDestPortrait.right = mLocation.mDestPortrait.left + GetPortraitFrameSize().width;
	mLocation.mDestPortrait.bottom = mLocation.mDestPortrait.top + GetPortraitFrameSize().height;
}