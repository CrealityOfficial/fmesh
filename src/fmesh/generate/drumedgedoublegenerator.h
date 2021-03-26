#ifndef FMESH_DRUNEDGEDOUBLEGENERATOR_1604508837329_H
#define FMESH_DRUNEDGEDOUBLEGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class DrumedgeDoubleGenerator : public GeneratorImpl
	{
	public:
		DrumedgeDoubleGenerator();
		virtual ~DrumedgeDoubleGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, bool onePloy = false);	
		void initTestData();
	};
}

#endif // FMESH_DRUNEDGEDOUBLEGENERATOR_1604508837329_H