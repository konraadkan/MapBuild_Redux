#include "ReceiveData.h"

const std::vector<char> ReceiveData::GetBuffer()
{
	return vBuffer;
}

const uint32_t ReceiveData::GetCurrentPosition()
{
	return uCurrentPosition;
}

void ReceiveData::FlushBuffer()
{
	vBuffer = std::vector<char>();
}

void ReceiveData::MoveCurrentPosition(int32_t iDistance)
{
	uCurrentPosition += iDistance;
	if (uCurrentPosition > uTotalSize)
	{
		UpdateLog(L"Unable to move current position. The new position is outside the buffer.", L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
		uCurrentPosition -= iDistance;
	}
}

void ReceiveData::GetWchar(wchar_t& data, const bool bAdvanceCurrentPosition)
{
	if (vBuffer.empty())
	{
		UpdateLog(L"Unable to GetDataBuffer. vBuffer is empty.", L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
		return;
	}
	if (uCurrentPosition >= uTotalSize)
	{
		UpdateLog(L"Unable to GetDataBuffer. CurrentPosition > TotalSize.", L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
		return;
	}

	memcpy(&data, &vBuffer.at(uCurrentPosition), sizeof(wchar_t));
	if (bAdvanceCurrentPosition) uCurrentPosition += sizeof(wchar_t);
}

void ReceiveData::SetVBuffer(const std::vector<char> vData)
{
	FlushBuffer();
	vBuffer = vData;
	uTotalSize = static_cast<uint32_t>(vBuffer.size());
	uCurrentPosition = 0;
}

void ReceiveData::GetDataBuffer(std::vector<char>& vData, size_t Len, const bool bAdvanceCurrentPosition)
{
	if (vBuffer.empty())
	{
		UpdateLog(L"Unable to GetDataBuffer. vBuffer is empty.", L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
		return;
	}
	if (uCurrentPosition + Len > uTotalSize)
	{
		UpdateLog(L"Unable to GetDataBuffer. CurrentPosition + Len > TotalSize.", L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
		return;
	}
	
	vData = std::vector<char>();
	vData.resize(Len);
	memcpy(&vData.front(), &vBuffer.at(uCurrentPosition), Len);
	if (bAdvanceCurrentPosition) uCurrentPosition += static_cast<uint32_t>(Len);
}

const bool ReceiveData::FillBufferFromFile(const std::wstring wFilePath)
{
	if (wFilePath.empty())
	{
		UpdateLog(L"Unable to fill buffer. wFilePath is empty.", L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
		return false;
	}
	if (vBuffer.size()) FlushBuffer();

	auto pFile = make_unique_wfopen_s(wFilePath, L"rb");
	if (!pFile.get()) return false;

	fseek(pFile.get(), 0, SEEK_END);
	uTotalSize = ftell(pFile.get());
	fseek(pFile.get(), 0, SEEK_SET);

	bool bSuccess = true;

	vBuffer.resize(uTotalSize);
	if (fread(&vBuffer.at(0), uTotalSize, 1, pFile.get()) != 1)
	{
		int iError = ferror(pFile.get());
		wchar_t wErrorMsg[MAX_SIZE] = {};
		if (_wcserror_s(wErrorMsg, 512, iError)) MessageBoxW(nullptr, L"Unable to determine error....", L"Error", MB_OK | MB_ICONERROR);
		else
		{
			std::wstring wError = L"Unable to read to buffer. Error Code: " + std::to_wstring(iError) + L"\n" + wErrorMsg;
			//MessageBoxW(nullptr, wError.c_str(), L"Error", MB_OK | MB_ICONERROR);
			UpdateLog(wError, L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
			bSuccess = false;
		}
	}

	return bSuccess;
}

const bool ReceiveData::GetWstring(std::wstring& data)
{
	if (vBuffer.empty())
	{
		UpdateLog(L"Unable to GetDataBuffer. vBuffer is empty.", L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
		return false;
	}
	if (uCurrentPosition >= uTotalSize)
	{
		UpdateLog(L"Unable to GetDataBuffer. CurrentPosition + Len > TotalSize.", L"ReceiveData.cpp", static_cast<uint32_t>(__LINE__));
		return false;
	}

	uint32_t uStringSize = 0;
	GetData(uStringSize);
	if (!uStringSize) return false;

	data.resize(static_cast<size_t>(uStringSize * 0.5));
	memcpy(&data.at(0), &vBuffer.at(uCurrentPosition), uStringSize);
	data.shrink_to_fit();
	uCurrentPosition += uStringSize;
	return true;
}