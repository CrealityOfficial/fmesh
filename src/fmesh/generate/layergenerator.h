#ifndef FMESH_LAYERGENERATOR_1607587577378_H
#define FMESH_LAYERGENERATOR_1607587577378_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class LayerGeneratorTracer
	{
	public:
		virtual ~LayerGeneratorTracer() {}

		virtual int index() = 0;
		virtual void impl(ClipperLib::PolyTree& poly1, ClipperLib::PolyTree& poly2) = 0;
	};

	class FMESH_API LayerGenerator : public GeneratorImpl
	{
	public:
		LayerGenerator();
		virtual ~LayerGenerator();

		void build() override;

		void setLayerGeneratorTracer(LayerGeneratorTracer* tracer);
		void saveXOR(ClipperLib::PolyTree& tree, const ADParam& param);
	protected:
		LayerGeneratorTracer* m_tracer;
	};
}

#endif // FMESH_LAYERGENERATOR_1607587577378_H