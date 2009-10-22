#include "stdafx.h"
#include "sphere.h"
#include <vector>
#include <algorithm>
#include "colors.h"
#include <time.h>
#include "matrices.h"

#ifndef NDEBUG
	#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
using namespace D3D;

typedef Sphere::Vertex Vertex;
typedef std::vector<Vertex> Vertices;
typedef std::vector<D3D::Index> Indices;

static const D3DVERTEXELEMENT9 DefaultVertexDeclaration[] = {
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END() };

class Node;
void Tessellation(	Node& n1, Node& n2, Node& n3, Vertices& vertices, Indices& indices,
					unsigned curDepth, unsigned maxDepth );
void InitVertices(	unsigned recursionDepth, float edgeSize,
					Vertices &pyramidVertices,
					Indices &pyramidIndices);
Node* GetOrCreateChild(Node& parent1, Node& parent2, Vertices& vertices);
Vertex SumWithWeight( const Vertex &v1, const Vertex &v2, float weight );


Sphere::Sphere(float radius, unsigned tesselationLevel, D3D::GraphicDevice device, float freq)
	: device_(device),
	  vertexDeclaration_(device, DefaultVertexDeclaration),
	  vertexBuffer_(device),
	  indexBuffer_(device),
	  shader_(device, L"sphere.vsh"),
	  radius_(radius),
	  tesselationLevel_(tesselationLevel),
	  freq_(freq)
{
	Vertices vertices;
	Indices indices;

	InitVertices(tesselationLevel, radius*sqrtf(2), vertices, indices);
	nVertices_ = vertices.size();
	nPrimitives_ = indices.size()/3;
	vertexBuffer_.SetVertices( &vertices[0], vertices.size() );
	indexBuffer_.SetIndices( &indices[0], indices.size() );
	SetPositionMatrix( UnityMatrix() );
	SetViewMatrix( UnityMatrix() );
	SetProjectiveMatrix( UnityMatrix() );
}
Sphere::~Sphere()
{
}
void Sphere::Draw()
{
	float time = static_cast<float>(clock()) / CLOCKS_PER_SEC;
	float weight = (sinf(freq_*time * (2*D3DX_PI)) + 1)/2;
	vertexBuffer_.Use(0,0);
	indexBuffer_.Use();
	vertexDeclaration_.Use();
	shader_.Use();
	shader_.SetMatrix( projectiveMatrix_*viewMatrix_*positionMatrix_, 0 );
	shader_.SetConstantF(4, radius_);
	shader_.SetConstantF(5, weight);
	vertexDeclaration_.Use();

	device_->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertices_, 0, nPrimitives_);
}
void Sphere::SetPositionMatrix(const D3DXMATRIX& positionMatrix)
{
	positionMatrix_ = positionMatrix;
}
void Sphere::SetViewMatrix(const D3DXMATRIX& viewMatrix)
{
	viewMatrix_ = viewMatrix;
}
void Sphere::SetProjectiveMatrix(const D3DXMATRIX& projectiveMatrix)
{
	projectiveMatrix_ = projectiveMatrix;
}

class Node
{
	friend Node* GetOrCreateChild(Node&, Node&, Vertices&);
public:
	Node(Node* parent1, Node* parent2, Index index);
	~Node();
	Node* AddChild(Node* node);
	bool IsParentOf(const Node* node);
	bool operator==(const Node& other) const;
	Index GetIndex() const;
private:
	typedef std::vector<Node*> Childs;

	Node *parent1_, *parent2_;
	Childs childs_;
	Index index_;
};
Node::Node(Node* parent1, Node* parent2, Index index)
	: parent1_(parent1), parent2_(parent2), index_(index)
{
}
Node::~Node()
{
	for( Childs::iterator i=childs_.begin(); i!=childs_.end(); ++i )
	{
		if( (*i)->parent1_ == this )
		{
			(*i)->parent1_ = NULL;
		}
		if( (*i)->parent2_ == this )
		{
			(*i)->parent2_ = NULL;
		}
		if( (*i)->parent1_ == NULL && (*i)->parent2_ == NULL )
		{
			delete *i;
		}
	}
}
Node* Node::AddChild(Node* node)
{
	childs_.push_back(node);
	return node;
}
bool Node::IsParentOf(const Node *node)
{
	return (std::find(childs_.begin(), childs_.end(), node) != childs_.end());
}
bool Node::operator==(const Node& other) const
{
	return index_ == other.index_;
}
Index Node::GetIndex() const
{
	return index_;
}
Node* GetOrCreateChild(Node& parent1, Node& parent2, Vertices& vertices)
{
	for( Node::Childs::const_iterator i=parent1.childs_.begin(); i!=parent1.childs_.end(); ++i )
	{
		if( parent2.IsParentOf( *i ) )
		{
			return *i;
		}
	}
	vertices.push_back( SumWithWeight(	vertices[parent1.GetIndex()],
										vertices[parent2.GetIndex()],
										0.5f ));
	Node* node = new Node(&parent1, &parent2, vertices.size()-1);
	parent1.AddChild(node);
	parent2.AddChild(node);
	return node;

}

void Tessellation(	Node& n1, Node& n2, Node& n3, Vertices& vertices, Indices& indices,
					unsigned curDepth, unsigned maxDepth )
{
	if( curDepth<maxDepth )
	{
		Node *n12 = GetOrCreateChild(n1, n2, vertices);
		Node *n23 = GetOrCreateChild(n2, n3, vertices);
		Node *n31 = GetOrCreateChild(n3, n1, vertices);
		Tessellation( n1, *n12, *n31, vertices, indices, curDepth+1, maxDepth );
		Tessellation( n2, *n23, *n12, vertices, indices, curDepth+1, maxDepth );
		Tessellation( n3, *n31, *n23, vertices, indices, curDepth+1, maxDepth );
		Tessellation( *n12, *n23, *n31, vertices, indices, curDepth+1, maxDepth );

	}
	else
	{
		indices.push_back( n1.GetIndex() );
		indices.push_back( n2.GetIndex() );
		indices.push_back( n3.GetIndex() );
	}
}

void InitVertices(	unsigned recursionDepth, float edgeSize,
					Vertices &pyramidVertices,
					Indices &pyramidIndices)
{
	std::vector<Node> nodes;

	pyramidVertices.push_back( Vertex( -edgeSize/2, 0.0f, -edgeSize/ 2, Colors::Red ) );
	pyramidVertices.push_back( Vertex(  edgeSize/2, 0.0f, -edgeSize/ 2, Colors::Green ) );
	pyramidVertices.push_back( Vertex(  edgeSize/2, 0.0f,  edgeSize/ 2, Colors::Cyan ) );
	pyramidVertices.push_back( Vertex( -edgeSize/2, 0.0f,  edgeSize/ 2, Colors::Magenta ) );
	pyramidVertices.push_back( Vertex( 0.0f,  edgeSize*sqrtf(2.0f)/2, 0.0f, Colors::White ) );
	pyramidVertices.push_back( Vertex( 0.0f, -edgeSize*sqrtf(2.0f)/2, 0.0f, Colors::Black ) );
	
	for( Index index=0; index<6; ++index )
	{
		nodes.push_back( Node(NULL, NULL, index) );
	}

	Tessellation( nodes[0], nodes[4], nodes[1], pyramidVertices, pyramidIndices, 0, recursionDepth );
	Tessellation( nodes[1], nodes[4], nodes[2], pyramidVertices, pyramidIndices, 0, recursionDepth );
	Tessellation( nodes[2], nodes[4], nodes[3], pyramidVertices, pyramidIndices, 0, recursionDepth );
	Tessellation( nodes[3], nodes[4], nodes[0], pyramidVertices, pyramidIndices, 0, recursionDepth );

	Tessellation( nodes[0], nodes[1], nodes[5], pyramidVertices, pyramidIndices, 0, recursionDepth );
	Tessellation( nodes[1], nodes[2], nodes[5], pyramidVertices, pyramidIndices, 0, recursionDepth );
	Tessellation( nodes[2], nodes[3], nodes[5], pyramidVertices, pyramidIndices, 0, recursionDepth );
	Tessellation( nodes[3], nodes[0], nodes[5], pyramidVertices, pyramidIndices, 0, recursionDepth );
		
}

Vertex SumWithWeight( const Vertex &v1, const Vertex &v2, float weight )
{
	DWORD r, g, b, a;
	DWORD color1 = v1.color;
	DWORD color2 = v2.color;
	a = static_cast<DWORD>( (color1 & 0xff000000)/0x1000000 * weight +
		(color2 & 0xff000000)/0x1000000 * (1-weight) );
	r = static_cast<DWORD>( (color1 & 0x00ff0000)/0x10000 * weight +
		(color2 & 0x00ff0000)/0x10000 * (1-weight) );
	g = static_cast<DWORD>( (color1 & 0x0000ff00)/0x100 * weight +
		(color2 & 0x0000ff00)/0x100 * (1-weight) );
	b = static_cast<DWORD>( (color1 & 0x000000ff) * weight +
		(color2 & 0x000000ff) * (1-weight) );

	return Vertex(	(v1.x*weight+v2.x*(1-weight)),
					(v1.y*weight+v2.y*(1-weight)),
					(v1.z*weight+v2.z*(1-weight)),
#ifndef RANDOM_COLORS
					D3DCOLOR_ARGB(a, r, g, b) );
#else
					Colors::Random() );
#endif
}
