#include "window.h"
#include "resource.h"

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

Window::Window(int Width, int Height, const wchar_t* name) noexcept : width(Width), height(Height)
{
	RECT wr = {};
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false);
	hWnd = CreateWindowW(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, WindowClass::GetInstance(), this);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	hAccel = LoadAcceleratorsW(WindowClass::GetInstance(), MAKEINTRESOURCEW(IDR_ACCELERATOR1));

	gfx = new Graphics;
	if (!gfx->Init(hWnd))
	{
		MessageBoxW(hWnd, L"Unable to Initialize graphics.", L"Error", MB_OK | MB_ICONERROR);
	}
	pTimer = new HPTimer();
	if (!pTimer)
	{
		MessageBoxW(hWnd, L"Unable to Initialize Timer.", L"Error", MB_OK | MB_ICONERROR);
	}
	Controller::SwitchLevel(new BaseLevel(gfx, &m_MouseCoordinates, width, height, pTimer));
}

Window::Window(POINT p, int Width, int Height, const wchar_t* name) noexcept : width(Width), height(Height)
{
	RECT wr = {};
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false);
	hWnd = CreateWindowW(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, p.x, p.y, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, WindowClass::GetInstance(), this);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	hAccel = LoadAcceleratorsW(WindowClass::GetInstance(), MAKEINTRESOURCEW(IDR_ACCELERATOR1));

	gfx = new Graphics;
	if (!gfx->Init(hWnd))
	{
		MessageBoxW(hWnd, L"Unable to Initialize graphics.", L"Error", MB_OK | MB_ICONERROR);
	}
	pTimer = new HPTimer();
	if (!pTimer)
	{
		MessageBoxW(hWnd, L"Unable to Initialize Timer.", L"Error", MB_OK | MB_ICONERROR);
	}

	Controller::SwitchLevel(new BaseLevel(gfx, &m_MouseCoordinates, width, height, pTimer));
}

Window::~Window()
{
	Controller::Cleanup();
	SafeDelete(&gfx);
	SafeDelete(&pTimer);
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
		if (m_MouseCoordinates.x > 0 && m_MouseCoordinates.y < width && m_MouseCoordinates.y > 0 && m_MouseCoordinates.y < height)
		{
			Controller::m_Mouse.OnMouseMove(m_MouseCoordinates.x, m_MouseCoordinates.y);
			if (!Controller::m_Mouse.InWindow())
			{
				Controller::m_Mouse.OnMouseEnter();
			}
		}
		else
		{
			m_MouseCoordinates.x = m_MouseCoordinates.x > 0.0f ? m_MouseCoordinates.x : 0.0f;
			m_MouseCoordinates.x = m_MouseCoordinates.x < width ? m_MouseCoordinates.x : width;
			m_MouseCoordinates.y = m_MouseCoordinates.y > 0.0f ? m_MouseCoordinates.y : 0.0f;
			m_MouseCoordinates.y = m_MouseCoordinates.y < height ? m_MouseCoordinates.y : height;
			if (wParam & (MK_LBUTTON | MK_RBUTTON | MK_MBUTTON))
			{				
				Controller::m_Mouse.OnMouseMove(m_MouseCoordinates.x, m_MouseCoordinates.y);
			}
			else
			{
				Controller::m_Mouse.OnMouseLeave();
				Controller::m_Mouse.OnLeftReleased(m_MouseCoordinates.x, m_MouseCoordinates.y);
				Controller::m_Mouse.OnRightReleased(m_MouseCoordinates.x, m_MouseCoordinates.y);
				Controller::m_Mouse.OnMiddleReleased(m_MouseCoordinates.x, m_MouseCoordinates.y);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		Controller::m_Mouse.OnLeftPressed(m_MouseCoordinates.x, m_MouseCoordinates.y);
		SetForegroundWindow(hWnd);
		break;
	case WM_RBUTTONDOWN:
		Controller::m_Mouse.OnRightPressed(m_MouseCoordinates.x, m_MouseCoordinates.y);
		break;
	case WM_MBUTTONDOWN:
		Controller::m_Mouse.OnMiddlePressed(m_MouseCoordinates.x, m_MouseCoordinates.y);
		break;
	case WM_LBUTTONUP:
		Controller::m_Mouse.OnLeftReleased(m_MouseCoordinates.x, m_MouseCoordinates.y);
		break;
	case WM_RBUTTONUP:
		Controller::m_Mouse.OnRightReleased(m_MouseCoordinates.x, m_MouseCoordinates.y);
		break;
	case WM_MBUTTONUP:
		Controller::m_Mouse.OnMiddleReleased(m_MouseCoordinates.x, m_MouseCoordinates.y);
		break;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			Controller::m_Mouse.OnWheelUp(m_MouseCoordinates.x, m_MouseCoordinates.y);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
		{
			Controller::m_Mouse.OnWheelDown(m_MouseCoordinates.x, m_MouseCoordinates.y);
		}
		break;
	case WM_CHAR:
		Controller::m_Keyboard.OnChar(static_cast<wchar_t>(wParam));
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000) || Controller::m_Keyboard.AutorepeatEnabled()) Controller::m_Keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Controller::m_Keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_KILLFOCUS:
		Controller::m_Keyboard.ClearState();
		break;
	case WM_COMMAND:
		//accelerator interaction here
		switch (LOWORD(wParam))
		{
		case ID_COPY_CMD:
			//may not use
			break;
		case ID_OPEN_CMD:
			//load a saved map
			break;
		case ID_NEW_CMD:
			//new map
			break;
		case ID_SAVE_CMD:
			//save the map
			break;
		case ID_RESIZE_CMD:
			//take input from user for exact resolution for the backbuffer
			break;
		}
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