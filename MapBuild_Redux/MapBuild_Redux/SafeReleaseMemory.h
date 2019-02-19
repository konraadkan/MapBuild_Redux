#pragma once

class SafeReleaseMemory
{
public:
	template<typename T> void SafeRelease(T** ppT)
	{
		if (ppT)
		{
			if (*ppT)
			{
				(*ppT)->Release();
				(*ppT) = nullptr;
			}
		}
	}
	template<typename T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			if (*ppT)
			{
				delete (*ppT);
				(*ppT) = nullptr;
			}
		}
	}
	template<typename T> void SafeDeleteArray(T** ppT)
	{
		if (ppT)
		{
			if (*ppT)
			{
				delete[](*ppT);
				(*ppT) = nullptr;
			}
		}
	}
};