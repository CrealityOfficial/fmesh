#include "domeimpl.h"
#include "fmesh/build/tribuilder.h"

DomeImpl::DomeImpl()
{

}

DomeImpl::~DomeImpl()
{

}

void DomeImpl::setup(const F2MParam& param)
{
	int layers = 100;
	int curLayer = 0;
	double delta = param.totalH / (double)layers;

	float drumRadius = 5.0;
	int stepsCount = drumRadius / delta ;

	std::vector<Layer*> bLayers;
	bLayers.resize(stepsCount+2);
	
	Layer* layer0 = createLayerWithoutInsert(param.thickness / 2.0f);
	bLayers.at(0) = layer0;
	Layer* layer_offset = createLayerWithoutInsert_offset(-param.thickness / 2.0f, param.thickness / 2.0f);

	int index = layers-stepsCount;
	offsetFunc fOffset = [&param, &delta, &index](const trimesh::dvec3& v)->trimesh::dvec3
	{
		return trimesh::dvec3(v.x, v.y, (double)index * delta);
	};

	//0 to stepsCount-1;
	bLayers.at(1) = createOffsetLayerWithoutInsert(layer0, fOffset);

	//stepsCount-1 to stepCount
	for (int i = 1; i < stepsCount+1; ++i)
	{
		index = layers - stepsCount + i + 1;
		double stretch = -(drumRadius - (std::sqrt(drumRadius * drumRadius - delta * delta*i*i)));
		Layer* layer_offset = createLayerWithoutInsert_offset(stretch , param.thickness / 2.0f);
		bLayers.at(i+1) = createOffsetLayerWithoutInsert(layer_offset, fOffset);
	}

// 	index = stepsCount + 1;
// 	bLayers.at(2) = createOffsetLayerWithoutInsert(layer_offset, fOffset);
// 
// 	//step +1 to layers
// 
// 	index = layers;
// 	bLayers.at(3) = createOffsetLayerWithoutInsert(layer_offset, fOffset);

	std::vector<RefTriangle*> tris;
	tris.resize(stepsCount + 1);

	//#pragma  omp parallel for
	for (int i = 0; i < stepsCount + 1; ++i)
	{
		tris.at(i) = buildFromParalleLayersInto(bLayers.at(i), bLayers.at(i + 1));
	}

	fillLayer(bLayers.back());
	fillLayer(layer0, true);
	fillLayerInner(bLayers[stepsCount + 1]);

	for (Layer* layer : bLayers)
		addLayer(layer);
	for (RefTriangle* tri : tris)
		addTriangle(tri);
}