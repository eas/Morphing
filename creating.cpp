#include "stdafx.h"

#include "creating.h"


#include "graphics.h"
#include "colors.h"

#include <vector>
#include <assert.h>
#include <algorithm>
#ifndef NDEBUG
	#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif
#define RANDOM_COLORS
using namespace D3D;

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



namespace FirstWay
{
	inline Vertex HalfSum( const Vertices &vertices, Index i1, Index i2 )
	{
		return SumWithWeight( vertices[i1], vertices[i2], 0.5f );
	}

	void Tessellation(	Index i1,Index i2,Index i3,
						Vertices &vertices,
						Indices &indices,
						unsigned curDepth, unsigned maxDepth )
	{
		if( curDepth < maxDepth )
		{
			Index newI = static_cast<Index>(vertices.size()-1);

			vertices.push_back( HalfSum(vertices, i1, i3) );
			vertices.push_back( HalfSum(vertices, i1, i2) );
			vertices.push_back( HalfSum(vertices, i2, i3) );


			Tessellation(	i1, newI + 2, newI + 1,
							vertices, indices, curDepth+1, maxDepth );

			Tessellation(	i2, newI + 3, newI + 2,
							vertices, indices, curDepth+1, maxDepth );

			Tessellation(	i3, newI + 1, newI + 3,
							vertices, indices, curDepth+1, maxDepth );

			Tessellation(	newI + 1, newI + 2, newI + 3,
							vertices, indices, curDepth+1, maxDepth );
		}
		else
		{

			indices.push_back(i1);
			indices.push_back(i2);
			indices.push_back(i3);
		}
	}
	void InitVertices(	unsigned recursionDepth, float edgeSize,
						Vertices &pyramidVertices,
						Indices &pyramidIndices)
	{
		pyramidVertices.erase( pyramidVertices.begin(), pyramidVertices.end() );
		pyramidIndices.erase( pyramidIndices.begin(), pyramidIndices.end() );
#ifndef RANDOM_COLORS
		pyramidVertices.push_back( Vertex( -edgeSize/2, 0.0f, -edgeSize/ 2, Colors::Red ) );
		pyramidVertices.push_back( Vertex(  edgeSize/2, 0.0f, -edgeSize/ 2, Colors::Green ) );
		pyramidVertices.push_back( Vertex(  edgeSize/2, 0.0f,  edgeSize/ 2, Colors::Cyan ) );
		pyramidVertices.push_back( Vertex( -edgeSize/2, 0.0f,  edgeSize/ 2, Colors::Magenta ) );
		pyramidVertices.push_back( Vertex( 0.0f,  edgeSize*sqrtf(2.0f)/2, 0.0f, Colors::White ) );
		pyramidVertices.push_back( Vertex( 0.0f, -edgeSize*sqrtf(2.0f)/2, 0.0f, Colors::Black ) );
#else
		pyramidVertices.push_back( Vertex( -edgeSize/2, 0.0f, -edgeSize/ 2, Colors::Random() ) );
		pyramidVertices.push_back( Vertex(  edgeSize/2, 0.0f, -edgeSize/ 2, Colors::Random() ) );
		pyramidVertices.push_back( Vertex(  edgeSize/2, 0.0f,  edgeSize/ 2, Colors::Random() ) );
		pyramidVertices.push_back( Vertex( -edgeSize/2, 0.0f,  edgeSize/ 2, Colors::Random() ) );
		pyramidVertices.push_back( Vertex( 0.0f,  edgeSize*sqrtf(2.0f)/2, 0.0f, Colors::Random() ) );
		pyramidVertices.push_back( Vertex( 0.0f, -edgeSize*sqrtf(2.0f)/2, 0.0f, Colors::Random() ) );
#endif

		Tessellation(	0, 4, 1,
						pyramidVertices, pyramidIndices,
						0, recursionDepth );
		Tessellation(	1, 4, 2,
						pyramidVertices, pyramidIndices,
						0, recursionDepth );
		Tessellation(	2, 4, 3,
						pyramidVertices, pyramidIndices,
						0, recursionDepth );
		Tessellation(	3, 4, 0,
						pyramidVertices, pyramidIndices,
						0, recursionDepth );

		Tessellation(	0, 1, 5,
						pyramidVertices, pyramidIndices,
						0, recursionDepth );
		Tessellation(	1, 2, 5,
						pyramidVertices, pyramidIndices,
						0, recursionDepth );
		Tessellation(	2, 3, 5,
						pyramidVertices, pyramidIndices,
						0, recursionDepth );
		Tessellation(	3, 0, 5,
						pyramidVertices, pyramidIndices,
						0, recursionDepth );
	}
} // namespace FirstWay

namespace SecondWay
{
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
		pyramidVertices.erase( pyramidVertices.begin(), pyramidVertices.end() );
		pyramidIndices.erase( pyramidIndices.begin(), pyramidIndices.end() );
	
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

} // namespace SecondWay