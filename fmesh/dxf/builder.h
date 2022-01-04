#ifndef CDRDXF_BUILDER_1605518188021_H
#define CDRDXF_BUILDER_1605518188021_H
#include "fmesh/common/point.h"
#include "clipperxyz/clipper.hpp"
#include <vector>

namespace cdrdxf
{
	struct DXFSpline;
	void convert(DXFSpline* spline, ClipperLibXYZ::Path* path);
	void rbspline(size_t nCtrls, size_t order, size_t n,
		const std::vector<Point>& ctrlPoints,
		const std::vector<double>& h,
		const std::vector<double>& knots,
		std::vector<Point>& fitPoints);

	std::vector<double> knot(size_t num, size_t order, const std::vector<double>& knots);
	std::vector<double> rbasis(int c, double t, int npts,
		const std::vector<double>& x,
		const std::vector<double>& h);
}

#endif // CDRDXF_BUILDER_1605518188021_H