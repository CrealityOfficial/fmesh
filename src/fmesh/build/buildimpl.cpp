#include "buildimpl.h"
#include <assert.h>

#include "fmesh/build/vertexpool.h"
#include "fmesh/build/tribuilder.h"
#include "fmesh/build/polyfiller.h"

#include "fmesh/contour/contour.h"

BuildImpl::BuildImpl()
	: m_paths(nullptr)
{

}

BuildImpl::~BuildImpl()
{

}

trimesh::TriMesh* BuildImpl::build(ClipperLib::Paths* paths, const F2MParam& param)
{
	m_vertexes.clear();
	m_triangles.clear();
	buildAllocate();

	m_paths = paths;
	m_poly = fmesh::convertPaths2PolyTree(m_paths);
	if (!paths || paths->size() == 0 || !m_poly)
		return nullptr;

	setup(param);

	trimesh::TriMesh* mesh = build();
	buildDeallocate();
	return mesh;
}

void BuildImpl::addVertex(RefPoly* poly)
{
	if (poly) m_vertexes.push_back(poly);
}

void BuildImpl::addLayer(Layer* layer)
{
	if (layer) m_layers.push_back(layer);
}

void BuildImpl::addTriangle(RefTriangle* tri, bool invert)
{
	if (tri)
	{
		if (invert)
		{
			size_t size = tri->size() / 3;
			for (size_t i = 0; i < size; ++i)
				std::swap(tri->at(3 * i + 1), tri->at(3 * i + 2));
		}
		m_triangles.push_back(tri);
	}
}

ClipperLib::PolyTree* BuildImpl::extend(double delta)
{
	return fmesh::extendPaths2PolyTree(m_paths, delta);
}

ClipperLib::PolyTree* BuildImpl::offsetAndExtend(double offset, double delta)
{
	ClipperLib::PolyTree* tree = fmesh::offsetPolyTree(m_poly, offset);
	ClipperLib::PolyTree* extendTree = fmesh::extendPolyTree2PolyTree(tree, delta);

	delete tree;
	return extendTree;
}

Layer* BuildImpl::createLayer(double delta, const trimesh::dvec3& offset)
{
	Layer* layer = new Layer();
	ClipperLib::PolyTree* tree = extend(delta);
	layer->build(tree, offset);
	addLayer(layer);

	delete tree;
	return layer;
}

Layer* BuildImpl::createExtendLayer(double delta, double extend, const trimesh::dvec3& offset)
{
	Layer* layer = new Layer();
	ClipperLib::PolyTree* tree = offsetAndExtend(extend, delta);
	layer->build(tree, offset);
	addLayer(layer);

	delete tree;
	return layer;
}

Layer* BuildImpl::createLayerWithoutInsert(double delta, const trimesh::dvec3& offset)
{
	Layer* layer = new Layer();
	ClipperLib::PolyTree* tree = extend(delta);
	layer->build(tree, offset);

	delete tree;
	return layer;
}

Layer* BuildImpl::createOffsetLayer(Layer* oLayer, offsetFunc func)
{
	Layer* layer = oLayer->createOffset(func);
	addLayer(layer);
	return layer;
}

Layer* BuildImpl::createOffsetLayerWithoutInsert(Layer* oLayer, offsetFunc func)
{
	Layer* layer = oLayer->createOffset(func);
	return layer;
}

void BuildImpl::buildFromParallelLayers(Layer* layer1, Layer* layer2)
{
	RefTriangle* triRef = new RefTriangle();
	triRef->reserve(1000);

	std::vector<RefPoly*> ref1;
	layer1->allPoly(ref1);
	std::vector<RefPoly*> ref2;
	layer2->allPoly(ref2);
	buildFromRefPolies(ref1, ref2, *triRef);
	addTriangle(triRef);
}

void BuildImpl::buildFromParallelLayersWithFlag(Layer* layer1, Layer* layer2, bool outter)
{
	RefTriangle* triRef = new RefTriangle();
	triRef->reserve(1000);

	std::vector<RefPoly*> ref1;
	std::vector<RefPoly*> ref2;
	layer1->flagPoly(ref1, outter);
	layer2->flagPoly(ref2, outter);

	buildFromRefPolies(ref1, ref2, *triRef);
	addTriangle(triRef);
}

RefTriangle* BuildImpl::buildFromParalleLayersInto(Layer* layer1, Layer* layer2)
{
	RefTriangle* triRef = new RefTriangle();
	triRef->reserve(1000);

	std::vector<RefPoly*> ref1;
	layer1->allPoly(ref1);
	std::vector<RefPoly*> ref2;
	layer2->allPoly(ref2);
	buildFromRefPolies(ref1, ref2, *triRef);
	return triRef;
}

void BuildImpl::fillLayer(Layer* layer, bool invert)
{
	std::vector<RefTriangle*> tris;
	fillComplexLayerNode(layer, tris);

	for (RefTriangle* tri : tris)
		addTriangle(tri, invert);
}

void BuildImpl::fillLayerOutline(Layer* layer, bool invert)
{
	std::vector<RefTriangle*> tris;
	fillLayerFirstLevel(layer, tris);

	for (RefTriangle* tri : tris)
		addTriangle(tri, invert);
}

void BuildImpl::fillLayerInner(Layer* layer, bool invert)
{
	std::vector<RefTriangle*> tris;
	fillLayerAllInner(layer, tris);

	for (RefTriangle* tri : tris)
		addTriangle(tri, invert);
}

trimesh::TriMesh* BuildImpl::build()
{
	int vertexNum = 0;
	for (RefPoly* poly : m_vertexes)
		remapRefPoly(poly, vertexNum);
	for (Layer* layer : m_layers)
		layer->remap(vertexNum);

	int faceNum = 0;
	for (RefTriangle* tri : m_triangles)
		faceNum += (int)tri->size() / 3;

	if (vertexNum == 0 || faceNum == 0)
		return nullptr;

	//fill vertex
	trimesh::TriMesh* mesh = new trimesh::TriMesh();
	mesh->vertices.resize(vertexNum);
	mesh->faces.resize(faceNum);

	trimesh::vec3* vertex = &mesh->vertices.at(0);
	for (RefPoly* poly : m_vertexes)
		fillFromRefPoly(poly, vertex);
	for (Layer* layer : m_layers)
		layer->fillFromLayer(vertex);

	//fill face
	int faceIndex = 0;
	for (RefTriangle* tri : m_triangles)
	{
		size_t size = tri->size() / 3;
		for (size_t i = 0; i < size; ++i)
		{
			trimesh::TriMesh::Face& f = mesh->faces.at(faceIndex);
			for(int j = 0; j < 3; ++j)
				f[j] = tri->at(3 * i + j)->index;

			++faceIndex;
		}
	}

	for (RefPoly* poly : m_vertexes)
		delete poly;
	m_vertexes.clear();
	for (Layer* layer : m_layers)
		delete layer;
	m_layers.clear();
	for (RefTriangle* tri : m_triangles)
		delete tri;
	m_triangles.clear();
	return mesh;
}

#define REGISTER(x, y)  { BIMIterator it = impls.find(x); \
						if(it != impls.end()) \
						{ \
							printf("register error."); \
						} else {\
							 BuildImpl* impl = new y();\
							impls.insert(BIMValue(x, impl)); }}

#include "fmesh/build/simpleimpl.h"
#include "fmesh/build/slopeimpl.h"
#include "fmesh/build/fillimpl.h"

#include "fmesh/build/fieldimpl.h"
#include "fmesh/build/waveimpl.h"
#include "fmesh/build/bottomimpl.h"
#include "fmesh/build/frustumimpl.h"
void createBuildImpls(BuildImplMap& impls)
{
	REGISTER("simple", SimpleImpl)
	REGISTER("slope", SlopeImpl)
	REGISTER("fill", FillImpl)
	REGISTER("field", FieldImpl)
	REGISTER("wave", WaveImpl)
	REGISTER("bottom", BottomImpl)
	REGISTER("frustum", FrustumImpl)
}

void destroyBuildImpls(BuildImplMap& impls)
{
	for (BIMIterator it = impls.begin(); it != impls.end(); ++it)
		delete it->second;
	impls.clear();
}