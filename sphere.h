#pragma once
#include "graphics.h"

class Sphere
{
public:
	struct Vertex;

	Sphere(float radius, unsigned tesselationLevel, D3D::GraphicDevice device, float freq);
	void Draw();
	void SetPositionMatrix(const D3DXMATRIX& positionMatrix);
	void SetViewMatrix(const D3DXMATRIX& viewMatrix);
	void SetProjectiveMatrix(const D3DXMATRIX& projectionMatrix);
	virtual ~Sphere();
private:
	Sphere(const Sphere&);
	Sphere& operator=(const Sphere&);

private:
	D3D::GraphicDevice device_;
	D3D::VertexDeclaration vertexDeclaration_;
	D3D::VertexBuffer<Vertex> vertexBuffer_;
	D3D::IndexBuffer indexBuffer_;
	D3D::Shader shader_;

	D3DXMATRIX positionMatrix_, viewMatrix_, projectiveMatrix_;
	unsigned nVertices_;
	unsigned nPrimitives_;
	const float radius_;
	const unsigned tesselationLevel_;
	const float freq_;
};

struct Sphere::Vertex
{
	float x, y, z;
	DWORD color;

	Vertex() {};
	Vertex( float x, float y, float z, DWORD color)
		:x(x), y(y), z(z), color(color) {}
};