#include "StoreData.h"

void StoreData::CreateSizeEntry()
{
	vBuffer.resize(sizeof(uCurrentSize));
	uCurrentSize += sizeof(uCurrentSize);
	uCurrentPosition += sizeof(uCurrentSize);
}

void StoreData::FlushBuffer()
{
	vBuffer = std::vector<char>();
}

const bool StoreData::AddWstring(const std::wstring& wBuffer)
{
	uint32_t uBufferLen = static_cast<uint32_t>(wBuffer.size() * sizeof(wchar_t));
	if (vBuffer.empty())
	{
		vBuffer.resize(sizeof(uCurrentSize));
		uCurrentSize += sizeof(uCurrentSize);
		uCurrentPosition += sizeof(uCurrentSize);
	}
	if (!AddEntry(uBufferLen))
	{
		UpdateLog(L"Unable to add entry " + wBuffer, L"StoreDaa.cpp", static_cast<uint32_t>(__LINE__));
		return false;
	}
	vBuffer.resize(uCurrentSize + uBufferLen);
	if (uBufferLen) memcpy(&vBuffer.at(uCurrentPosition), &wBuffer.at(0), uBufferLen);
	uCurrentPosition += uBufferLen;
	uCurrentSize += uBufferLen;
	return true;
}

const uint32_t StoreData::CalcSize()
{
	return uCurrentSize;
}

void StoreData::UpdateBufferSize()
{
	if (vBuffer.empty())
	{
		CreateSizeEntry();
	}
	memcpy(&vBuffer.front(), &uCurrentSize, sizeof(uCurrentSize));
}

const bool StoreData::WriteBuffer(const std::wstring& wFilePath, const std::wstring& wMode)
{
	memcpy(&vBuffer.at(0), &uCurrentSize, sizeof(uCurrentSize));

	if (wFilePath.empty())
	{
		UpdateLog(L"Unable to write buffer. wFilePath is empty.", L"StoreData.cpp", static_cast<uint32_t>(__LINE__));
		return false;
	}
	if (wMode.empty())
	{
		UpdateLog(L"Unable to write buffer. wMode is empty.", L"StoreData.cpp", static_cast<uint32_t>(__LINE__));
		return false;
	}
	if (vBuffer.empty())
	{
		UpdateLog(L"Unable to write buffer. vBuffer is empty.", L"StoreData.cpp", static_cast<uint32_t>(__LINE__));
		return false;
	}

	auto pFile = make_unique_wfopen_s(wFilePath, wMode);
	if (!pFile.get()) return false;

	bool bSuccess = true;

	if (fwrite(&vBuffer.at(0), uCurrentSize, 1, pFile.get()) != 1)
	{
		int iError = ferror(pFile.get());
		wchar_t wErrorMsg[MAX_SIZE] = {};
		if (_wcserror_s(wErrorMsg, 512, iError))
		{
			UpdateLog(L"Failed to write buffer. Unknown error code: " + std::to_wstring(iError), L"StoreData.cpp", static_cast<uint32_t>(__LINE__));
			MessageBoxW(nullptr, L"Unable to determine error....", L"Error", MB_OK | MB_ICONERROR);
		}
		else
		{
			std::wstring wError = L"Unable to write buffer. Error Code: " + std::to_wstring(iError) + L"\n" + wErrorMsg;
			MessageBoxW(nullptr, wError.c_str(), L"Error", MB_OK | MB_ICONERROR);
			UpdateLog(wError, L"StoreData.cpp", static_cast<uint32_t>(__LINE__));
			bSuccess = false;
		}
	}
	return bSuccess;
}

void StoreData::CombineBuffer(std::vector<char> Buffer)
{
	if (Buffer.empty()) return;
	uint32_t BufferSize = static_cast<uint32_t>(Buffer.size());
	memcpy(&Buffer.front(), &BufferSize, sizeof(BufferSize));

	uCurrentSize += static_cast<uint32_t>(Buffer.size());
	vBuffer.resize(uCurrentSize);
	memcpy(&vBuffer.at(uCurrentPosition), &Buffer.front(), Buffer.size());
	uCurrentPosition += static_cast<uint32_t>(Buffer.size());
}

const std::vector<char> StoreData::GetBuffer()
{
	return vBuffer;
}