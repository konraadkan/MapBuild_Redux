#include "StringMod.h"

const std::wstring StringMod::GetLineW(wchar_t* const buffer, wchar_t*& pos)
{	
	std::wstring wbuffer = (buffer[0] == 0xFEFF) ? buffer + 1 : buffer;

	wchar_t endline[] = L";ş\n\r";
	auto p = wbuffer.find_first_of(endline);
	if (p == std::string::npos)
	{
		pos = pos + wbuffer.size() + 1;
		return wbuffer;
	}
	pos = buffer + p + 2;

	std::wstring tw(wbuffer.begin(), wbuffer.begin() + p);
	return Trim(tw);
}

const std::wstring StringMod::GetLineW(char* const buffer, char*& pos)
{
	std::wstring wbuffer(strlen(buffer), L' ');
	size_t conv = 0;
	mbstowcs_s(&conv, &wbuffer[0], strlen(buffer) + 1, buffer, strlen(buffer));

	wchar_t endline[] = L";ş\n\r";
	auto p = wbuffer.find_first_of(endline);
	if (p == std::string::npos)
	{
		pos = pos + wbuffer.size() + 1;
		return wbuffer;
	}
	pos = buffer + p + 1;

	std::wstring tw(wbuffer.begin(), wbuffer.begin() + p);
	return Trim(tw);
}

const std::string StringMod::GetLine(char* const buffer, char*& pos)
{
	std::string sbuffer = buffer;

	char endline[] = ";ş\n\r";
	auto p = sbuffer.find_first_of(endline);
	if (p == std::string::npos)
	{
		pos = pos + sbuffer.size() + 1;
		return sbuffer;
	}

	pos = buffer + p + 1;

	std::string t(sbuffer.begin(), sbuffer.begin() + p);
	return Trim(t);
}

const std::wstring StringMod::Trim(const std::wstring s)
{
	return TrimR(TrimL(s));
}

const std::string StringMod::Trim(const std::string s)
{
	return TrimR(TrimL(s));
}

const std::wstring StringMod::TrimL(const std::wstring s)
{
	const wchar_t whitespace[] = L" \t\n\r\f\v";
	std::wstring temp = s;
	temp.erase(0, s.find_first_not_of(whitespace));
	return temp;
}

const std::string StringMod::TrimL(const std::string s)
{
	const char whitespace[] = " \t\n\r\f\v";
	std::string temp = s;
	temp.erase(0, temp.find_first_not_of(whitespace));
	return temp;
}

const std::wstring StringMod::TrimR(const std::wstring s)
{
	const wchar_t whitespace[] = L" \t\n\r\f\v";
	std::wstring temp = s;
	temp.erase(temp.find_last_not_of(whitespace) + 1);
	return temp;
}

const std::string StringMod::TrimR(const std::string s)
{
	const char whitespace[] = " \t\n\r\f\v";
	std::string temp = s;
	temp.erase(temp.find_last_not_of(whitespace) + 1);
	return temp;
}

bool StringMod::SplitString(const std::wstring s, std::wstring& front, std::wstring& back, wchar_t wc)
{
	auto splitpoint = s.find_first_of(wc);
	if (splitpoint == std::string::npos) return false;
	if (splitpoint >= s.size()) return false;

	front = Trim(std::wstring(s.begin(), s.begin() + splitpoint));
	back = Trim(std::wstring(s.begin() + splitpoint + 1, s.end()));
	return true;
}

bool StringMod::SplitString(const std::string s, std::string& front, std::string& back, char c)
{
	auto splitpoint = s.find_first_of(c);
	if (splitpoint == std::string::npos) return false;
	if (splitpoint >= s.size()) return false;

	front = Trim(std::string(s.begin(), s.begin() + splitpoint));
	back = Trim(std::string(s.begin() + splitpoint + 1, s.end()));
	return true;
}