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

#endif // _EARPOLYGON_1604135604097_H