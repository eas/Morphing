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

void CheckHwnd( HWND hWnd )
{
	if (!hWnd )
		throw WinApiError(GetLastError());
}