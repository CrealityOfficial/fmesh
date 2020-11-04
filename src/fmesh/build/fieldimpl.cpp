#include "fieldimpl.h"
#include "fmesh/build/tribuilder.h"

FieldImpl::FieldImpl()
{

}

FieldImpl::~FieldImpl()
{

}

void FieldImpl::setup(const F2MParam& param)
{
	Layer* layer1 = createLayer(param.thickness / 2.0f);

	double slope = (param.totalH - param.initH) / (param.dmax.x - param.dmin.x);
	offsetFunc fSlope = [&slope, &param](const trimesh::dvec3& v)->trimesh::dvec3
	{
		return trimesh::dvec3(v.x, v.y, param.initH + slope * (v.x - param.dmin.x));
	};

	offsetFunc f = fSlope;
	Layer* layer2 = createOffsetLayer(layer1, f);

	buildFromParallelLayers(layer1, layer2);
	fillLayer(layer2);
	fillLayer(layer1, true);
}