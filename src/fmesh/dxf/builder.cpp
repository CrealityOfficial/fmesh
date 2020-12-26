#include "builder.h"
#include "fmesh/dxf/spline.h"

#include <iostream>
#include <numeric>

namespace cdrdxf
{
	void convert(DXFSpline* spline, ClipperLib::Path* path)
	{
		if (!spline || !path)
			return;

		if (spline->degree < 1 || spline->degree>3) 
		{
			printf("invalid degree: %d", spline->degree);
			return;
		}

		if (spline->controlPoints.size() < spline->degree + 1) 
		{
			printf("not enough control points");
			return;
		}

		const size_t nCtrl = spline->nControl;
		// order:
		const size_t  order = spline->degree + 1;
		// resolution:
		const size_t  n = 8 * nCtrl;

		std::vector<double> h(nCtrl + 1, 1.0);
		std::vector<Point> fitPoints(n);

		rbspline(nCtrl, order, n, spline->controlPoints, h, spline->knots, fitPoints);

		if (n > 0)
		{
			path->resize(n);
			for (size_t i = 0; i < n; ++i)
			{
				ClipperLib::IntPoint& p = path->at(i);
				Point& pp = fitPoints.at(i);
				p.X = (ClipperLib::cInt)(1000.0 * pp.x);
				p.Y = (ClipperLib::cInt)(1000.0 * pp.y);
			}
		}
	}

	void rbspline(size_t nCtrls, size_t order, size_t n,
		const std::vector<Point>& ctrlPoints,
		const std::vector<double>& h,
		const std::vector<double>& knots,
		std::vector<Point>& fitPoints)
	{
		size_t const nplusc = nCtrls + order;

		// generate the open knot vector
		auto const x = knot(nCtrls, order, knots);

		// calculate the points on the rational B-spline curve
		double t{ x[0] };
		double const step{ (x[nplusc - 1] - t) / (n - 1) };

		for (Point& vp : fitPoints)
		{
			if (x[nplusc - 1] - t < 5e-6)
				t = x[nplusc - 1];

			// generate the basis function for this value of t
			auto const nbasis = rbasis(order, t, nCtrls, x, h);

			// generate a point on the curve
			for (size_t i = 0; i < nCtrls; i++)
			{
				vp.x += ctrlPoints[i].x * nbasis[i];
				vp.y += ctrlPoints[i].y * nbasis[i];
			}
			t += step;
		}
	}

	std::vector<double> knot(size_t num, size_t order, const std::vector<double>& knots)
	{
		if (knots.size() == num + order)
		{
			//use custom knot vector
			return knots;
		}

		std::vector<double> knotVector(num + order, 0.);
		//use uniform knots
		std::iota(knotVector.begin() + order, knotVector.begin() + num + 1, 1);
		std::fill(knotVector.begin() + num + 1, knotVector.end(), knotVector[num]);
		return knotVector;
	}

	std::vector<double> rbasis(int c, double t, int npts,
		const std::vector<double>& x,
		const std::vector<double>& h) 
	{
		int const nplusc = npts + c;

		std::vector<double> temp(nplusc, 0.);

		// calculate the first order nonrational basis functions n[i]
		for (int i = 0; i < nplusc - 1; i++)
			if ((t >= x[i]) && (t < x[i + 1])) temp[i] = 1;

		/* calculate the higher order nonrational basis functions */

		for (int k = 2; k <= c; k++) 
		{
			for (int i = 0; i < nplusc - k; i++) 
			{
				// if the lower order basis function is zero skip the calculation
				if (temp[i] != 0)
					temp[i] = ((t - x[i]) * temp[i]) / (x[i + k - 1] - x[i]);
				// if the lower order basis function is zero skip the calculation
				if (temp[i + 1] != 0)
					temp[i] += ((x[i + k] - t) * temp[i + 1]) / (x[i + k] - x[i + 1]);
			}
		}

		// pick up last point
		if (t >= x[nplusc - 1])
			temp[npts - 1] = 1;

		// calculate sum for denominator of rational basis functions
		double sum = 0.;
		for (int i = 0; i < npts; i++) 
		{
			sum += temp[i] * h[i];
		}

		std::vector<double> r(npts, 0);
		// form rational basis functions and put in r vector
		if (sum != 0) 
		{
			for (int i = 0; i < npts; i++)
				r[i] = (temp[i] * h[i]) / sum;
		}
		return r;
	}
}