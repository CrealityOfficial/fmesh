#include "layer.h"
#include "vertexpool.h"

void remapRefPoly(RefPoly* poly, int& vertexIndex)
{
	for (RefVertex* v : *poly)
	{
		v->index = vertexIndex++;
	}
}

void fillFromRefPoly(RefPoly* poly, trimesh::vec3*& vertex)
{
	for (RefVertex* v : *poly)
	{
		*vertex++ = v->v;
	}
}

void invertRef(RefPoly* poly, RefPoly* ref)
{
	size_t size = ref->size();
	if (size > 0)
	{
		poly->reserve(size);
		for (size_t i = 0; i < size; ++i)
		{
			poly->push_back(ref->at(size - 1 - i));
		}
	}
}

LayerNode::LayerNode()
	:poly(new RefPoly())
	, depth(0)
{

}

LayerNode::~LayerNode()
{
	delete poly;
	for (LayerNode* n : children)
		delete n;
	children.clear();
}

void LayerNode::expand(ClipperLib::PolyNode* node, const trimesh::dvec3& offset, int d)
{
	buildRefPoly(node->Contour, poly, offset);

	depth = d;

	int size = node->ChildCount();
	for (int i = 0; i < size; ++i)
	{
		LayerNode* layerNode = new LayerNode();
		layerNode->expand(node->Childs.at(i), offset, d + 1);
		children.push_back(layerNode);
	}

	if (size > 0)
	{
		for (int i = 0; i < size; ++i)
			children.at(i)->calMaxX();

		std::sort(children.begin(), children.end(), [](LayerNode* n1, LayerNode* n2)->bool {
			return n1->maxX < n2->maxX;
			});
	}
}

void LayerNode::offset(LayerNode* src, offsetFunc func)
{
	offsetRefPoly(poly, src->poly, func);

	depth = src->depth;
	int size = (int)src->children.size();
	for (int i = 0; i < size; ++i)
	{
		LayerNode* layerNode = new LayerNode();
		layerNode->offset(src->children.at(i), func);
		children.push_back(layerNode);
	}
}

int LayerNode::vertexNum()
{
	int vn = (int)poly->size();
	int size = (int)children.size();
	for (int i = 0; i < size; ++i)
		vn += children.at(i)->vertexNum();
	return vn;
}

double LayerNode::calMaxX()
{
	double x = -9999999999999.0;
	for (RefVertex* v : *poly)
	{
		if (v->v.x > x)
			x = v->v.x;
	}
	maxX = x;
	return x;
}

void LayerNode::remap(int& vertexIndex)
{
	remapRefPoly(poly, vertexIndex);
	for (LayerNode* n : children)
		n->remap(vertexIndex);
}

void LayerNode::fillFromLayer(trimesh::vec3*& vertex)
{
	fillFromRefPoly(poly, vertex);
	for (LayerNode* n : children)
		n->fillFromLayer(vertex);
}

void LayerNode::allPoly(std::vector<RefPoly*>& polys)
{
	polys.push_back(poly);
	for (LayerNode* n : children)
		n->allPoly(polys);
}

void LayerNode::flagPoly(std::vector<RefPoly*>& polys, bool outter)
{
	if (outter && depth % 2 == 1)
		polys.push_back(poly);
	else if (!outter && depth % 2 == 0)
		polys.push_back(poly);

	for (LayerNode* n : children)
		n->flagPoly(polys, outter);
}

Layer::Layer()
{

}

Layer::~Layer()
{

}

void Layer::build(ClipperLib::PolyTree* tree, const trimesh::dvec3& offset)
{
	if (!tree)
		return;
	
	expand(tree, offset, 0);
}

Layer* Layer::createOffset(offsetFunc func)
{
	Layer* layer = new Layer();
	layer->offset(this, func);
	return layer;
}