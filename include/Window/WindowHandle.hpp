#pragma once

#ifndef NTH_WINDOWHANDLE_HPP
#define NTH_WINDOWHANDLE_HPP

namespace Nth {
	enum class WindowProtocol {
		X11,
		Windows,
		Unknow
	};

	struct X11Handle {
		void * dpy; // Display
		unsigned long window; // Window
	};

	struct Win32Handle {
		void * hinstance; // HINSTANCE
		void * hwnd; // HWND
	};

	struct WindowHandle{
		WindowProtocol protocol = WindowProtocol::Unknow;

		union{
			X11Handle x11;
			Win32Handle windows;
		};
	};
}

#endif