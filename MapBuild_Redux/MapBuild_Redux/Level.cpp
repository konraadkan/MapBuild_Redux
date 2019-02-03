#include "Level.h"

Graphics* Level::gfx = NULL;

const std::string Level::Trim(const std::string s)
{
	return RTrim(LTrim(s));
}

const std::string Level::LTrim(const std::string s)
{
	const char whitespace[] = " \t\n\r\f\v";
	std::string temp = s;
	temp.erase(0, temp.find_first_not_of(whitespace));
	return temp;
}

const std::string Level::RTrim(const std::string s)
{
	const char whitespace[] = " \t\n\r\f\v";
	std::string temp = s;
	temp.erase(temp.find_last_not_of(whitespace) + 1);
	return temp;
}

const std::wstring Level::Trim(const std::wstring s)
{
	return RTrim(LTrim(s));
}

const std::wstring Level::LTrim(const std::wstring s)
{
	const wchar_t whitespace[] = L" \t\n\r\f\v";
	std::wstring temp = s;
	temp.erase(0, s.find_first_not_of(whitespace));
	return temp;
}

const std::wstring Level::RTrim(const std::wstring s)
{
	const wchar_t whitespace[] = L" \t\n\r\f\v";
	std::wstring temp = s;
	temp.erase(s.find_last_not_of(whitespace) + 1);
	return temp;
}