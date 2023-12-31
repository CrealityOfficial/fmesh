#ifndef FMESH_SLOPEGENERATOR_1604508837329_H
#define FMESH_SLOPEGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SlopeGenerator : public GeneratorImpl
	{
	public:
		SlopeGenerator();
		virtual ~SlopeGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys,bool onePoly=false);
		void initTestData();
	};
}

#endif // FMESH_SLOPEGENERATOR_1604508837329_H