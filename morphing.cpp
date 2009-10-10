// morphing.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "morphing.h"

#include "internals.h"
#include "creating.h"
#include "colors.h"
#include "matrices.h"

#ifndef NDEBUG
	#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
const LPCTSTR ShaderFileName = L"shader.vsh";
const float FrontClippingPlane = 0.5f;
const float BackClippingPlane = 1.0e13f;
const float SphereRadius = 10.0f;
const float PyramidEdge = SphereRadius * sqrtf(2.0f);
const unsigned TessellationDepth = 5;

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void Render(D3D::GraphicDevice& device, Vertices &vertices,Indices &indices)
{
	D3D::GraphicDevice::DC dc( device, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Colors::Gray, 1.0f, 0 );
	dc.DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, vertices.size(), 0, indices.size()/3 );
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE ,
                     LPTSTR    ,
                     int       nCmdShow)
{
	Window mainWindow(hInstance, nCmdShow, &WndProc);

	D3DPRESENT_PARAMETERS params;
		ZeroMemory( &params, sizeof( params ) );

		params.Windowed = TRUE;
		params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		params.BackBufferFormat = D3DFMT_UNKNOWN;
		params.EnableAutoDepthStencil = TRUE;
		params.AutoDepthStencilFormat = D3DFMT_D16;
		params.MultiSampleType = D3DMULTISAMPLE_NONE;

	D3D::GraphicDevice graphicDevice( mainWindow.GetHWND(), params );
	graphicDevice.SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

	D3D::VertexDeclaration vertexDeclaration(graphicDevice);
	vertexDeclaration.Use();

	D3D::Shader shader(graphicDevice, ShaderFileName);
	shader.Use();
	shader.SetConstantF( 5, &SphereRadius, 1 );

	Vertices pyramidVertices;
	Indices indices;
	SecondWay::InitVertices(TessellationDepth, PyramidEdge, pyramidVertices, indices);
	//FirstWay::InitVertices(TessellationDepth, PyramidEdge, pyramidVertices, indices);

	D3D::VertexBuffer vertexBuffer(graphicDevice, pyramidVertices.size());
	vertexBuffer.SetVertices( &pyramidVertices[0], pyramidVertices.size() );
	vertexBuffer.Use(0, 0);

	D3D::IndexBuffer indexBuffer(graphicDevice, indices.size());
	indexBuffer.SetIndices(&indices[0], indices.size());
	indexBuffer.Use();

	SpectatorCoords spectatorCoords( 20.0f, D3DX_PI / 2, -D3DX_PI / 2 );

	shader.SetWorldMatrix( UnityMatrix() );
	shader.SetProjectiveMatrix( ProjectiveMatrix(FrontClippingPlane, BackClippingPlane) );
	shader.SetViewMatrix(ViewMatrix(	spectatorCoords.GetCartesianCoords(),
										D3DXVECTOR3(0.0f, 0.0f, 0.0f),
										D3DXVECTOR3(0.0f, 1.0f, 0.0f) ));

	float weight = 1.0f;
	int time = 0;

	SetWindowLong(mainWindow.GetHWND(), 0, reinterpret_cast<LONG>(&spectatorCoords));
	SetWindowLong(mainWindow.GetHWND(), sizeof(LONG), reinterpret_cast<LONG>(&shader));
	SetWindowLong(mainWindow.GetHWND(), sizeof(LONG)*2, reinterpret_cast<LONG>(&time) );
	SetWindowLong(mainWindow.GetHWND(), sizeof(LONG)*3, reinterpret_cast<LONG>(&weight) );

	SetTimer(mainWindow.GetHWND(), NULL, 100, NULL );

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
			Render(graphicDevice, pyramidVertices, indices);
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
			D3D::Shader* pShader = NULL;
			SpectatorCoords* pSpectatorCoords = NULL;
			pSpectatorCoords = reinterpret_cast<SpectatorCoords*>(GetWindowLong(hWnd, 0));
			pShader = reinterpret_cast<D3D::Shader*>(GetWindowLong(hWnd, sizeof(LONG)));

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
			pShader->SetViewMatrix(ViewMatrix(	pSpectatorCoords->GetCartesianCoords(),
												D3DXVECTOR3(0.0f, 0.0f, 0.0f),
												D3DXVECTOR3(0.0f, 1.0f, 0.0f) ));
			break;
		}
	case WM_TIMER:
		{
			int *time = NULL;
			float *weight = NULL;
			D3D::Shader *shader = NULL;
			time = reinterpret_cast<int*>(GetWindowLong(hWnd, 2*sizeof(LONG)));
			weight = reinterpret_cast<float*>(GetWindowLong(hWnd, 3*sizeof(LONG)));
			shader = reinterpret_cast<D3D::Shader*>(GetWindowLong(hWnd, sizeof(LONG)));
			++*time;
			*weight = -(-(*time % 100) + 50)*(*time % 100 - 50) / 2500.0f;
			shader->SetConstantF(4, weight, 1);
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

