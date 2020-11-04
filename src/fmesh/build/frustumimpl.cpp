#include "frustumimpl.h"
#include "fmesh/build/tribuilder.h"

FrustumImpl::FrustumImpl()
{

}

FrustumImpl::~FrustumImpl()
{

}

void FrustumImpl::setup(const F2MParam& param)
{
	double thickness = param.thickness / 2.0f;
	int count = 1;
	double offset = param.totalH * std::sin(11.0 * 3.1415926 / 180.0) / (double)count;

	Layer* layer0 = createLayer(thickness, trimesh::dvec3(0.0, 0.0, param.totalH));
	fillLayer(layer0);

	std::vector<ClipperLib::PolyTree*> trees;

	///m_extend1.reset(fmesh::offsetAndExtend(m_poly, xx / 200.0, xx / 40.0));
	///m_extend2.reset(fmesh::offsetAndExtend(m_poly, xx / 100.0, xx / 40.0));
	for (int i = 1; i <= count; ++i)
	{	
		Layer* layer = createExtendLayer(thickness, (double)i * offset, trimesh::dvec3(0.0, 0.0, param.totalH - (double)i * param.totalH / (double)count));
		fillLayer(layer);
	}
}