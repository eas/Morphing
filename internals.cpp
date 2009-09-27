#include "stdafx.h"
#include "internals.h"
#include "Resource.h"

ATOM MyRegisterClass(HINSTANCE hInstance, WndProcType wndProc, TCHAR* , TCHAR szWindowClass[])
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= wndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(LONG) * 4;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MORPHING));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

Window::Window(	HINSTANCE hInstance, int nCmdShow, WndProcType wndProc)
{
	TCHAR szTitle[MaxLoadString];
	TCHAR szWindowClass[MaxLoadString];				// the main window class name

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MaxLoadString);
	LoadString(hInstance, IDC_MORPHING, szWindowClass, MaxLoadString);
	MyRegisterClass(hInstance, wndProc, szTitle, szWindowClass);

	hWnd_ = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		WindowPostionX, WindowPostionY, WindowWidth, WindowHeight, NULL, NULL, hInstance, NULL);

	if (!hWnd_)
		throw WinApiError( GetLastError() );

	ShowWindow(hWnd_, nCmdShow);
	UpdateWindow(hWnd_);
}
Window::~Window()
{
}

const float SpectatorCoords::deltaFi = D3DX_PI / 24;
const float SpectatorCoords::deltaTheta = D3DX_PI / 24;
const float SpectatorCoords::deltaR = 2.5f;
const float SpectatorCoords::thetaMin = 1e-3f;
const float SpectatorCoords::thetaMax = D3DX_PI - thetaMin;
const float SpectatorCoords::rMin = 0.1f;
