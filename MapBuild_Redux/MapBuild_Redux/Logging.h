#pragma once
#include <string>
#include <Windows.h>
#include <mutex>
#include <thread>
#include <vector>

class Logging
{
protected:
	bool bLog;
	const uint32_t uMaxSize = 1000000u;
	const uint32_t uMaxTries = 1000u;
	FILE* pLogFile = nullptr;
	std::mutex m;
	std::vector<std::thread> vThreads;
public:
	Logging(const bool Log = true) : bLog(Log) {}
	~Logging() {
		for (auto& thread : vThreads) thread.join();
	}
	virtual void WriteFile(const std::wstring wBuffer)
	{		
		uint32_t uTries = 0u;
		std::lock_guard<std::mutex> lock(m);		
		errno_t err = _wfopen_s(&pLogFile, L"log.txt", L"ab");
		if (err)
		{
			while (err == EACCES)
			{
				if (uTries >= uMaxTries)
				{
					wchar_t wErrorMsg[512] = {};
					if (_wcserror_s(wErrorMsg, 512, err)) MessageBoxW(nullptr, L"Unable to determine error....", L"Error", MB_OK | MB_ICONERROR);
					std::wstring wError = L"Unable to update log. Error Code: " + std::to_wstring(err) + L"\n" + wErrorMsg;
					MessageBoxW(nullptr, wError.c_str(), L"Error", MB_OK);
					return;
				}
				err = _wfopen_s(&pLogFile, L"log.txt", L"ab");
				uTries++;
			}
			if (err)
			{
				wchar_t wErrorMsg[512] = {};
				if (_wcserror_s(wErrorMsg, 512, err)) MessageBoxW(nullptr, L"Unable to determine error....", L"Error", MB_OK | MB_ICONERROR);
				std::wstring wError = L"Unable to update log. Error Code: " + std::to_wstring(err) + L"\n" + wErrorMsg;
				MessageBoxW(nullptr, wError.c_str(), L"Error", MB_OK);
				return;
			}
		}
		fseek(pLogFile, 0, SEEK_END);
		fwrite(&wBuffer[0], wBuffer.size() * sizeof(wchar_t), 1, pLogFile);
		err = fclose(pLogFile);
		while (err)
		{
			err = fclose(pLogFile);
		}
	}
	void UpdateLog(const std::wstring wEvent, const std::wstring wFile, const uint32_t uLineNumber)
	{
		std::mutex mu;
		mu.lock();
		if (!bLog) return;
		mu.unlock();

		SYSTEMTIME mSystemTime = {};
		GetLocalTime(&mSystemTime);
		std::wstring wTime = std::to_wstring(mSystemTime.wDay) + L"\\" +
			std::to_wstring(mSystemTime.wMonth) + L"\\" +
			std::to_wstring(mSystemTime.wYear) + L" " +
			std::to_wstring(mSystemTime.wHour) + L":" +
			std::to_wstring(mSystemTime.wMinute) + L":" +
			std::to_wstring(mSystemTime.wSecond);
		std::wstring wBuffer = wTime + L"\t" + wEvent + L"\t\t" + wFile + L"\t" + std::to_wstring(uLineNumber) + L"\r\n";
		//WriteFile(wBuffer);
		try
		{
			vThreads.push_back(std::thread([&, wBuffer] { WriteFile(wBuffer); }));
		}
		catch (const std::system_error& e)
		{
			std::string sErrorMsg = "Caught system_error code " + std::to_string(e.code().value()) + "\n" + std::string(e.what());
			MessageBox(nullptr, sErrorMsg.c_str(), "Error", MB_OK);
		}
	}
	void TrimFile()
	{
		FILE* pLogFile = nullptr;
		if (_wfopen_s(&pLogFile, L"log.txt", L"rb"))
		{
			MessageBoxW(nullptr, L"Unable to open log", L"Error", MB_OK);
			return;
		}
		fseek(pLogFile, 0, SEEK_END);
		uint32_t uFileSize = ftell(pLogFile);
		fseek(pLogFile, 0, SEEK_SET);

		if (uFileSize > uMaxSize)
		{
			size_t startpos = 0;
			fseek(pLogFile, uFileSize - uMaxSize, SEEK_SET);
			while (!feof(pLogFile))
			{
				if (fgetwc(pLogFile) == L'\n')
				{
					startpos = ftell(pLogFile) + sizeof(wchar_t);
					break;
				}
			}
			if (!feof(pLogFile))
			{
				uint32_t uNewFileSize = uFileSize - static_cast<uint32_t>(startpos);
				char* Buffer = new char[uNewFileSize];
				fread(Buffer, uNewFileSize, 1, pLogFile);
				fclose(pLogFile);

				if (_wfopen_s(&pLogFile, L"log.txt", L"wb"))
				{
					MessageBoxW(nullptr, L"Unable to trim log", L"Error", MB_OK);
					return;
				}
				fwrite(Buffer, uNewFileSize, 1, pLogFile);
				delete[] Buffer;
			}
		}
		fclose(pLogFile);
	}
};

//#pragma once
//#include <string>
//#include <Windows.h>
//
//class Logging
//{
//private:
//	bool bLog;
//	const uint32_t uMaxSize = 1000000u;
//public:
//	Logging(const bool Log = true) : bLog(Log) {}
//	void UpdateLog(std::wstring wEvent, const std::wstring wFile, const uint32_t uLineNumber)
//	{
//		if (!bLog) return;
//		FILE* pLogFile = nullptr;
//		SYSTEMTIME mSystemTime = {};
//		GetLocalTime(&mSystemTime);
//		std::wstring wTime = std::to_wstring(mSystemTime.wDay) + L"\\" +
//			std::to_wstring(mSystemTime.wMonth) + L"\\" +
//			std::to_wstring(mSystemTime.wYear) + L" " +
//			std::to_wstring(mSystemTime.wHour) + L":" +
//			std::to_wstring(mSystemTime.wMinute) + L":" +
//			std::to_wstring(mSystemTime.wSecond);
//		std::wstring wBuffer = wTime + L"\t" + wEvent + L"\t\t" + wFile + L"\t" + std::to_wstring(uLineNumber) + L"\r\n";
//		if (_wfopen_s(&pLogFile, L"log.txt", L"ab"))
//		{
//			MessageBoxW(nullptr, L"Unable to update log", L"Error", MB_OK);
//			return;
//		}
//		fseek(pLogFile, 0, SEEK_END);
//		fwrite(&wBuffer[0], wBuffer.size() * sizeof(wchar_t), 1, pLogFile);
//		fclose(pLogFile);
//		TrimFile();
//	}
//	void TrimFile()
//	{
//		FILE* pLogFile = nullptr;
//		if (_wfopen_s(&pLogFile, L"log.txt", L"rb"))
//		{
//			MessageBoxW(nullptr, L"Unable to open log", L"Error", MB_OK);
//			return;
//		}
//		fseek(pLogFile, 0, SEEK_END);
//		uint32_t uFileSize = ftell(pLogFile);
//		fseek(pLogFile, 0, SEEK_SET);
//		
//		if (uFileSize > uMaxSize)
//		{
//			size_t startpos = 0;
//			fseek(pLogFile, uFileSize - uMaxSize, SEEK_SET);
//			while (!feof(pLogFile))
//			{
//				if (fgetwc(pLogFile) == L'\n')
//				{
//					startpos = ftell(pLogFile) + sizeof(wchar_t);
//					break;
//				}
//			}
//			if (!feof(pLogFile))
//			{
//				uint32_t uNewFileSize = uFileSize - static_cast<uint32_t>(startpos);
//				char* Buffer = new char[uNewFileSize];
//				fread(Buffer, uNewFileSize, 1, pLogFile);
//				fclose(pLogFile);
//
//				if (_wfopen_s(&pLogFile, L"log.txt", L"wb"))
//				{
//					MessageBoxW(nullptr, L"Unable to trim log", L"Error", MB_OK);
//					return;
//				}
//				fwrite(Buffer, uNewFileSize, 1, pLogFile);
//				delete[] Buffer;
//			}
//		}
//		fclose(pLogFile);
//	}
//};