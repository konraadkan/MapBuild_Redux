#include "Pieces.h"
#include<mutex>

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
	SetSize(Piece.GetSize());
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
	pSprite = new Sprite(GetSpritePath().c_str(), gfx, pTimer, pGridSize);
}

void PiecesW::LoadSpriteM(std::atomic<unsigned int>& numberfinished)
{
	std::mutex m;
	if (GetSpritePath().empty())
	{
		m.lock();
		numberfinished++;
		m.unlock();
		return;
	}
	pSprite = new Sprite(GetSpritePath().c_str(), gfx, pTimer, pGridSize);
	
	m.lock();
	numberfinished++;
	m.unlock();
}

void PiecesW::LoadPortrait()
{
	if (GetIconPath().empty()) return;
	pPortrait = new Sprite(GetIconPath().c_str(), gfx, pTimer, pGridSize);
	if (!pPortrait->IsSuccess())
		SafeDelete(&pPortrait);
}

void PiecesW::LoadPortraitM(std::atomic<unsigned int>& numberfinished)
{
	std::mutex m;
	if (GetIconPath().empty())
	{
		m.lock();
		numberfinished++;
		m.unlock();
		return;
	}
	pPortrait = new Sprite(GetIconPath().c_str(), gfx, pTimer, pGridSize);
	if (!pPortrait->IsSuccess())
		SafeDelete(&pPortrait);

	m.lock();
	numberfinished++;
	m.unlock();
}

void Pieces::LoadSprite()
{
	if (GetSpritePath().empty()) return;
	std::wstring SpritePathW(sSpritePath.begin(), sSpritePath.end());
	pSprite = new Sprite(SpritePathW.c_str(), gfx, pTimer, pGridSize);
	if (!pSprite->IsSuccess())
		SafeDelete(&pSprite);
}

void Pieces::LoadSpriteM(std::atomic<unsigned int>& numberfinished)
{
	std::mutex m;
	if (GetSpritePath().empty())
	{
		m.lock();
		numberfinished++;
		m.unlock();
		return;
	}
	std::wstring SpritePathW(sSpritePath.begin(), sSpritePath.end());
	pSprite = new Sprite(SpritePathW.c_str(), gfx, pTimer, pGridSize);
	if (!pSprite->IsSuccess())
		SafeDelete(&pSprite);
	m.lock();
	numberfinished++;
	m.unlock();
}

void Pieces::LoadPortraitM(std::atomic<unsigned int>& numberfinished)
{
	std::mutex m;
	if (GetIconPath().empty())
	{
		m.lock();
		numberfinished++;
		m.unlock();
		return;
	}
	std::wstring IconPathW(sIconPath.begin(), sIconPath.end());
	pPortrait = new Sprite(IconPathW.c_str(), gfx, pTimer, pGridSize);
	if (!pPortrait->IsSuccess())
		SafeDelete(&pPortrait);

	m.lock();
	numberfinished++;
	m.unlock();
}

void Pieces::LoadPortrait()
{
	if (GetIconPath().empty()) return;
	std::wstring IconPathW(sIconPath.begin(), sIconPath.end());
	pPortrait = new Sprite(IconPathW.c_str(), gfx, pTimer, pGridSize);
	if (!pPortrait->IsSuccess())
		SafeDelete(&pPortrait);	
}

void SpritePointer::DrawSprite(Graphics* const gfx, bool back)
{
	if (!GetSprite()) return;
	if (back) gfx->DrawBitmap(gfx->GetCompatibleTarget(), GetSprite()->GetBitmap(), mLocation.mDestSprite, fOpacity, GetSprite()->GetFrame());
	else gfx->DrawBitmap(gfx->GetRenderTarget(), GetSprite()->GetBitmap(), mLocation.mDestSprite, fOpacity, GetSprite()->GetFrame());
	GetSprite()->NextFrame();

	if (wTag[0] != L'\0')
	{
		if (mSize < CreatureSize::Medium)
		{
			if (back) gfx->OutputTextSmall(gfx->GetCompatibleTarget(), wTag, GetDestTag());
			else gfx->OutputTextSmall(gfx->GetRenderTarget(), wTag, GetDestTag());
		}
		else
		{
			if (back) gfx->OutputText(gfx->GetCompatibleTarget(), wTag, GetDestTag());
			else gfx->OutputText(gfx->GetRenderTarget(), wTag, GetDestTag());
		}
	}

	for (auto& child : vSpriteChild)
	{
		if (child) child->DrawSprite(gfx, back);
	}
}

void SpritePointer::DrawPortrait(Graphics* const gfx, bool back)
{
	if (!GetPortrait()) return;
	if (back) gfx->DrawBitmap(gfx->GetCompatibleTarget(), GetPortrait()->GetBitmap(), mLocation.mDestSprite, fOpacity, GetPortrait()->GetFrame());
	else gfx->DrawBitmap(gfx->GetRenderTarget(), GetPortrait()->GetBitmap(), mLocation.mDestSprite, fOpacity, GetPortrait()->GetFrame());
	GetPortrait()->NextFrame();

	for (auto& child : vPortraitChild)
	{
		if (child) child->DrawPortrait(gfx, back);
	}
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
		else if (mSize < CreatureSize::OneX)
		{
			float sizemod = static_cast<float>(static_cast<unsigned long>(mSize) - static_cast<unsigned long>(CreatureSize::Medium) + 1);
			mLocation.mResizedDestSprite = D2D1::RectF(mLocation.mDestSprite.left, mLocation.mDestSprite.top,
				mLocation.mDestSprite.left + (mLocation.mDestSprite.right - mLocation.mDestSprite.left) * sizemod,
				mLocation.mDestSprite.top + (mLocation.mDestSprite.bottom - mLocation.mDestSprite.top) * sizemod);
		}
		else
		{
			float sizemod = static_cast<float>(mSize) - static_cast<float>(CreatureSize::Colossal);
			mLocation.mResizedDestSprite = D2D1::RectF(mLocation.mDestSprite.left, mLocation.mDestSprite.top, mLocation.mDestSprite.left + GetSpriteFrameSize().width * sizemod, mLocation.mDestSprite.top + GetSpriteFrameSize().height * sizemod);
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
	mLocation.mDestTag = CalcDestTag();
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
	mLocation.mDestTag = CalcDestTag();
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
	mLocation.mDestTag = CalcDestTag();
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

void SpritePointer::AddSpriteChild(PiecesW* const piece)
{
	vSpriteChild.push_back(new SpritePointer(piece, mLocation, pGridSquareSize, bKeepAspectRatioSprite, bKeepAspectRatioPortrait));
	vSpriteChild.back()->SetCreatureSize(GetCreatureSize());
	vSpriteChild.back()->SetOpacity(0.60f);
}

void SpritePointer::AddPortraitChild(PiecesW* const piece)
{
	vPortraitChild.push_back(new SpritePointer(piece, mLocation, pGridSquareSize, bKeepAspectRatioSprite, bKeepAspectRatioPortrait));
}

void SpritePointer::RemoveChildren()
{
	for (auto& sprite : vSpriteChild)
	{
		SafeDelete(&sprite);
	}
	for (auto& sprite : vPortraitChild)
	{
		SafeDelete(&sprite);
	}
	vSpriteChild = std::vector<SpritePointer*>();
	vPortraitChild = std::vector<SpritePointer*>();
}

void SpritePointer::AddTag(wchar_t w)
{
	if (wTag[0] == w) wTag[0] = L'\0';
	else wTag[0] = w;
}

const char* SpritePointer::GetSaveInformation()
{
	return CreateSaveInformation();
}

const char* SpritePointer::CreateSaveInformation()
{
	unsigned int uBufferSize = CalcSaveSize();										//get size of buffer
	char* Buffer = new char[uBufferSize];											//allocate buffer
	size_t pos = 0;
	memcpy(Buffer, &uBufferSize, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);

	/****Child Buffer Data***/
	unsigned int uLen = static_cast<unsigned int>(vSpriteChild.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	for (auto& child : vSpriteChild)
	{
		unsigned int ilen = child->CalcSaveSize();
		const char* temp = child->GetSaveInformation();
		memcpy(Buffer + pos, temp, ilen);
		pos += ilen;
		delete[] temp;
	}
	uLen = static_cast<unsigned int>(vPortraitChild.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	for (auto& child : vPortraitChild)
	{
		unsigned int ilen = child->CalcSaveSize();
		const char* temp = child->GetSaveInformation();
		memcpy(Buffer + pos, temp, ilen);
		pos += ilen;
		delete[] temp;
	}
	wchar_t t = wTag[0];
	memcpy(Buffer + pos, &t, sizeof(t));
	pos += sizeof(t);
	/****Begin Location Data*******/
	memcpy(Buffer + pos, &mLocation.mDestSprite.left, sizeof(mLocation.mDestSprite.left));
	pos += sizeof(mLocation.mDestSprite.left);
	memcpy(Buffer + pos, &mLocation.mDestSprite.top, sizeof(mLocation.mDestSprite.top));
	pos += sizeof(mLocation.mDestSprite.top);
	memcpy(Buffer + pos, &mLocation.mDestSprite.right, sizeof(mLocation.mDestSprite.right));
	pos += sizeof(mLocation.mDestSprite.right);
	memcpy(Buffer + pos, &mLocation.mDestSprite.bottom, sizeof(mLocation.mDestSprite.bottom));
	pos += sizeof(mLocation.mDestSprite.bottom);
	memcpy(Buffer + pos, &mLocation.mDestTag.left, sizeof(mLocation.mDestTag.left));
	pos += sizeof(mLocation.mDestTag.left);
	memcpy(Buffer + pos, &mLocation.mDestTag.top, sizeof(mLocation.mDestTag.top));
	pos += sizeof(mLocation.mDestTag.top);
	memcpy(Buffer + pos, &mLocation.mDestTag.right, sizeof(mLocation.mDestTag.right));
	pos += sizeof(mLocation.mDestTag.right);
	memcpy(Buffer + pos, &mLocation.mDestTag.bottom, sizeof(mLocation.mDestTag.bottom));
	pos += sizeof(mLocation.mDestTag.bottom);
	memcpy(Buffer + pos, &mLocation.mDestPortrait.left, sizeof(mLocation.mDestPortrait.left));
	pos += sizeof(mLocation.mDestPortrait.left);
	memcpy(Buffer + pos, &mLocation.mDestPortrait.top, sizeof(mLocation.mDestPortrait.top));
	pos += sizeof(mLocation.mDestPortrait.top);
	memcpy(Buffer + pos, &mLocation.mDestPortrait.right, sizeof(mLocation.mDestPortrait.right));
	pos += sizeof(mLocation.mDestPortrait.right);
	memcpy(Buffer + pos, &mLocation.mDestPortrait.bottom, sizeof(mLocation.mDestPortrait.bottom));
	pos += sizeof(mLocation.mDestPortrait.bottom);
	memcpy(Buffer + pos, &mLocation.mResizedDestSprite.left, sizeof(mLocation.mResizedDestSprite.left));
	pos += sizeof(mLocation.mResizedDestSprite.left);
	memcpy(Buffer + pos, &mLocation.mResizedDestSprite.top, sizeof(mLocation.mResizedDestSprite.top));
	pos += sizeof(mLocation.mResizedDestSprite.top);
	memcpy(Buffer + pos, &mLocation.mResizedDestSprite.right, sizeof(mLocation.mResizedDestSprite.right));
	pos += sizeof(mLocation.mResizedDestSprite.right);
	memcpy(Buffer + pos, &mLocation.mResizedDestSprite.bottom, sizeof(mLocation.mResizedDestSprite.bottom));
	pos += sizeof(mLocation.mResizedDestSprite.bottom);
	memcpy(Buffer + pos, &mLocation.uLayer, sizeof(mLocation.uLayer));
	pos += sizeof(mLocation.uLayer);
	memcpy(Buffer + pos, &mLocation.uRoom, sizeof(mLocation.uRoom));
	pos += sizeof(mLocation.uRoom);

	unsigned char bools = 0;
	if (bSelected) bools |= 1;
	if (bKeepAspectRatioSprite) bools |= 2;
	if (bKeepAspectRatioPortrait) bools |= 4;
	memcpy(Buffer + pos, &bools, sizeof(bools));
	pos += sizeof(bools);
	memcpy(Buffer + pos, &fOpacity, sizeof(fOpacity));
	pos += sizeof(fOpacity);

	unsigned int uCreatureSize = static_cast<unsigned int>(mSize);
	memcpy(Buffer + pos, &uCreatureSize, sizeof(uCreatureSize));
	pos += sizeof(uCreatureSize);

	unsigned int ilen = pPiece->CalcSaveSize();
	const char* tempBuffer = pPiece->GetSaveBuffer();
	memcpy(Buffer + pos, tempBuffer, ilen);
	pos += ilen;
	delete[] tempBuffer;

	return Buffer;
}

const D2D1_RECT_F SpritePointer::CalcDestTag()
{
	D2D1_RECT_F rect = D2D1::RectF();
	D2D1_SIZE_F size = D2D1::SizeF(mLocation.mDestSprite.right - mLocation.mDestSprite.left, mLocation.mDestSprite.bottom - mLocation.mDestSprite.top);
	if (mSize < CreatureSize::Medium)
	{
		rect.left = mLocation.mDestSprite.left;
		rect.top = mLocation.mDestSprite.top - pGridSquareSize->height * 0.25f;
		rect.right = mLocation.mDestSprite.left + pGridSquareSize->width * 0.25f;
		rect.bottom = mLocation.mDestSprite.top;
	}
	else
	{
		rect.left = mLocation.mDestSprite.left;
		rect.top = mLocation.mDestSprite.top;
		rect.right = rect.left + size.width * 0.25f;
		rect.bottom = rect.top + size.height * 0.25f;
	}
	return rect;
}

const bool SpritePointer::LoadSaveBuffer(const char* Buffer)
{
	if (!Buffer) return false;
	unsigned int uBufferSize = 0;
	size_t pos = 0;
	memcpy(&uBufferSize, Buffer, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	unsigned int uLen = 0;
	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	if (uLen)
	{
		for (unsigned int i = 0; i < uLen; i++)
		{
			unsigned int ilen = 0;
			memcpy(&ilen, Buffer + pos, sizeof(ilen));
			vSpriteChild.push_back(new SpritePointer(nullptr, mLocation, pGridSquareSize));
			vSpriteChild.back()->LoadSaveBuffer(Buffer + pos);
			pos += ilen;
		}
	}
	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	if (uLen)
	{
		for (unsigned int i = 0; i < uLen; i++)
		{
			unsigned int ilen = 0;
			memcpy(&ilen, Buffer + pos, sizeof(ilen));
			vPortraitChild.push_back(new SpritePointer(nullptr, mLocation, pGridSquareSize));
			vPortraitChild.back()->LoadSaveBuffer(Buffer + pos);
			pos += ilen;
		}
	}
	memcpy(&wTag, Buffer + pos, sizeof(wTag));
	pos += sizeof(wchar_t);

	memcpy(&mLocation.mDestSprite.left, Buffer + pos, sizeof(mLocation.mDestSprite.left));
	pos += sizeof(mLocation.mDestSprite.left);
	memcpy(&mLocation.mDestSprite.top, Buffer + pos, sizeof(mLocation.mDestSprite.top));
	pos += sizeof(mLocation.mDestSprite.top);
	memcpy(&mLocation.mDestSprite.right, Buffer + pos, sizeof(mLocation.mDestSprite.right));
	pos += sizeof(mLocation.mDestSprite.right);
	memcpy(&mLocation.mDestSprite.bottom, Buffer + pos, sizeof(mLocation.mDestSprite.bottom));
	pos += sizeof(mLocation.mDestSprite.bottom);
	memcpy(&mLocation.mDestTag.left, Buffer + pos, sizeof(mLocation.mDestTag.left));
	pos += sizeof(mLocation.mDestTag.left);
	memcpy(&mLocation.mDestTag.top, Buffer + pos, sizeof(mLocation.mDestTag.top));
	pos += sizeof(mLocation.mDestTag.top);
	memcpy(&mLocation.mDestTag.right, Buffer + pos, sizeof(mLocation.mDestTag.right));
	pos += sizeof(mLocation.mDestTag.right);
	memcpy(&mLocation.mDestTag.bottom, Buffer + pos, sizeof(mLocation.mDestTag.bottom));
	pos += sizeof(mLocation.mDestTag.bottom);
	memcpy(&mLocation.mDestPortrait.left, Buffer + pos, sizeof(mLocation.mDestPortrait.left));
	pos += sizeof(mLocation.mDestPortrait.left);
	memcpy(&mLocation.mDestPortrait.top, Buffer + pos, sizeof(mLocation.mDestPortrait.top));
	pos += sizeof(mLocation.mDestPortrait.top);
	memcpy(&mLocation.mDestPortrait.right, Buffer + pos, sizeof(mLocation.mDestPortrait.right));
	pos += sizeof(mLocation.mDestPortrait.right);
	memcpy(&mLocation.mDestPortrait.bottom, Buffer + pos, sizeof(mLocation.mDestPortrait.bottom));
	pos += sizeof(mLocation.mDestPortrait.bottom);
	memcpy(&mLocation.mResizedDestSprite.left, Buffer + pos, sizeof(mLocation.mResizedDestSprite.left));
	pos += sizeof(mLocation.mResizedDestSprite.left);
	memcpy(&mLocation.mResizedDestSprite.top, Buffer + pos, sizeof(mLocation.mResizedDestSprite.top));
	pos += sizeof(mLocation.mResizedDestSprite.top);
	memcpy(&mLocation.mResizedDestSprite.right, Buffer + pos, sizeof(mLocation.mResizedDestSprite.right));
	pos += sizeof(mLocation.mResizedDestSprite.right);
	memcpy(&mLocation.mResizedDestSprite.bottom, Buffer + pos, sizeof(mLocation.mResizedDestSprite.bottom));
	pos += sizeof(mLocation.mResizedDestSprite.bottom);
	memcpy(&mLocation.uLayer, Buffer + pos, sizeof(mLocation.uLayer));
	pos += sizeof(mLocation.uLayer);
	memcpy(&mLocation.uRoom, Buffer + pos, sizeof(mLocation.uRoom));
	pos += sizeof(mLocation.uRoom);

	unsigned char bools = 0;
	memcpy(&bools, Buffer + pos, sizeof(bools));
	pos += sizeof(bools);
	bSelected = (bools & 1);
	bKeepAspectRatioSprite = (bools & 2);
	bKeepAspectRatioPortrait = (bools & 4);

	memcpy(&fOpacity, Buffer + pos, sizeof(fOpacity));
	pos += sizeof(fOpacity);
	unsigned int uCreatureSize = 0;
	memcpy(&uCreatureSize, Buffer + pos, sizeof(uCreatureSize));
	pos += sizeof(uCreatureSize);
	mSize = static_cast<CreatureSize>(uCreatureSize);

	unsigned int ilen = 0;
	memcpy(&ilen, Buffer + pos, sizeof(ilen));
	SafeDeleteArray(&LoadedPiece);
	LoadedPiece = new char[ilen];
	memcpy(LoadedPiece, Buffer + pos, ilen);
	pos += ilen;
	return true;
}

const unsigned int SpritePointer::CalcSaveSize()
{
	unsigned int uBufferSize = 0;
	uBufferSize += sizeof(unsigned int);											//hold size of this buffer
	uBufferSize += sizeof(unsigned int);											//holds vSpriteChild.size()
	
	for (auto& child : vSpriteChild)
	{
		uBufferSize += child->CalcSaveSize();										//stores vSpriteChild buffer for each vSpriteChild
	}

	uBufferSize += sizeof(unsigned int);											//holds vPortraitChild.size()
	
	for (auto& child : vPortraitChild)
	{
		uBufferSize += child->CalcSaveSize();										//stores vPortraitchild buffer for each vPortraitChild
	}
	uBufferSize += sizeof(wchar_t);													//stores tag data
	/**   start calculating size of mLocation   **/
	uBufferSize += sizeof(float) * 4;												//stores DestSprite data
	uBufferSize += sizeof(float) * 4;												//stores DestTag data
	uBufferSize += sizeof(float) * 4;												//stores DestPortrait data
	uBufferSize += sizeof(float) * 4;												//stores ResizedDestSprite data
	uBufferSize += sizeof(mLocation.uLayer);										//stores Layer data
	uBufferSize += sizeof(mLocation.uRoom);											//stores room data
	uBufferSize += sizeof(unsigned char);											//unsigned char to hold booleen data
	uBufferSize += sizeof(float);													//stores opacity data
	uBufferSize += sizeof(unsigned int);											//stores creature data as an unsigned int
	uBufferSize += pPiece->CalcSaveSize();											//stores piecew value
	return uBufferSize;
}

const unsigned int PiecesW::CalcSaveSize()
{
	unsigned int uBufferSize = 0;
	uBufferSize += sizeof(unsigned int);											//holds size of this buffer
	uBufferSize += sizeof(unsigned int);											//holds creature size
	uBufferSize += sizeof(unsigned int);											//holds sType.size()
	uBufferSize += static_cast<unsigned int>(sType.size()) * sizeof(wchar_t);		//holds sType wchar_t's
	uBufferSize += sizeof(unsigned int);											//holds sName.size()
	uBufferSize += static_cast<unsigned int>(sName.size()) * sizeof(wchar_t);		//holds sName wchar_t's
	uBufferSize += sizeof(unsigned int);											//holds sIconPath.size()
	uBufferSize += static_cast<unsigned int>(sIconPath.size()) * sizeof(wchar_t);	//holsd sIconPath wchar_t's
	uBufferSize += sizeof(unsigned int);											//holds sSpritePath.size()
	uBufferSize += static_cast<unsigned int>(sSpritePath.size()) * sizeof(wchar_t); //holds sSpritePath wchar_t's
	uBufferSize += sizeof(unsigned int);											//holds sSubmenu.size()
	uBufferSize += static_cast<unsigned int>(sSubMenu.size()) * sizeof(wchar_t);	//holds sSubmenu wchar_t's
	uBufferSize += sizeof(unsigned char);											//holds bool values for bDefaultInit, bKeepAspect, bKeepIconAspect
	uBufferSize += sizeof(float) * 4;												//holds color values
	return uBufferSize;
}

const char* PiecesW::BuildSaveBuffer()
{
	unsigned int uBufferSize = CalcSaveSize();
	char* Buffer = new char[uBufferSize];
	size_t pos = 0;

	memcpy(Buffer, &uBufferSize, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);
	
	unsigned int uCreatureSize = static_cast<unsigned int>(mSize);
	memcpy(Buffer + pos, &uCreatureSize, sizeof(uCreatureSize));
	pos += sizeof(uCreatureSize);

	unsigned int uLen = static_cast<unsigned int>(sType.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);	
	if (sType.size())
	{
		memcpy(Buffer + pos, &sType.front(), sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	uLen = static_cast<unsigned int>(sName.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	if (sName.size())
	{
		memcpy(Buffer + pos, &sName.front(), sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	uLen = static_cast<unsigned int>(sIconPath.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	if (sIconPath.size())
	{
		memcpy(Buffer + pos, &sIconPath.front(), sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	uLen = static_cast<unsigned int>(sSpritePath.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	if (sSpritePath.size())
	{
		memcpy(Buffer + pos, &sSpritePath.front(), sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	uLen = static_cast<unsigned int>(sSubMenu.size());
	memcpy(Buffer + pos, &uLen, sizeof(uLen));
	pos += sizeof(uLen);
	if (sSubMenu.size())
	{
		memcpy(Buffer + pos, &sSubMenu.front(), sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	unsigned char bools = 0;
	if (bDetaultInitOrder) bools |= 1;
	if (bKeepAspect) bools |= 2;
	if (bKeepIconAspect) bools |= 4;

	memcpy(Buffer + pos, &bools, sizeof(bools));
	pos += sizeof(bools);
	
	memcpy(Buffer + pos, &BackgroundColor.r, sizeof(BackgroundColor.r));
	pos += sizeof(BackgroundColor.r);
	memcpy(Buffer + pos, &BackgroundColor.g, sizeof(BackgroundColor.g));
	pos += sizeof(BackgroundColor.g);
	memcpy(Buffer + pos, &BackgroundColor.b, sizeof(BackgroundColor.b));
	pos += sizeof(BackgroundColor.b);
	memcpy(Buffer + pos, &BackgroundColor.a, sizeof(BackgroundColor.a));

	return Buffer;
}

const bool PiecesW::LoadSaveBuffer(const char* Buffer)
{
	if (!Buffer) return false;

	unsigned int uBufferSize = 0;
	size_t pos = 0;

	memcpy(&uBufferSize, Buffer, sizeof(uBufferSize));
	pos += sizeof(uBufferSize);

	unsigned int uCreatureSize = 0;
	memcpy(&uCreatureSize, Buffer + pos, sizeof(uCreatureSize));
	mSize = static_cast<CreatureSize>(uCreatureSize);
	pos += sizeof(uCreatureSize);

	unsigned int uLen = 0;
	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	sType = std::wstring(uLen, L'\0');
	if (uLen)
	{
		memcpy(&sType.at(0), Buffer + pos, sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	sName = std::wstring(uLen, L'\0');
	if (uLen)
	{
		memcpy(&sName.at(0), Buffer + pos, sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	sIconPath = std::wstring(uLen, L'\0');
	if (uLen)
	{
		memcpy(&sIconPath.at(0), Buffer + pos, sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	sSpritePath = std::wstring(uLen, L'\0');
	if (uLen)
	{
		memcpy(&sSpritePath.at(0), Buffer + pos, sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	memcpy(&uLen, Buffer + pos, sizeof(uLen));
	pos += sizeof(uLen);
	sSubMenu = std::wstring(uLen, L'\0');
	if (uLen)
	{
		memcpy(&sSubMenu.at(0), Buffer + pos, sizeof(wchar_t) * uLen);
		pos += sizeof(wchar_t) * uLen;
	}

	unsigned char bools = 0;
	memcpy(&bools, Buffer + pos, sizeof(bools));
	pos += sizeof(bools);
	bDetaultInitOrder = (bools & 1);
	bKeepAspect = (bools & 2);
	bKeepIconAspect = (bools & 4);

	memcpy(&BackgroundColor.r, Buffer + pos, sizeof(BackgroundColor.r));
	pos += sizeof(BackgroundColor.r);
	memcpy(&BackgroundColor.g, Buffer + pos, sizeof(BackgroundColor.g));
	pos += sizeof(BackgroundColor.g);
	memcpy(&BackgroundColor.b, Buffer + pos, sizeof(BackgroundColor.b));
	pos += sizeof(BackgroundColor.b);
	memcpy(&BackgroundColor.a, Buffer + pos, sizeof(BackgroundColor.a));
	pos += sizeof(BackgroundColor.a);

	return true;
}