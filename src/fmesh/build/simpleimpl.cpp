#include "simpleimpl.h"
#include "tribuilder.h"

SimpleImpl::SimpleImpl()
{

}

SimpleImpl::~SimpleImpl()
{

}

void SimpleImpl::setup(const F2MParam& param)
{
	offsetFunc f = [&param](const trimesh::dvec3& v)->trimesh::dvec3
	{
		return v + trimesh::dvec3(0.0, 0.0, param.totalH);
	};

	Layer* layer1 = createLayer(param.thickness / 2.0f);
	Layer* layer2 = createOffsetLayer(layer1, f);
	
	buildFromParallelLayers(layer1, layer2);
	fillLayer(layer2);
	fillLayer(layer1, true);
}