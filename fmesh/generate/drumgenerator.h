#ifndef FMESH_DRUMGENERATOR_1604649717545_H
#define FMESH_DRUMGENERATOR_1604649717545_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class DrumGenerator : public GeneratorImpl
	{
	public:
		DrumGenerator();
		virtual ~DrumGenerator();

		void build() override;

		void buildShell() override;
		void buildBoard(ClipperLibXYZ::PolyTree& topTree, ClipperLibXYZ::PolyTree& bottomTree) override;

	private:
		void buildMiddle(std::vector<ClipperLibXYZ::PolyTree>& middlePolys, bool onePloy=false);
		void initTestData();
	};
}

#endif // FMESH_DRUMGENERATOR_1604649717545_H