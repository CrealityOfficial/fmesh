#ifndef FMESH_WIDENOUTERGENERATOR_1604475645830_H
#define FMESH_WIDENOUTERGENERATOR_1604475645830_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class WidenOuterGenerator : public GeneratorImpl
	{
	public:
		WidenOuterGenerator();
		virtual ~WidenOuterGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;
	};
}

#endif // FMESH_WIDENOUTERGENERATOR_1604475645830_H