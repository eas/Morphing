#pragma once

#include "windows.h"
#include "exception"
#include "d3dx9.h"

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

class SpectatorCoords
{
public:
	SpectatorCoords(float r, float theta, float fi)
		:r_(r), theta_(theta), fi_(fi)
	{
	}
	const D3DXVECTOR3 GetCartesianCoords() const
	{
		return D3DXVECTOR3(	r_*sinf(theta_)*cosf(fi_),
							r_*cosf(theta_),
							r_*sinf(theta_)*sinf(fi_) );
	}
	void IncTheta() { if((theta_+=deltaTheta) > thetaMax) theta_= thetaMax; }
	void DecTheta() { if((theta_-=deltaTheta) < thetaMin) theta_= thetaMin; }
	void IncFi() { fi_ += deltaFi; }
	void DecFi() { fi_ -= deltaFi; }
	void IncR() { r_ += deltaR; }
	void DecR() { if((r_-=deltaR) < rMin) r_ = rMin; }


private:
	float r_, theta_, fi_;

public:
	static const float deltaFi;
	static const float deltaTheta;
	static const float deltaR;
	static const float thetaMin;
	static const float thetaMax;
	static const float rMin;
};