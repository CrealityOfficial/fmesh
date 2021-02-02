#ifndef FMESH_REITALICSGENERATOR_1604508837329_H
#define FMESH_REITALICSGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class ReItalicsGenerator : public GeneratorImpl
	{
	public:
		ReItalicsGenerator();
		virtual ~ReItalicsGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, double& middleoffset);
		void initTestData();
	};
}

#endif // #define FMESH_REITALICSGENERATOR_1604508837329_H
