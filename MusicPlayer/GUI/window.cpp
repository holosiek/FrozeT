#include <SFML/Window.hpp>
#include <windows.h>
#include <iostream>
#include "../config.hpp"
#include "window.hpp"

namespace GUI{
	namespace Window{
		// Change window fullscreen or not
		void windowSetFullscreen(sf::WindowHandle a_handle){
			WINDOWPLACEMENT static g_wpPrev = { sizeof(g_wpPrev) }; 
			DWORD           static dwStyle  = GetWindowLong(a_handle, GWL_STYLE);
			if(!cfg.isFullscreen){
				// "mi" will store monitor information, which will be used for positioning
				MONITORINFO mi = { sizeof(mi) };
				if(GetWindowPlacement(a_handle, &g_wpPrev) && GetMonitorInfo(MonitorFromWindow(a_handle, MONITOR_DEFAULTTOPRIMARY), &mi)){
					// Set window to fullscreen
					SetWindowLong(a_handle, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
					SetWindowPos(a_handle, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
				}
				cfg.isFullscreen = true;
			} else {
				// Set window to windowed
				SetWindowLong(a_handle, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
				SetWindowPlacement(a_handle, &g_wpPrev);
				SetWindowPos(a_handle, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
				cfg.isFullscreen = false;
			}
		}
	}
}
