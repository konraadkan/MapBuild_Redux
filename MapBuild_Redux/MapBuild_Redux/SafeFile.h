#pragma once
#include <memory>
#include <string>
#include <vector>
#include <comdef.h>

#define MAX_SIZE 512

class SafeFile
{
public:
	struct FILEDeleter
	{
		void operator() (FILE* pFile)
		{
			if (pFile) fclose(pFile);
		}
	};
	std::unique_ptr<FILE, FILEDeleter> make_unique_wfopen_s(const std::wstring wFilePath, const std::wstring wMode)
	{
		FILE* fileHandle = nullptr;
		auto err = _wfopen_s(&fileHandle, wFilePath.c_str(), wMode.c_str());
		if (err)
		{
			wchar_t wErrorMsg[MAX_SIZE] = {};
			if (_wcserror_s(wErrorMsg, MAX_SIZE, err)) MessageBoxW(nullptr, L"Unknown error.", L"Error", MB_OK | MB_ICONERROR);
			else
			{
				std::wstring wError = L"Unable to open file. Error Code: " + std::to_wstring(err) + L"\n" + wErrorMsg;
				MessageBoxW(nullptr, wError.c_str(), L"Error", MB_OK | MB_ICONERROR);
				//UpdateLog(wError, L"ReceiveData.h", static_cast<uint32_t>(__LINE__));
			}
			return nullptr;
		}
		return std::unique_ptr<FILE, FILEDeleter>(fileHandle);
	};
	const std::vector<char> GetFileBuffer(const std::wstring wFilePath, const std::wstring wMode = L"rb")
	{
		auto pFile = make_unique_wfopen_s(wFilePath, wMode);
		fseek(pFile.get(), 0, SEEK_END);
		size_t FileLen = ftell(pFile.get());
		fseek(pFile.get(), 0, SEEK_SET);

		std::vector<char> vBuffer(FileLen, '\0');
		fread(&vBuffer.front(), FileLen, 1, pFile.get());
		return vBuffer;
	}
};