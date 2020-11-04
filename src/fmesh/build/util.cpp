#include "util.h"

namespace fmesh
{
	void calculateBox(ClipperLib::PolyTree* poly, ClipperLib::IntPoint& bMin, ClipperLib::IntPoint& bMax)
	{
		bMin = ClipperLib::IntPoint(99999999, 99999999);
		bMax = ClipperLib::IntPoint(-99999999, -99999999);

		polyNodeFunc func = [&func, &bMin, &bMax](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& p : node->Contour)
			{
				bMin.X = std::min(bMin.X, p.X);
				bMin.Y = std::min(bMin.Y, p.Y);
				bMax.X = std::max(bMax.X, p.X);
				bMax.Y = std::max(bMax.Y, p.Y);
			}

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		func(poly);
	}
}