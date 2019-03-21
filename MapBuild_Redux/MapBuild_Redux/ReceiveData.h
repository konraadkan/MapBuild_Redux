#pragma once
#include <vector>
#include <cstdio>
#include <memory>
#include <string>
#include <Windows.h>
#include "Logging.h"

#define MAX_SIZE 512

class ReceiveData : public Logging
{
private:
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
				UpdateLog(wError, L"ReceiveData.h", static_cast<uint32_t>(__LINE__));
			}
			return nullptr;
		}
		return std::unique_ptr<FILE, FILEDeleter>(fileHandle);
	};
private:
	std::vector<char> vBuffer;
	uint32_t uCurrentPosition = 0;
	uint32_t uTotalSize = 0;
public:
	ReceiveData() = default;
	~ReceiveData() = default;
	ReceiveData(const ReceiveData&) = delete;
	ReceiveData& operator=(const ReceiveData&) = delete;
public:
	void FlushBuffer();
	void MoveCurrentPosition(int32_t iDistance);
	const bool FillBufferFromFile(const std::wstring wFilePath);
	const bool GetWstring(std::wstring& data);
	const std::vector<char> GetBuffer();
	const uint32_t GetCurrentPosition();
	void GetWchar(wchar_t& data, const bool bAdvanceCurrentPosition = true);
	template<typename T> uint32_t GetData(T& data, const bool bAdvanceCurrentPosition = true)
	{
		if (vBuffer.empty())
		{
			UpdateLog(L"Unable to GetData from vBuffer. vBuffer is empty.", L"ReceiveData.h", static_cast<uint32_t>(__LINE__));
			return 0;
		}
		if (uCurrentPosition >= uTotalSize)
		{
			UpdateLog(L"Unable to GetData from vBuffer. CurrentPosition > TotalSize.", L"ReceiveData.h", static_cast<uint32_t>(__LINE__));
			return 0;
		}

		memcpy(&data, &vBuffer.at(uCurrentPosition), sizeof(data));
		if (bAdvanceCurrentPosition) uCurrentPosition += sizeof(data);
		return static_cast<uint32_t>(sizeof(data));
	}
	void GetDataBuffer(std::vector<char>& vData, size_t Len, const bool bAdvanceCurrentPosition = true);
	void SetVBuffer(const std::vector<char> vData);
};