#ifndef FMESH_SHARPTOPGENERATOR_1604649717545_H
#define FMESH_SHARPTOPGENERATOR_1604649717545_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SharptopGenerator : public GeneratorImpl
	{
	public:
		SharptopGenerator();
		virtual ~SharptopGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys,bool onePoly=false);
		void initTestData();
	};
}

#endif // FMESH_SHARPTOPGENERATOR_1604649717545_H