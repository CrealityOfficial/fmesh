#ifndef FMESH_DRUNEDGEGENERATOR_1604508837329_H
#define FMESH_DRUNEDGEGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class DrumedgeGenerator : public GeneratorImpl
	{
	public:
		DrumedgeGenerator();
		virtual ~DrumedgeGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePloy = false);	
		void initTestData();
	};
}

#endif // FMESH_DRUNEDGEGENERATOR_1604508837329_H