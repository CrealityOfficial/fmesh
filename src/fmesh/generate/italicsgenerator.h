#ifndef FMESH_ITALICSGENERATOR_1604508837329_H
#define FMESH_ITALICSGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class ItalicsGenerator : public GeneratorImpl
	{
	public:
		ItalicsGenerator();
		virtual ~ItalicsGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLib::PolyTree& topTree, ClipperLib::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLib::PolyTree>& middlePolys, double& middleoffset,bool onePoly=false);
		void initTestData();
	};
}

#endif // FMESH_ITALICSGENERATOR_1604508837329_H