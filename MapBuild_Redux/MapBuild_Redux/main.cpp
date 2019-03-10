#include "window.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPWSTR cmd, int iCmdShow)
{
	setlocale(LC_ALL, "");
	DEVMODEW monitor;
	monitor.dmSize = sizeof(DEVMODEW);
	if (!EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &monitor)) return -1;

	//this sets the window size to either 1920x1080 or whatever the monitor resolution is set to
	//will add the ability to set a custom resolution at a later date; probably stored in the init.ini
	int WindowWidth = monitor.dmPelsWidth < 1920 ? monitor.dmPelsWidth : 1920;
	int WindowHeight = monitor.dmPelsHeight < 1080 ? monitor.dmPelsHeight : 1080;
	//int WindowWidth = 640;
	//int WindowHeight = 480;
	POINT StartPoint = { static_cast<long>(monitor.dmPelsWidth * 0.5f - WindowWidth * 0.5f - 5), static_cast<long>(monitor.dmPelsHeight * 0.5f - WindowHeight * 0.5f - 5) };
	//construct window
	Window wnd(StartPoint, WindowWidth, WindowHeight, L"MapBuilder", cmd);

	MSG msg = {};
	
	//main loop
	while (wnd.ProcessMessage())
	{
		wnd.Update();
		wnd.Render();
	}

	return static_cast<int>(msg.wParam);
}