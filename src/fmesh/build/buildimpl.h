#ifndef _BUILDIMPL_1603547482787_H
#define _BUILDIMPL_1603547482787_H
#include <clipper/clipper.hpp>
#include "trimesh2/TriMesh.h"

#include "mmesh/clipper/contour.h"
#include "mmesh/trimesh/contour.h"
#include "mmesh/clipper/circurlar.h"

#include "fmesh/contour/contour.h"
#include "fmesh/build/layer.h"
#include "fmesh/build/offsetfunc.h"
#include <map>

class BuildImpl
{
public:
	BuildImpl();
	virtual ~BuildImpl();

	trimesh::TriMesh* build(ClipperLib::Paths* paths, const F2MParam& param);
protected:
	virtual void setup(const F2MParam& param) = 0;
	trimesh::TriMesh* build();
	void addVertex(RefPoly* poly);
	void addLayer(Layer* layer);
	void addTriangle(RefTriangle* tri, bool invert = false);

	Layer* createLayer(double delta, const trimesh::dvec3& offset = trimesh::dvec3(0.0, 0.0, 0.0));
	Layer* createExtendLayer(double delta, double extend, const trimesh::dvec3& offset = trimesh::dvec3(0.0, 0.0, 0.0));
	Layer* createLayerWithoutInsert(double delta, const trimesh::dvec3& offset = trimesh::dvec3(0.0, 0.0, 0.0));
	Layer* createOffsetLayer(Layer* oLayer, offsetFunc func);
	Layer* createOffsetLayerWithoutInsert(Layer* oLayer, offsetFunc func);

	Layer* createLayerWithoutInsert_offset(double stretch,double delta, const trimesh::dvec3& offset = trimesh::dvec3(0.0, 0.0, 0.0));
	ClipperLib::PolyTree* extend_offset(double delta);

	void buildFromParallelLayers(Layer* layer1, Layer* layer2);
	void buildFromParallelLayersWithFlag(Layer* layer1, Layer* layer2, bool outter);
	RefTriangle* buildFromParalleLayersInto(Layer* layer1, Layer* layer2);
	
	void fillLayer(Layer* layer, bool invert = false);
	void fillLayerOutline(Layer* layer, bool invert = false);
	void fillLayerInner(Layer* layer, bool invert = false);

	ClipperLib::PolyTree* extend(double delta);
	ClipperLib::PolyTree* offsetAndExtend(double offset, double delta);
protected:
	ClipperLib::Paths* m_paths;
	ClipperLib::PolyTree* m_poly;

	std::vector<RefPoly*> m_vertexes;
	std::vector<Layer*> m_layers;
	std::vector<RefTriangle*> m_triangles;
};

typedef std::map<std::string, BuildImpl*> BuildImplMap;
typedef BuildImplMap::iterator BIMIterator;
typedef BuildImplMap::value_type BIMValue;

void createBuildImpls(BuildImplMap& impls);
void destroyBuildImpls(BuildImplMap& impls);
#endif // _BUILDIMPL_1603547482787_H