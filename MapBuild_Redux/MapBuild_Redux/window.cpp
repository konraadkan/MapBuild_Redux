#include "window.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInstance(GetModuleHandle(NULL))
{
	WNDCLASSEXW wndClass = {};
	wndClass.cbSize = sizeof(wndClass);
	wndClass.style = CS_OWNDC;
	wndClass.hInstance = GetInstance();
	wndClass.hIcon = (HICON)LoadIcon(NULL, IDI_APPLICATION);			//default application icon
	wndClass.hIconSm = NULL;
	wndClass.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);			//default arrow curosr
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = GetName();
	wndClass.lpfnWndProc = HandleMsgSetup;
	RegisterClassExW(&wndClass);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClassW(wndClassName, GetInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInstance;
}

Window::Window(int width, int height, const wchar_t* name) noexcept
{
	RECT wr = {};
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false);
	hWnd = CreateWindowW(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, WindowClass::GetInstance(), this);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	//hAccel = LoadAcceleratorsW(WindowClass::GetInstance(), MAKEINTRESOURCEW(IDR_ACCELERATOR1));

	gfx = new Graphics;
	if (!gfx->Init(hWnd))
	{
		MessageBoxW(hWnd, L"Unable to Initialize graphics.", L"Error", MB_OK | MB_ICONERROR);
	}
	m_Timer = new HPTimer();
	if (!m_Timer)
	{
		MessageBoxW(hWnd, L"Unable to Initialize Timer.", L"Error", MB_OK | MB_ICONERROR);
	}

	Controller::SwitchLevel(new BaseLevel(gfx, &m_MouseCoordinates));
}

Window::Window(POINT p, int width, int height, const wchar_t* name) noexcept
{
	RECT wr = {};
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false);
	hWnd = CreateWindowW(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, p.x, p.y, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, WindowClass::GetInstance(), this);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	//hAccel = LoadAcceleratorsW(WindowClass::GetInstance(), MAKEINTRESOURCEW(IDR_ACCELERATOR1));

	gfx = new Graphics;
	if (!gfx->Init(hWnd))
	{
		MessageBoxW(hWnd, L"Unable to Initialize graphics.", L"Error", MB_OK | MB_ICONERROR);
	}
	m_Timer = new HPTimer();
	if (!m_Timer)
	{
		MessageBoxW(hWnd, L"Unable to Initialize Timer.", L"Error", MB_OK | MB_ICONERROR);
	}
}

Window::~Window()
{
	Controller::Cleanup();
	SafeDelete(&gfx);
	SafeDelete(&m_Timer);
	if (hAccel) DestroyAcceleratorTable(hAccel);
	DestroyWindow(hWnd);
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//use create parameter to store window pointer
	if (msg == WM_NCCREATE)
	{
		//extract pointer to window class from the creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		//set winapi-managed user data to store ptr to window class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		//set message proc to normal handler
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		//forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	//retrieve ptr to window class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	//forward message to window class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_MOUSEMOVE:
		if (gfx->GetFactory()) m_MouseCoordinates = GetMousePositionForCurrentDpi(lParam, gfx->GetFactory());
		else
		{
			POINT tp = {};
			GetCursorPos(&tp);
			ScreenToClient(hWnd, &tp);
			m_MouseCoordinates = D2D1::Point2F(static_cast<float>(tp.x), static_cast<float>(tp.y));
		}
		break;
	case WM_CHAR:
		Controller::m_Keyboard.OnChar(static_cast<wchar_t>(wParam));
		break;
	case WM_KEYDOWN:
		if (!(lParam & 0x40000000) || Controller::m_Keyboard.AutorepeatEnabled()) Controller::m_Keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	case WM_KEYUP:
		Controller::m_Keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_COMMAND:
		//accelerator interaction here
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

bool Window::ProcessMessage()
{
	MSG msg = {};
	while (PeekMessageW(&msg, 0, 0, NULL, PM_REMOVE))
	{
		if (!TranslateAcceleratorW(hWnd, hAccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
			if (msg.message == WM_QUIT) return false;
		}
	}

	return true;
}