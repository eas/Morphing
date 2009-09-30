#pragma once
#include "graphics.h"
#include <vector>

typedef std::vector<D3D::Vertex> Vertices;
typedef std::vector<D3D::Index> Indices;

namespace FirstWay
{
	void InitVertices(	unsigned recursionDepth, float edgeSize,
						Vertices &pyramidVertices,
						Indices &pyramidIndices);
}

namespace SecondWay
{
	void InitVertices(	unsigned recursionDepth, float edgeSize,
						Vertices &pyramidVertices,
						Indices &pyramidIndices);
}