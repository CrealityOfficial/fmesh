#ifndef FMESH_EARPOLYGON_1604488905381_H
#define FMESH_EARPOLYGON_1604488905381_H
#include <clipper/clipper.hpp>
#include "fmesh/generate/patch.h"

namespace fmesh
{
	typedef std::vector<ClipperLib::IntPoint*> SimplePoly;
	struct ENode
	{
		ENode* prev;
		ENode* next;

		ClipperLib::IntPoint* vertex;
		int type;  // 0 colliear 1 concave 2 convex 3 ear
		double dot;
	};

	class SimplePolyWrapper
	{
	public:
		SimplePolyWrapper();
		~SimplePolyWrapper();

		void load(const std::string& file);

		std::vector<ClipperLib::IntPoint> source;
		SimplePoly poly;
	};

	void saveSimplePoly(const SimplePoly& poly, const std::string& file);

	class EarPolygon
	{
	public:
		EarPolygon();
		~EarPolygon();

		void setup(SimplePoly* poly);
		void setup(ClipperLib::Path* path);
		Patch* earClipping();
		Patch* earClippingFromRefPoly(SimplePoly* poly);
		Patch* earClippingFromPath(ClipperLib::Path* path);
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
	};
}

#endif // FMESH_EARPOLYGON_1604488905381_H