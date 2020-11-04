#ifndef _LAYER_1603812037436_H
#define _LAYER_1603812037436_H
#include "trimesh2/Vec.h"
#include <clipper/clipper.hpp>
#include <vector>

struct RefVertex
{
	trimesh::dvec3 v;
	int index;
};

typedef std::vector<RefVertex*> RefPoly;
typedef std::vector<RefVertex*> RefTriangle;  // 3 
typedef std::function<trimesh::dvec3(const trimesh::dvec3& v)> offsetFunc;

void remapRefPoly(RefPoly* poly, int& vertexIndex);
void fillFromRefPoly(RefPoly* poly, trimesh::vec3*& vertex);
void invertRef(RefPoly* poly, RefPoly* ref);

class LayerNode
{
public:
	LayerNode();
	virtual ~LayerNode();

	void expand(ClipperLib::PolyNode* node, const trimesh::dvec3& offset, int depth);
	void offset(LayerNode* src, offsetFunc func);
	int vertexNum();
	double calMaxX();
	void remap(int& vertexIndex);
	void fillFromLayer(trimesh::vec3*& vertex);
	void allPoly(std::vector<RefPoly*>& polys);
	void flagPoly(std::vector<RefPoly*>& polys, bool outter);

	int depth;
	RefPoly* poly;
	std::vector<LayerNode*> children;
	double maxX;
};

class Layer : public LayerNode
{
public:
	Layer();
	virtual ~Layer();

	void build(ClipperLib::PolyTree* tree, const trimesh::dvec3& offset);
	Layer* createOffset(offsetFunc func);
};
#endif // _LAYER_1603812037436_H