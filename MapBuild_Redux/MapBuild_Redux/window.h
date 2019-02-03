#pragma once
#include <Windows.h>
#include <vector>
#include "HPTimer.h"
#include "Graphics.h"
#include "Controller.h"
#include "BaseLevel.h"

class Window
{
private:
	template<class T> void SafeDelete(T** ppT)
	{
		if (ppT)
		{
			delete (*ppT);
			(*ppT) = nullptr;
		}
	}
private:
	struct Move
	{
		bool bMove = false;
		enum class Direction
		{
			Up,
			Down,
			Left,
			Right,
			Error
		};
		std::vector<Direction> dir;
	} s_Movement;

	static D2D1_POINT_2F GetMousePositionForCurrentDpi(LPARAM lParam, ID2D1Factory* factory)
	{
		static D2D1_POINT_2F dpi = { 96,96 };	//default dpi

		factory->GetDesktopDpi(&dpi.x, &dpi.y);

		return D2D1::Point2F(static_cast<int>(static_cast<short>(LOWORD(lParam))) * 96 / dpi.x,
			static_cast<int>(static_cast<short>(HIWORD(lParam))) * 96 / dpi.y);
	}
private:
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const wchar_t* wndClassName = L"ClassNamething";
		static WindowClass wndClass;
		HINSTANCE hInstance = NULL;
	};
public:
	Window(int width, int height, const wchar_t* name) noexcept;
	Window(POINT p, int width, int height, const wchar_t* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	HWND& GetHWND() { return hWnd; }
	bool ProcessMessage();
	void SetGraphics(Graphics* const graphics) { gfx = graphics; }
	D2D1_POINT_2F GetMousePosition() { return m_MouseCoordinates; }
	Graphics* const GetGraphics() { return gfx; }
	void Render()
	{ 		
		Controller::Render();		
	}
	void Update()
	{
		Controller::Update(m_Timer->GetTimeDelta());
		m_Timer->Update();
	}
	const int GetWidth() { return width; }
	const int GetHeight() { return height; }
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width = 0;
	int height = 0;
	float scale = 1.0f;
	D2D1_SIZE_F offset = D2D1::SizeF();
	D2D1_POINT_2F pCenter = D2D1::Point2F(static_cast<float>(width) / 2.0f, 0.0f);
	D2D1_POINT_2F pTransformed = {};
	HWND hWnd = NULL;
	D2D1_POINT_2F m_MouseCoordinates = D2D1::Point2F();
	HACCEL hAccel = NULL;
	Graphics* gfx = NULL;
	HPTimer* m_Timer = NULL;
};