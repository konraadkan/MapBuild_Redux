#pragma once
#include "Graphics.h"
#include "StringMod.h"
#include "HPTimer.h"
#include "Logging.h"
#include "ReceiveData.h"
#include <vector>
#include <wincodec.h>

class Sprite : public StringMod, public Logging
{
private:
	template<typename T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			if (*ppT) delete (*ppT);
			(*ppT) = nullptr;
		}
	}
	template<typename T> void SafeDeleteArray(T** ppT)
	{
		if (ppT)
		{
			if (*ppT) delete[](*ppT);
			(*ppT) = nullptr;
		}
	}
	template<typename T> void SafeRelease(T** ppT)
	{
		if (ppT)
		{
			if(*ppT) (*ppT)->Release();
			(*ppT) = nullptr;
		}
	}
private:
	double dTimeout = 30.0;
	HPTimer* const pTimer;
	float cTime = 0.0f;
	float pTime = 0.0f;
	bool bLoadSuccess = false;
	bool bIsSPR3 = false;
	Graphics* const gfx = nullptr;
	std::wstring sFilePathW;
	std::vector<D2D1_RECT_F> vFrames;
	ID2D1Bitmap* pBitmap = nullptr;
	ID2D1Bitmap* pFrameBitmap = nullptr;
	D2D1_SIZE_F fBitmapSize = D2D1::SizeF();
	int iCurrentFrame = 0;
	D2D1_SIZE_F* pGridSize = nullptr;
protected:
	float fFrameSpeed = 0.33f;
public:
	Sprite(const wchar_t* sFilePath, Graphics* const graphics, HPTimer* const timer, D2D1_SIZE_F* const gridsize);
	~Sprite();
	Sprite(const Sprite&) = delete;
	Sprite& operator=(const Sprite&) = delete;
	bool InitFromMemory(char* const Buffer, size_t BufferLen);
public:
	void TranslateLine(std::string sLine);
	void SendErrorMessage(HRESULT hr, int iLineNumber);
	void SendErrorMessage(std::wstring filname, HRESULT hr, int iLineNumber);
	void SetIsSpr3() { bIsSPR3 = true; }
	void UnsetIsSpr3() { bIsSPR3 = false; }
	void AddFrame(D2D1_RECT_F rect) { vFrames.push_back(rect); }
	bool IsSpr3() { return bIsSPR3; }
	bool IsSuccess() { return bLoadSuccess; }
	void NextFrame();
	void CreateFrameBitmap();
	D2D1_SIZE_F GetBitmapSize() { return fBitmapSize; }
	D2D1_RECT_F GetFrame() { if (vFrames.size()) return vFrames.at(iCurrentFrame); return D2D1::RectF(); }
	D2D1_SIZE_F GetFrameSize()
	{
		if (vFrames.empty()) return D2D1::SizeF();
		return D2D1::SizeF(vFrames.at(iCurrentFrame).right - vFrames.at(iCurrentFrame).left, vFrames.at(iCurrentFrame).bottom - vFrames.at(iCurrentFrame).top); 
	}
	ID2D1Bitmap* const GetBitmap() { return pBitmap; }
	ID2D1Bitmap* const GetFrameBitmap() { return pFrameBitmap; }
private:
	char* DecodeSPR3(const wchar_t* FilePath, uint32_t& ImageBufferLen);
	char* DecodeSPR(const wchar_t* FilePath);
	char* GetStreamBuffer(const wchar_t* FilePath, size_t& ImageBufferLen);
};