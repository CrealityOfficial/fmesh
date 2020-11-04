#include "bottomimpl.h"
#include "fmesh/build/tribuilder.h"

BottomImpl::BottomImpl()
{

}

BottomImpl::~BottomImpl()
{

}

void BottomImpl::setup(const F2MParam& param)
{
	offsetFunc fNormalH = [&param](const trimesh::dvec3& v)->trimesh::dvec3
	{
		return v + trimesh::dvec3(0.0, 0.0, param.totalH);
	};
	offsetFunc fNormalB = [&param](const trimesh::dvec3& v)->trimesh::dvec3
	{
		return v + trimesh::dvec3(0.0, 0.0, param.bottomH);
	};

	Layer* layer0 = createLayer(param.thickness / 2.0f);
	Layer* layerB = createOffsetLayer(layer0, fNormalB);
	Layer* layerH = createOffsetLayer(layer0, fNormalH);

	fillLayer(layerH);
	fillLayerOutline(layer0, true);
	fillLayerInner(layerB);
	buildFromParallelLayers(layerB, layerH);
	buildFromParallelLayersWithFlag(layer0, layerB, true);
}