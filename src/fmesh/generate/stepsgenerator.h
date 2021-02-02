#ifndef FMESH_STEPSGENERATOR_1604549763432_H
#define FMESH_STEPSGENERATOR_1604549763432_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class StepsGenerator : public GeneratorImpl
	{
	public:
		StepsGenerator();
		virtual ~StepsGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys);
		void initTestData();
	};
}

#endif // FMESH_STEPSGENERATOR_1604549763432_H