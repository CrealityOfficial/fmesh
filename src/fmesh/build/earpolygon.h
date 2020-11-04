#ifndef _EARPOLYGON_1604135604097_H
#define _EARPOLYGON_1604135604097_H
#include <list>
#include "layer.h"

struct ENode
{
	ENode* prev;
	ENode* next;

	RefVertex* vertex;
	int type;  // 0 colliear 1 concave 2 convex 3 ear
	double dot;
};

class EarPolygon
{
public:
	EarPolygon();
	~EarPolygon();

	void setup(RefPoly* poly);
	RefTriangle* earClipping();
	RefTriangle* earClippingFromRefPoly(RefPoly* poly);
protected:
	void calType(ENode* node, bool testContainEdge = true);
	void releaseNode();

	bool earClipping(RefTriangle* triangle);
	void nodeTriangle(RefTriangle* triangle, ENode* node);
protected:
	ENode* m_root;
	int m_totalSize;
	int m_circleSize;
	std::list<ENode*> m_ears;
	std::vector<ENode*> m_nodes;
};

inline double crossValue(const trimesh::dvec3& v10, const trimesh::dvec3& v12)
{
	return v10.x * v12.y - v10.y * v12.x;
}

inline double dotValue(const trimesh::dvec3& v10, const trimesh::dvec3& v12)
{
	trimesh::dvec3 nv10 = trimesh::normalized(v10);
	trimesh::dvec3 nv12 = trimesh::normalized(v12);
	return trimesh::dot(nv10, nv12);
}

bool insideTriangle(const trimesh::dvec3& va, const trimesh::dvec3& vb, const trimesh::dvec3& vc,
	const trimesh::dvec3& vp);
bool insideTriangleEx(const trimesh::dvec3& va, const trimesh::dvec3& vb, const trimesh::dvec3& vc,
	const trimesh::dvec3& vp);
#endif // _EARPOLYGON_1604135604097_H