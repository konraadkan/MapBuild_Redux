#pragma once
#include "Graphics.h"
#include "keyboard.h"
#include "Mouse.h"
#include <queue>

class Level
{
protected:
	template<typename T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			delete (*ppT);
			(*ppT) = nullptr;
		}
	}
protected:
	static Graphics* gfx;
	Keyboard* pKeyboard = nullptr;
	Mouse* pMouse = nullptr;
	bool bGridOnTop = false;
public:	
	virtual void Load(Keyboard* const keyboard, Mouse* const mouse) = 0;
	virtual void Unload() = 0;
	virtual void Render() = 0;
	virtual void Update(double dDelta) = 0;
	virtual void ProcessEvents(double dDelta) = 0;
	virtual bool Open(const wchar_t* sFilePath)
	{
		return false;
	}
	virtual void SetGridOnTop() { bGridOnTop = true; }
	virtual void UnsetGridOnTop() { bGridOnTop = false; }
public:
	struct Move
	{
		bool bMove = false;
		enum class Direction
		{
			Up,
			Down,
			Left,
			Right,
			RotatePositive,
			RotateNegative,
			Error
		};
		std::vector<Direction> vDirection;
	} Movement;
};