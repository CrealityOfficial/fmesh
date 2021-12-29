#ifndef CDRDXF_SPLINE_1605513472182_H
#define CDRDXF_SPLINE_1605513472182_H
#include "fmesh/common/point.h"
#include <vector>

namespace cdrdxf
{
	struct DXFSpline
	{
		int degree;
		int nKnots;
		int nControl;
		int flags;
		std::vector<Point> controlPoints;
		std::vector<double> knots;
	};
}

#endif // CDRDXF_SPLINE_1605513472182_H