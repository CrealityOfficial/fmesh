#ifndef FMESH_SHARPSIDEDOUBLEGENERATOR_1604508837329_H
#define FMESH_SHARPSIDEDOUBLEGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SharpsideDoubleGenerator : public GeneratorImpl
	{
	public:
		SharpsideDoubleGenerator();
		virtual ~SharpsideDoubleGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys,bool onePoly=false);
		void initTestData();
	};
}

#endif // FMESH_SHARPSIDEDOUBLEGENERATOR_1604508837329_H