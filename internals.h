#pragma once

#include "windows.h"
#include "exception"

class WinApiError
	:public std::exception
{
public:
	WinApiError( DWORD errorCode )
		: errorCode_(errorCode)
	{
	}

public:
	DWORD errorCode_;
};

inline void CheckHwnd( HWND hWnd )
{
	if (!hWnd )
		throw WinApiError(GetLastError());
}

const int MaxLoadString = 100;
const int WindowHeight = 500;
const int WindowWidth = WindowHeight;
const int WindowPostionX = 350;
const int WindowPostionY = 120;

typedef LRESULT (CALLBACK *WndProcType)(HWND, UINT, WPARAM, LPARAM);

class Window
{
public:
	Window(	HINSTANCE hInstance, int nCmdShow, WndProcType wndProc);
	~Window();
	HWND GetHWND()
	{
		return hWnd_;
	}

private:
	Window(const Window&);
	Window& operator=(const Window&);

	HWND hWnd_;
};