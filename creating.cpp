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

VertexWithIndex HalfSum( VertexWithIndex v1, VertexWithIndex v2, Index newIndex )
{
	return VertexWithIndex(	Vertex(	(v1.vertex.x+v2.vertex.x)/2,
									(v1.vertex.y+v2.vertex.y)/2,
									(v1.vertex.z+v2.vertex.z)/2,
									(v1.vertex.color/2+v2.vertex.color/2) ),
							newIndex );
}

void Tessellation(	VertexWithIndex v1,VertexWithIndex v2,VertexWithIndex v3,
					std::vector<Vertex> &pyramidVertices,
					std::vector<Index> pyramidIndices,
					unsigned curDepth, unsigned maxDepth, Index &nVertices )
{
	if( curDepth < maxDepth )
	{
		VertexWithIndex newV1 = HalfSum( v1, v3, ++nVertices );
		pyramidVertices.push_back( newV1.vertex );

		VertexWithIndex newV2 = HalfSum( v1, v2, ++nVertices );
		pyramidVertices.push_back( newV2.vertex );

		VertexWithIndex newV3 = HalfSum( v2, v3, ++nVertices );
		pyramidVertices.push_back( newV3.vertex );

		Tessellation(	v1, newV1, newV2,
						pyramidVertices, pyramidIndices, curDepth-1, maxDepth, nVertices );

		Tessellation(	v1, newV3, newV1,
						pyramidVertices, pyramidIndices, curDepth-1, maxDepth, nVertices );

		Tessellation(	v2, newV2, newV3,
						pyramidVertices, pyramidIndices, curDepth-1, maxDepth, nVertices );

		Tessellation(	newV1, newV3, newV2,
						pyramidVertices, pyramidIndices, curDepth-1, maxDepth, nVertices );
	}
	else
	{
		pyramidIndices.push_back(v1.index);
		pyramidIndices.push_back(v2.index);
		pyramidIndices.push_back(v3.index);
	}
}
void InitVertices(	unsigned recursionDepth, float edgeSize,
					std::vector<Vertex> &pyramidVertices,
					std::vector<Index> &pyramidIndices)
{
	pyramidVertices.erase( pyramidVertices.begin(), pyramidVertices.end() );
	pyramidIndices.erase( pyramidIndices.begin(), pyramidIndices.end() );

	std::vector<VertexWithIndex> initialVertices;

	initialVertices.push_back( VertexWithIndex(Vertex( -edgeSize/2, 0.0f, -edgeSize/ 2, Red ), 0) );
	initialVertices.push_back( VertexWithIndex(Vertex(  edgeSize/2, 0.0f, -edgeSize/ 2, Red ), 1) );
	initialVertices.push_back( VertexWithIndex(Vertex(  edgeSize/2, 0.0f,  edgeSize/ 2, Red ), 2) );
	initialVertices.push_back( VertexWithIndex(Vertex( -edgeSize/2, 0.0f,  edgeSize/ 2, Red ), 3) );
	initialVertices.push_back( VertexWithIndex(Vertex( 0.0f,  edgeSize*sqrtf(1 - sqrtf(2)/2), 0.0f, Red ), 4) );
	initialVertices.push_back( VertexWithIndex(Vertex( 0.0f, -edgeSize*sqrtf(1 - sqrtf(2)/2), 0.0f, Red ), 5) );


	for( unsigned i=0; i<=initialVertices.size(); ++i )
		pyramidVertices.push_back( initialVertices[i].vertex );

	Index nVertices = initialVertices.size();

	Tessellation(	initialVertices[0], initialVertices[4], initialVertices[1],
					pyramidVertices, pyramidIndices,
					0, recursionDepth, nVertices );
	Tessellation(	initialVertices[1], initialVertices[4], initialVertices[2],
					pyramidVertices, pyramidIndices,
					0, recursionDepth, nVertices );
	Tessellation(	initialVertices[2], initialVertices[4], initialVertices[3],
					pyramidVertices, pyramidIndices,
					0, recursionDepth, nVertices );
	Tessellation(	initialVertices[3], initialVertices[4], initialVertices[0],
					pyramidVertices, pyramidIndices,
					0, recursionDepth, nVertices );

	Tessellation(	initialVertices[0], initialVertices[1], initialVertices[5],
					pyramidVertices, pyramidIndices,
					0, recursionDepth, nVertices );
	Tessellation(	initialVertices[1], initialVertices[2], initialVertices[5],
					pyramidVertices, pyramidIndices,
					0, recursionDepth, nVertices );
	Tessellation(	initialVertices[2], initialVertices[3], initialVertices[5],
					pyramidVertices, pyramidIndices,
					0, recursionDepth, nVertices );
	Tessellation(	initialVertices[3], initialVertices[0], initialVertices[5],
					pyramidVertices, pyramidIndices,
					0, recursionDepth, nVertices );
}