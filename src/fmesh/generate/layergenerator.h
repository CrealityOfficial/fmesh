#ifndef FMESH_LAYERGENERATOR_1607587577378_H
#define FMESH_LAYERGENERATOR_1607587577378_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class LayerGeneratorTracer
	{
	public:
		virtual ~LayerGeneratorTracer() {}
	};

	class LayerGenerator : public GeneratorImpl
	{
	public:
		LayerGenerator();
		virtual ~LayerGenerator();

		void build() override;
	};
}

#endif // FMESH_LAYERGENERATOR_1607587577378_H