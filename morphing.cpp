// morphing.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "morphing.h"

#include "helper.h"
#include "graphics.h"
#include "colors.h"
#include "matrices.h"
#include "sphere.h"

#ifndef NDEBUG
	#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
const LPCTSTR ShaderFileName = L"shader.vsh";
const float FrontClippingPlane = 0.5f;
const float BackClippingPlane = 1.0e13f;
const float SphereRadius = 10.0f;
const float PyramidEdge = SphereRadius * sqrtf(2.0f);
const unsigned TessellationDepth = 5;
const float Freq = 1.0f;

static const D3DVERTEXELEMENT9 VertexDeclaration[] = {
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END() };

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void Render(D3D::GraphicDevice& device, Sphere& sphere)
{
	D3D::GraphicDevice::DC dc( device, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Colors::Gray, 1.0f, 0 );
	sphere.Draw();
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE ,
                     LPTSTR    ,
                     int       nCmdShow)
{
	Helper::Window mainWindow(hInstance, nCmdShow, &WndProc, L"Morphing", L"morphing", 2);

	D3DPRESENT_PARAMETERS params;
		ZeroMemory( &params, sizeof( params ) );

		params.Windowed = TRUE;
		params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		params.BackBufferFormat = D3DFMT_UNKNOWN;
		params.EnableAutoDepthStencil = TRUE;
		params.AutoDepthStencilFormat = D3DFMT_D16;
		params.MultiSampleType = D3DMULTISAMPLE_NONE;

	D3D::GraphicDevice graphicDevice( mainWindow.GetHWND(), params );
	//graphicDevice.SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	Sphere sphere(SphereRadius, TessellationDepth, graphicDevice, Freq);
	
	Helper::SpectatorCoords spectatorCoords( 20.0f, D3DX_PI / 2, -D3DX_PI / 2 );

	D3DXMATRIX worldMatrix = UnityMatrix();
	D3DXMATRIX projectiveMatrix = ProjectiveMatrix(FrontClippingPlane, BackClippingPlane);
	D3DXMATRIX viewMatrix =  ViewMatrix(spectatorCoords.GetCartesianCoords(),
										D3DXVECTOR3(0.0f, 0.0f, 0.0f),
										D3DXVECTOR3(0.0f, 1.0f, 0.0f) );

	sphere.SetPositionMatrix( worldMatrix );
	sphere.SetProjectiveMatrix( projectiveMatrix );
	sphere.SetViewMatrix( viewMatrix );
	SetWindowLong(mainWindow.GetHWND(), 0, reinterpret_cast<LONG>(&spectatorCoords));
	SetWindowLong(mainWindow.GetHWND(), sizeof(LONG), reinterpret_cast<LONG>(&sphere));

	MSG msg;

	ZeroMemory(&msg, sizeof(msg));
    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
		{
			Render(graphicDevice, sphere);
		}
    }

	_CrtDumpMemoryLeaks();

	return (int) msg.wParam;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		{
			Sphere* sphere = NULL;
			Helper::SpectatorCoords* pSpectatorCoords = NULL;
			pSpectatorCoords = reinterpret_cast<Helper::SpectatorCoords*>(GetWindowLong(hWnd, 0));
			sphere = reinterpret_cast<Sphere*>(GetWindowLong(hWnd, sizeof(LONG)));

			switch(wParam)
			{
			case VK_UP:
				pSpectatorCoords->DecTheta();
				break;
			case VK_DOWN:
				pSpectatorCoords->IncTheta();
				break;
			case VK_RIGHT:
				pSpectatorCoords->IncPhi();
				break;
			case VK_LEFT:
				pSpectatorCoords->DecPhi();
				break;
			case VK_NEXT:
			case 'S':
				pSpectatorCoords->IncR();
				break;
			case VK_PRIOR:
			case 'W':
				pSpectatorCoords->DecR();
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
			sphere->SetViewMatrix(ViewMatrix(	pSpectatorCoords->GetCartesianCoords(),
												D3DXVECTOR3(0.0f, 0.0f, 0.0f),
												D3DXVECTOR3(0.0f, 1.0f, 0.0f) ));
			break;
		}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

