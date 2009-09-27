#include "stdafx.h"

#include "creating.h"


#include "graphics.h"
#include "colors.h"

#include <vector>

using namespace D3D;

struct VertexWithIndex
{
	Vertex vertex;
	Index index;

	VertexWithIndex() {}
	VertexWithIndex( const Vertex& vertex, const Index index )
		:vertex(vertex), index(index) {}
};

Vertex HalfSum( Vertices vertices, Index i1, Index i2 )
{
	DWORD r, g, b, a;
	DWORD color1 = vertices[i1].color;
	DWORD color2 = vertices[i2].color;
	a = ((color1 & 0xff000000)/0x1000000 + (color2 & 0xff000000)/0x1000000) / 2;
	r = ((color1 & 0x00ff0000)/0x10000 + (color2 & 0x00ff0000)/0x10000) / 2;
	g = ((color1 & 0x0000ff00)/0x100 + (color2 & 0x0000ff00)/0x100) / 2;
	b = ((color1 & 0x000000ff) + (color2 & 0x000000ff)) / 2;
	return Vertex(	(vertices[i1].x+vertices[i2].x)/2,
					(vertices[i1].y+vertices[i2].y)/2,
					(vertices[i1].z+vertices[i2].z)/2,
					D3DCOLOR_ARGB(a, r, g, b) );
}

void Tessellation(	Index i1,Index i2,Index i3,
					Vertices &pyramidVertices,
					Indices &pyramidIndices,
					unsigned curDepth, unsigned maxDepth )
{
	if( curDepth < maxDepth )
	{
		Index newI = static_cast<Index>(pyramidVertices.size()-1);

		pyramidVertices.push_back( HalfSum(pyramidVertices, i1, i3) );
		pyramidVertices.push_back( HalfSum(pyramidVertices, i1, i2) );
		pyramidVertices.push_back( HalfSum(pyramidVertices, i2, i3) );


		Tessellation(	i1, newI + 2, newI + 1,
						pyramidVertices, pyramidIndices, curDepth+1, maxDepth );

		Tessellation(	i2, newI + 3, newI + 2,
						pyramidVertices, pyramidIndices, curDepth+1, maxDepth );

		Tessellation(	i3, newI + 1, newI + 3,
						pyramidVertices, pyramidIndices, curDepth+1, maxDepth );

		Tessellation(	newI + 1, newI + 2, newI + 3,
						pyramidVertices, pyramidIndices, curDepth+1, maxDepth );
	}
	else
	{
#ifndef TRIANGLES
		pyramidIndices.push_back(i1);
		pyramidIndices.push_back(i2);
		pyramidIndices.push_back(i2);
		pyramidIndices.push_back(i3);
		pyramidIndices.push_back(i3);
		pyramidIndices.push_back(i1);
#else
		pyramidIndices.push_back(i1);
		pyramidIndices.push_back(i2);
		pyramidIndices.push_back(i3);
#endif
	}
}
void InitVertices(	unsigned recursionDepth, float edgeSize,
					std::vector<Vertex> &pyramidVertices,
					std::vector<Index> &pyramidIndices)
{
	pyramidVertices.erase( pyramidVertices.begin(), pyramidVertices.end() );
	pyramidIndices.erase( pyramidIndices.begin(), pyramidIndices.end() );

	std::vector<VertexWithIndex> initialVertices;

	pyramidVertices.push_back( Vertex( -edgeSize/2, 0.0f, -edgeSize/ 2, Red ) );
	pyramidVertices.push_back( Vertex(  edgeSize/2, 0.0f, -edgeSize/ 2, Green ) );
	pyramidVertices.push_back( Vertex(  edgeSize/2, 0.0f,  edgeSize/ 2, Cyan ) );
	pyramidVertices.push_back( Vertex( -edgeSize/2, 0.0f,  edgeSize/ 2, Magenta ) );
	pyramidVertices.push_back( Vertex( 0.0f,  edgeSize*sqrtf(2.0f)/2, 0.0f, White ) );
	pyramidVertices.push_back( Vertex( 0.0f, -edgeSize*sqrtf(2.0f)/2, 0.0f, Black ) );


	Tessellation(	0, 4, 1,
					pyramidVertices, pyramidIndices,
					1, recursionDepth );
	Tessellation(	1, 4, 2,
					pyramidVertices, pyramidIndices,
					1, recursionDepth );
	Tessellation(	2, 4, 3,
					pyramidVertices, pyramidIndices,
					1, recursionDepth );
	Tessellation(	3, 4, 0,
					pyramidVertices, pyramidIndices,
					1, recursionDepth );

	Tessellation(	0, 1, 5,
					pyramidVertices, pyramidIndices,
					1, recursionDepth );
	Tessellation(	1, 2, 5,
					pyramidVertices, pyramidIndices,
					1, recursionDepth );
	Tessellation(	2, 3, 5,
					pyramidVertices, pyramidIndices,
					1, recursionDepth );
	Tessellation(	3, 0, 5,
					pyramidVertices, pyramidIndices,
					1, recursionDepth );
}