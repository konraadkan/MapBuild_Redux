#pragma once
#include <vector>
#include <cstdio>
#include <memory>
#include <string>
#include <Windows.h>
#include "Logging.h"

#define MAX_SIZE 512

class StoreData : public Logging
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
				UpdateLog(wError, L"StoreData.h", static_cast<uint32_t>(__LINE__));
			}
			return nullptr;
		}
		return std::unique_ptr<FILE, FILEDeleter>(fileHandle);
	};
private:
	uint32_t uCurrentSize = 0;
	uint32_t uCurrentPosition = 0;
	std::vector<char> vBuffer;
public:
	StoreData() = default;
	~StoreData() = default;
	StoreData(const StoreData& s)
	{
		uCurrentSize = s.uCurrentSize;
		uCurrentPosition = s.uCurrentPosition;
		vBuffer = s.vBuffer;
	}
	StoreData& operator=(const StoreData& s)
	{
		uCurrentSize = s.uCurrentSize;
		uCurrentPosition = s.uCurrentPosition;
		vBuffer = s.vBuffer;
	}
public:
	void CreateSizeEntry();

	template<typename T> const bool AddEntry(const T& data)
	{
		if (vBuffer.empty())
		{
			CreateSizeEntry();
		}
		vBuffer.resize(uCurrentSize + sizeof(data));
		memcpy(&vBuffer[uCurrentPosition], &data, sizeof(data));
		uCurrentPosition += sizeof(data);
		uCurrentSize = static_cast<uint32_t>(vBuffer.size());
		return true;
	}
	const bool AddWstring(const std::wstring& wBuffer);
	const uint32_t CalcSize();
	const bool WriteBuffer(const std::wstring& wFilePath, const std::wstring& wMode);
	void CombineBuffer(std::vector<char> Buffer);
	void FlushBuffer();
	void UpdateBufferSize();
	const std::vector<char> GetBuffer();
};