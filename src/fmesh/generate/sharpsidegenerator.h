#ifndef FMESH_SHARPSIDEGENERATOR_1604508837329_H
#define FMESH_SHARPSIDEGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SharpsideGenerator : public GeneratorImpl
	{
	public:
		SharpsideGenerator();
		virtual ~SharpsideGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys,bool onePoly=false);
		void initTestData();
	};
}

#endif // FMESH_SHARPSIDEGENERATOR_1604508837329_H