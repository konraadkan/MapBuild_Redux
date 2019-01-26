#include "window.h"
#include "HPTimer.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPWSTR cmd, int iCmdShow)
{
	DEVMODEW monitor;
	monitor.dmSize = sizeof(DEVMODEW);
	if (!EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &monitor)) return -1;

	//this sets the window size to either 1920x1080 or whatever the monitor resolution is set to
	//will add the ability to set a custom resolution at a later date; probably stored in the init.ini
	int WindowWidth = monitor.dmPelsWidth < 1920 ? monitor.dmPelsWidth : 1920;
	int WindowHeight = monitor.dmPelsHeight < 1080 ? monitor.dmPelsHeight : 1080;

	//construct window
	Window wnd(WindowWidth, WindowHeight, L"MapBuilder");

	MSG msg = {};
	HPTimer* timer = new HPTimer;

	//main loop
	while (wnd.ProcessMessage())
	{
		timer->Update();
		wnd.Render();
	}

	//cleanup
	if (timer) delete timer;
	return msg.wParam;
}