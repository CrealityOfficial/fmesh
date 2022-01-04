#ifndef FMESH_WIDENINNERGENERATOR_1604475645830_H
#define FMESH_WIDENINNERGENERATOR_1604475645830_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class WidenInnerGenerator : public GeneratorImpl
	{
	public:
		WidenInnerGenerator();
		virtual ~WidenInnerGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree) override;
	};
}

#endif // FMESH_WIDENINNERGENERATOR_1604475645830_H