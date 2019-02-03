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
	while (pos < Buffer + lstrlenW(Buffer))
	{
		std::wstring line = GetLineW(pos ? pos : Buffer, pos);
		std::wstring front, back;
		if (!SplitString(line, front, back, L'=')) continue;

		if (!_wcsicmp(front.c_str(), L"Start"))
		{
			if (GetType().size())
			{
				pos -= line.size() + 1;
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