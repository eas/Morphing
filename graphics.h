#pragma once

#include "windows.h"
#include "d3d9.h"
#include "exception"
#include <d3dx9.h>


namespace D3D
{
	class Error
		:public std::exception
	{
	public:
		Error(HRESULT errorCode);

		HRESULT errorCode_;
		char* error_;
	};

	struct Vertex
	{
		float x, y, z;
		DWORD color;
	};
	static D3DVERTEXELEMENT9 vertexDeclaration[] = 
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};

	inline void CheckResult(HRESULT errorCode)
	{
		if( errorCode != D3D_OK )
			throw Error(errorCode);
	}

	class GraphicDevice
	{
	public:
		class DC;
		GraphicDevice( HWND hWnd, D3DPRESENT_PARAMETERS& params );
		GraphicDevice( const GraphicDevice& other );

		~GraphicDevice();
		IDirect3DDevice9* GetDevice()
		{
			return device_;
		}
		IDirect3DDevice9* operator->()
		{
			return device_;
		}
		void SetRenderState(D3DRENDERSTATETYPE state, DWORD value)
		{
			CheckResult( device_->SetRenderState(state, value) );
		}
	private:
		IDirect3D9* directX_;
		IDirect3DDevice9* device_;
	};


	class GraphicDevice::DC
	{
	public:
		DC(GraphicDevice& device,
				DWORD flags, D3DCOLOR color, float z, DWORD stencil,
				DWORD count = NULL, const D3DRECT* rects = NULL )
				:device_(device)
		{
			//assert( (count == NULL && rects == NULL) || (count != NULL && rects != NULL) );
			CheckResult( device->Clear( count, rects, flags, color, z, stencil ) );
			CheckResult( device->BeginScene() );
		}
		~DC()
		{
			CheckResult( device_->EndScene() );
			CheckResult( device_->Present( NULL, NULL, NULL, NULL ) );
		}
		void DrawIndexedPrimitive( D3DPRIMITIVETYPE type,INT baseVertexIndex,UINT minIndex, 
									UINT numVertices, UINT startIndex, UINT primitiveCount )
		{
			CheckResult(device_->DrawIndexedPrimitive(type, baseVertexIndex, minIndex,
						numVertices, startIndex, primitiveCount));
		}
	private:
		DC(const DC&);
		DC& operator=(const DC&);

		GraphicDevice device_;
	};

	class Shader
	{
	public:
		Shader(GraphicDevice& device, LPCTSTR fileName);
		IDirect3DVertexShader9* GetShader()
		{
			return shader_;
		}
		void SetWorldMatrix( const D3DXMATRIX& worldMatrix );
		void SetProjectiveMatrix( const D3DXMATRIX& projectiveMatrix );
		void SetViewMatrix( const D3DXMATRIX& viewMatrix );
		void Use()
		{
			CheckResult( device_->SetVertexShader(shader_) );
		}
		~Shader();
	private:
		Shader(const Shader&);
		Shader& operator=(const Shader&);
	public:
		void SetShaderMatrix();
	private:


		IDirect3DVertexShader9* shader_;
		GraphicDevice device_;
		D3DXMATRIX worldMatrix_, viewMatrix_, projectiveMatrix_;
	};

	class VertexBuffer
	{
	public:
		VertexBuffer(GraphicDevice& device, UINT nVertices);
		~VertexBuffer();
		void SetVertices(Vertex vertices[], UINT nVertices);
		inline IDirect3DVertexBuffer9* GetBuffer()
		{
			return vertexBuffer_;
		}
		IDirect3DVertexBuffer9* operator->()
		{
			return vertexBuffer_;
		}
		void Use(UINT streamNumber, UINT offsetInBytes)
		{
			CheckResult( device_->SetStreamSource(streamNumber, vertexBuffer_, offsetInBytes, sizeof(Vertex)) );
		}
	private:
		VertexBuffer(const VertexBuffer&);
		VertexBuffer& operator=(const VertexBuffer&);

		IDirect3DVertexBuffer9* vertexBuffer_;
		GraphicDevice device_;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer(GraphicDevice& device, UINT nIndices);
		~IndexBuffer();
		void SetIndices(UINT indices[], UINT nIndices);
		inline IDirect3DIndexBuffer9* GetBuffer()
		{
			return indexBuffer_;
		}
		IDirect3DIndexBuffer9* operator->()
		{
			return indexBuffer_;
		}
		void Use()
		{
			CheckResult( device_->SetIndices(indexBuffer_) );
		}
	private:
		IndexBuffer(const IndexBuffer&);
		IndexBuffer& operator=(const IndexBuffer&);

		IDirect3DIndexBuffer9* indexBuffer_;
		GraphicDevice device_;
	};

	class VertexDeclaration
	{
	public:
		VertexDeclaration(GraphicDevice& device);
		~VertexDeclaration();
		IDirect3DVertexDeclaration9* GetDeclaration()
		{
			return vertexDeclaration_;
		}
		void Use()
		{
			CheckResult( device_->SetVertexDeclaration(vertexDeclaration_) );
		}
	private:
		VertexDeclaration(const VertexDeclaration&);
		VertexDeclaration& operator=(const VertexDeclaration&);

		IDirect3DVertexDeclaration9* vertexDeclaration_;
		GraphicDevice device_;
	};

} // namespace D3D