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
	bool bLockToGrid = true;
	bool bKeepAspect = true;
	bool bUseTexture = false;	
	bool bExit = false;
	bool bUpdateRotation = false;	
public:	
	virtual const bool CreateNew() = 0;
	virtual const bool IsExit() { return bExit; }
	virtual const std::string Trim(const std::string s);
	virtual const std::string LTrim(const std::string s);
	virtual const std::string RTrim(const std::string s);
	virtual const std::wstring Trim(const std::wstring s);
	virtual const std::wstring LTrim(const std::wstring s);
	virtual const std::wstring RTrim(const std::wstring s);
	virtual void Load(Keyboard* const keyboard, Mouse* const mouse) = 0;
	virtual void Unload() = 0;
	virtual void Render() = 0;
	virtual void Update(double dDelta) = 0;
	virtual void ProcessEvents(double dDelta) = 0;
	virtual const bool Open(const std::wstring sFilePath) = 0;
	virtual const bool Save(const std::wstring wFilePath) = 0;
	virtual void SetGridOnTop() { bGridOnTop = true; }
	virtual void UnsetGridOnTop() { bGridOnTop = false; }
	virtual void SetUseTexture() { bUseTexture = true; }
	virtual void UnsetUseTexture() { bUseTexture = false; }
	virtual void ToggleUseTexture() { bUseTexture ^= true; }
	virtual const bool UseTexture() { return bUseTexture; }
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