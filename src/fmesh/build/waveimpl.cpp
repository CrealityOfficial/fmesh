#include "waveimpl.h"
#include "fmesh/build/tribuilder.h"

WaveImpl::WaveImpl()
{

}

WaveImpl::~WaveImpl()
{

}

void WaveImpl::setup(const F2MParam& param)
{

	Layer* layer0 = createLayerWithoutInsert(param.thickness / 2.0f);
	int layers = 100;
	int curLayer = 0;
	double delta = param.totalH / (double)layers;

	std::vector<Layer*> bLayers;
	bLayers.resize(101);
	bLayers.at(0) = layer0;

//#pragma omp parallel for
	for (int i = 0; i < layers; ++i)
	{
		int index = i + 1;
		offsetFunc fOffset = [&param, &delta, &index](const trimesh::dvec3& v)->trimesh::dvec3
		{
			return trimesh::dvec3(v.x + sinf((double)index / 20.0), v.y, (double)index * delta);
		};

		bLayers.at(index) = createOffsetLayerWithoutInsert(layer0, fOffset);
	}

	std::vector<RefTriangle*> tris;
	tris.resize(100);

//#pragma omp parallel for
	for (int i = 0; i < layers; ++i)
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