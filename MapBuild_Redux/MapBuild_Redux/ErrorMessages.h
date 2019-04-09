#pragma once
#include<comdef.h>
#include<string>
#include<stdexcept>

class ErrorMsgs
{
public:
	struct GfxException : public std::exception
	{
		GfxException() : std::exception() {}
		GfxException(const char* str) : std::exception(str) {}
	};
	void SendErrorMessage(HRESULT hr, const uint32_t iLineNumber)
	{
		_com_error err(hr);
		std::string sErrMsg = err.ErrorMessage();
		std::wstring sErrMsgW(sErrMsg.begin(), sErrMsg.end());
		sErrMsgW = L"Error on Line " + std::to_wstring(iLineNumber) + std::wstring(L".\n") + sErrMsgW;
		MessageBoxW(nullptr, sErrMsgW.c_str(), L"Error", MB_OK | MB_ICONERROR);
	}
	void SendErrorMessage(const DWORD err, const uint32_t uLineNumber)
	{
		wchar_t wErrorMsg[512] = {};
		if (_wcserror_s(wErrorMsg, 512, err))
		{
			MessageBoxW(nullptr, L"Unable to determine error....", L"Error", MB_OK | MB_ICONERROR);
		}
		else
		{
			std::wstring wError = L"Error on Line " + std::to_wstring(uLineNumber) + L".\nError Code: " + std::to_wstring(err) + L"\n" + wErrorMsg;
			MessageBoxW(nullptr, wError.c_str(), L"Error", MB_OK | MB_ICONERROR);
		}
	}
	void SendErrorMessage(const std::string sErrMsg)
	{
		std::wstring wErrMsg(sErrMsg.begin(), sErrMsg.end());
		MessageBoxW(nullptr, wErrMsg.c_str(), L"Error", MB_OK | MB_ICONERROR);
	}
};