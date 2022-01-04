#ifndef FMESH_SIMPLEGENERATOR_1604475645830_H
#define FMESH_SIMPLEGENERATOR_1604475645830_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SimpleGenerator : public GeneratorImpl
	{
	public:
		SimpleGenerator();
		virtual ~SimpleGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree) override;
	};
}

#endif // FMESH_SIMPLEGENERATOR_1604475645830_H