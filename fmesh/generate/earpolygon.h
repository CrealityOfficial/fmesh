#ifndef FMESH_EARPOLYGON_1604488905381_H
#define FMESH_EARPOLYGON_1604488905381_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/common/export.h"
#include "fmesh/generate/patch.h"

#include "mmesh/trimesh/polygonstack.h"

//namespace mmesh
//{
//	class PolygonStack;
//}
//
//namespace trimesh {
//	class TriMesh;
//}

namespace fmesh
{
	typedef std::vector<ClipperLibXYZ::IntPoint*> SimplePoly;
	struct ENode
	{
		ENode* prev;
		ENode* next;

		ClipperLibXYZ::IntPoint* vertex;
		int type;  // 0 colliear 1 concave 2 convex 3 ear
		double dot;
	};

	class FMESH_API SimplePolyWrapper
	{
	public:
		SimplePolyWrapper();
		~SimplePolyWrapper();

		void load(const std::string& file);

		std::vector<ClipperLibXYZ::IntPoint> source;
		SimplePoly poly;
	};

	void saveSimplePoly(const SimplePoly& poly, const std::string& file);

	class FMESH_API EarPolygon
	{
	public:
		EarPolygon();
		~EarPolygon();

		void setup(SimplePoly* poly);
		void setup(ClipperLibXYZ::Path* path);
		void setup(ClipperLibXYZ::PolyNode* poly);

		Patch* earClipping();
		Patch* earClippingNewType();
		Patch* earClippingFromRefPoly(SimplePoly* poly);
		Patch* earClippingFromPath(ClipperLibXYZ::Path* path);

		Patch* earClippingFromRefPoly(ClipperLibXYZ::PolyNode* poly);
	protected:
		void calType(ENode* node, bool testContainEdge = true);
		void releaseNode();

		bool earClipping(Patch* triangle);
		void nodeTriangle(Patch* triangle, ENode* node);
	protected:
		ENode* m_root;
		int m_totalSize;
		int m_circleSize;
		std::list<ENode*> m_ears;
		std::vector<ENode*> m_nodes;

		//
		mmesh::PolygonStack m_stack;
		std::vector<trimesh::TriMesh::Face> m_tris;
		std::vector<std::vector<int>> m_earspolygons;
		std::vector<trimesh::dvec2> m_earspoints;
		std::vector<double> m_earspointsZ;
	};
}

#endif // FMESH_EARPOLYGON_1604488905381_H