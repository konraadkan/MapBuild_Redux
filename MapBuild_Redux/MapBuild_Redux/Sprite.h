#pragma once
#include "Graphics.h"
#include "StringMod.h"
#include "HPTimer.h"
#include <vector>
#include <wincodec.h>

class Sprite : public StringMod
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
	HPTimer* const pTimer;
	float cTime = 0.0f;
	float pTime = 0.0f;
	bool bLoadSuccess = false;
	bool bIsSPR3 = false;
	Graphics* const gfx = nullptr;
	std::wstring sFilePathW;
	std::vector<D2D1_RECT_F> vFrames;
	ID2D1Bitmap* pBitmap = nullptr;
	D2D1_SIZE_F fBitmapSize = D2D1::SizeF();
	int iCurrentFrame = 0;
protected:
	float fFrameSpeed = 0.33f;
public:
	Sprite(const wchar_t* sFilePath, Graphics* const graphics, HPTimer* const timer);
	~Sprite();
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
	D2D1_SIZE_F GetBitmapSize() { return fBitmapSize; }
	D2D1_RECT_F GetFrame() { if (vFrames.size()) return vFrames.at(iCurrentFrame); return D2D1::RectF(); }
	D2D1_SIZE_F GetFrameSize()
	{
		if (vFrames.empty()) return D2D1::SizeF();
		return D2D1::SizeF(vFrames.at(iCurrentFrame).right - vFrames.at(iCurrentFrame).left, vFrames.at(iCurrentFrame).bottom - vFrames.at(iCurrentFrame).top); 
	}
	ID2D1Bitmap* const GetBitmap() { return pBitmap; }
private:
	char* DecodeSPR3(const wchar_t* FilePath, unsigned int& ImageBufferLen);
	char* DecodeSPR(const wchar_t* FilePath);
	char* GetStreamBuffer(const wchar_t* FilePath, size_t& ImageBufferLen);
};