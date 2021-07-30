#ifndef FMESH_PUNCHINGGENERATOR_1604508837329_H
#define FMESH_PUNCHINGGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class PunchingGenerator : public GeneratorImpl
	{
	public:
		PunchingGenerator();
		virtual ~PunchingGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, double& middleoffset,bool onePoly=false);
		void initTestData();
	};
}

#endif // FMESH_PUNCHINGGENERATOR_1604508837329_H