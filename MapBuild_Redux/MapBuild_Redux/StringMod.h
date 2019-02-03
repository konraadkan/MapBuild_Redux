#pragma once
#include<string>

class StringMod
{
public:
	const std::wstring GetLineW(wchar_t* const buffer, wchar_t*& pos);
	const std::wstring GetLineW(char* const buffer, char*& pos);

	const std::wstring Trim(const std::wstring s);
	const std::wstring TrimL(const std::wstring s);
	const std::wstring TrimR(const std::wstring s);

	bool SplitString(const std::wstring s, std::wstring& front, std::wstring& back, wchar_t wc);
public:
	const std::string GetLine(char* const buffer, char*& pos);

	const std::string Trim(const std::string s);
	const std::string TrimL(const std::string s);
	const std::string TrimR(const std::string s);

	bool SplitString(const std::string s, std::string& front, std::string& back, char c);
};