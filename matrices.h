#pragma once
#include "d3dx9.h"

inline const D3DXMATRIX UnityMatrix()
{
	return D3DXMATRIX(	1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f );
}
inline const D3DXMATRIX ScaleMatrix( const D3DXVECTOR3& scale )
{
	return D3DXMATRIX( scale.x,		0.0f,		0.0f,		0.0f,
					   0.0f,		scale.y,	0.0f,		0.0f,
					   0.0f,		0.0f,		scale.z,	0.0f,
					   0.0f,		0.0f,		0.0f,		1.0f );
}
inline const D3DXMATRIX ScaleMatrix( float scaleX, float scaleY, float scaleZ )
{
	return D3DXMATRIX( scaleX,		0.0f,		0.0f,		0.0f,
					   0.0f,		scaleY,		0.0f,		0.0f,
					   0.0f,		0.0f,		scaleZ,		0.0f,
					   0.0f,		0.0f,		0.0f,		1.0f );
}

inline const D3DXMATRIX TranslationMatrix( const D3DXVECTOR3& translation )
{
	return D3DXMATRIX( 1.0f,	0.0f,	0.0f,	translation.x,
					   0.0f,	1.0f,	0.0f,	translation.y,
					   0.0f,	0.0f,	1.0f,	translation.z,
					   0.0f,	0.0f,	0.0f,	1.0f );
}
inline const D3DXMATRIX TranslationMatrix( float dx, float dy, float dz )
{
	return D3DXMATRIX( 1.0f,	0.0f,	0.0f,	dx,
					   0.0f,	1.0f,	0.0f,	dy,
					   0.0f,	0.0f,	1.0f,	dz,
					   0.0f,	0.0f,	0.0f,	1.0f );
}

inline const D3DXMATRIX RotateZMatrix( float angle )
{
	return D3DXMATRIX(  cosf(angle),	sinf(angle),	0.0f,		0.0f,
					   -sinf(angle),	cosf(angle),	0.0f,		0.0f,
						0.0f,			0.0f,			1.0f,		0.0f,
						0.0f,			0.0f,			0.0f,		1.0f );
}

inline const D3DXMATRIX RotateYMatrix( float angle )
{
	return D3DXMATRIX(  cosf(angle),	0.0f,		-sinf(angle),	0.0f,
						0.0f,			1.0f,		0.0f,			0.0f,
						sinf(angle),	0.0f,		cosf(angle),	0.0f,
						0.0f,			0.0f,		0.0f,			1.0f );
}

inline const D3DXMATRIX RotateXMatrix( float angle )
{
	return D3DXMATRIX(  1.0f,		0.0f,			0.0f,			0.0f,
						0.0f,		cosf(angle),	sinf(angle),	0.0f,
						0.0f,		-sinf(angle),	cosf(angle),	0.0f,
						0.0f,		0.0f,			0.0f,			1.0f );
}

const D3DXMATRIX ViewMatrix( const D3DXVECTOR3& eye, const D3DXVECTOR3& at, const D3DXVECTOR3& up )
{
	D3DXVECTOR3 c, _c = at-eye;
	D3DXVec3Normalize( &c, &_c );

	D3DXVECTOR3 a, _a;
	D3DXVec3Cross( &_a, &up, &c );
	D3DXVec3Normalize( &a, &_a );

	D3DXVECTOR3 b;
	D3DXVec3Cross( &b, &c, &a );
	c = c / 2.0f;

	D3DXVECTOR3 d(	-D3DXVec3Dot(&eye, &a),
					-D3DXVec3Dot(&eye, &b),
					-D3DXVec3Dot(&eye, &c) );

	D3DXMATRIX temp =  D3DXMATRIX(	a.x,	a.y,	a.z,	d.x,
						b.x,	b.y,	b.z,	d.y,
						c.x,	c.y,	c.z,	d.z,
						0.0f,	0.0f,	0.0f,	1.0f );
	return temp;
}

const D3DXMATRIX ProjectiveMatrix( float front, float back )
{
	float a = back / (back - front);
	float b = back * front / (front - back);
	return D3DXMATRIX(	front,	0.0f,	0.0f,	0.0f,
						0.0f,	front,	0.0f,	0.0f,
						0.0f,	0.0f,	a,		b,
						0.0f,	0.0f,	1.0f,	0.0f );
}
