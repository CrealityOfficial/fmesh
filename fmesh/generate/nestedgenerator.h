#ifndef FMESH_NESTEDGENERATOR_1604508837329_H
#define FMESH_NESTEDGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class NestedGenerator : public GeneratorImpl
	{
	public:
		NestedGenerator();
		virtual ~NestedGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;

	private:
		void buildOuter(std::vector<ClipperLib::PolyTree>& middlePolys,bool onePoly = false);
		void buildInner(std::vector<ClipperLib::PolyTree>& middlePolys,bool onePoly = false);

		ClipperLib::cInt getAABB(ClipperLib::PolyTree& poly);

		void initTestData();
	};
}

#endif // FMESH_NESTEDGENERATOR_1604508837329_H