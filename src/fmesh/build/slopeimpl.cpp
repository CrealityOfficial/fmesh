#include "slopeimpl.h"
#include "tribuilder.h"

SlopeImpl::SlopeImpl()
{

}

SlopeImpl::~SlopeImpl()
{

}

void SlopeImpl::setup(const F2MParam& param)
{
	Layer* layer1 = createLayer(param.thickness / 2.0f);

	double slope = (param.totalH - param.initH) / (param.dmax.x - param.dmin.x);
	double deltaH = param.totalH - param.initH;
	offsetFunc fNormal = [&param](const trimesh::dvec3& v)->trimesh::dvec3
	{
		return v + trimesh::dvec3(0.0, 0.0, param.totalH);
	};
	offsetFunc fSlope = [&slope, &param](const trimesh::dvec3& v)->trimesh::dvec3
	{
		return trimesh::dvec3(v.x, v.y, param.initH + slope * (v.x - param.dmin.x));
	};

	trimesh::vec2 center = (param.dmax + param.dmin) / 2.0;
	float radius = std::max(param.dmax.x - param.dmin.x, param.dmax.y - param.dmin.y);
	offsetFunc fCircle = [&deltaH, &center, &radius, &param](const trimesh::dvec3& v)->trimesh::dvec3
	{
		double z = deltaH * (1.0 - trimesh::len(center - trimesh::vec2(v.x, v.y)) / radius);
		return trimesh::dvec3(v.x, v.y, param.initH + z);
	};

	offsetFunc f = fSlope;
	if (param.args.size() > 0)
	{
		if (!strcmp(param.args.at(0).c_str(), "normal"))
			f = fNormal;
		else if (!strcmp(param.args.at(0).c_str(), "circle"))
			f = fCircle;
		else
			f = fSlope;
	}
	Layer* layer2 = createOffsetLayer(layer1, f);

	buildFromParallelLayers(layer1, layer2);
}