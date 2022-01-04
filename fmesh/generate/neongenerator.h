#ifndef FMESH_NEONGENERATOR_1604508837329_H
#define FMESH_NEONGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class NeonGenerator : public GeneratorImpl
	{
	public:
		NeonGenerator();
		virtual ~NeonGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, double& middleoffset,bool onePoly=false);
		void initTestData();
	};
}

#endif // FMESH_NEONGENERATOR_1604508837329_H