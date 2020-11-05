#include "stepsimpl.h"
#include "fmesh/build/tribuilder.h"

StepsImpl::StepsImpl()
{

}

StepsImpl::~StepsImpl()
{

}

void StepsImpl::setup(const F2MParam& param)
{
	int layers = 100;
	int curLayer = 0;
	double delta = param.totalH / (double)layers;
	double stepsH = 2.0;
	int stepsCount = layers - (stepsH / delta);

	std::vector<Layer*> bLayers;
	bLayers.resize(4);
	
	Layer* layer0 = createLayerWithoutInsert(param.thickness / 2.0f);
	bLayers.at(0) = layer0;
	Layer* layer_offset = createLayerWithoutInsert_offset(-param.thickness / 2.0f, param.thickness / 2.0f);

	int index = stepsCount;
	offsetFunc fOffset = [&param, &delta, &index](const trimesh::dvec3& v)->trimesh::dvec3
	{
		return trimesh::dvec3(v.x, v.y, (double)index * delta);
	};

	//0 to stepsCount-1;
	bLayers.at(1) = createOffsetLayerWithoutInsert(layer0, fOffset);

	//stepsCount-1 to stepCount
	index = stepsCount + 1;
	bLayers.at(2) = createOffsetLayerWithoutInsert(layer_offset, fOffset);

	//step +1 to layers
	index = layers;
	bLayers.at(3) = createOffsetLayerWithoutInsert(layer_offset, fOffset);

	std::vector<RefTriangle*> tris;
	tris.resize(3);

	//#pragma omp parallel for
	for (int i = 0; i < 3; ++i)
	{
		tris.at(i) = buildFromParalleLayersInto(bLayers.at(i), bLayers.at(i + 1));
	}

	fillLayer(bLayers.back());
	fillLayer(layer0, true);

	for (Layer* layer : bLayers)
		addLayer(layer);
	for (RefTriangle* tri : tris)
		addTriangle(tri);
}