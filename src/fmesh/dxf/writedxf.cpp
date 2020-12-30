#include "writedxf.h"
#include "fmesh/dxf/spline.h"

#include <iostream>
#include <numeric>

#include "dxf/dl_dxf.h"
#include "fmesh/generate/earpolygon.h"
#include "fmesh/generate/polyfiller.h"
#include "mmesh/clipper/circurlar.h"

namespace cdrdxf
{
	void writedxf(ClipperLib::PolyTree* tree, const std::string& file)
	{
		std::vector<ClipperLib::Path*> paths;
		size_t count = 0;
		auto f = [&count, &paths](ClipperLib::PolyNode* node) {
			if (node->IsHole())
				return;
			paths.push_back(&node->Contour);
		};
		mmesh::loopPolyTree(f,tree);

		DL_Dxf dxf;
		DL_WriterA* dw = dxf.out(file.c_str(), DL_Codes::AC1015);

		// section header:
		dxf.writeHeader(*dw);
		dw->sectionEnd();

		// section tables:
		dw->sectionTables();

		// VPORT:
		dxf.writeVPort(*dw);

		// LTYPE:
		dw->tableLinetypes(1);
		dxf.writeLinetype(*dw, DL_LinetypeData("CONTINUOUS", "Continuous", 0, 0, 0.0));
		dxf.writeLinetype(*dw, DL_LinetypeData("BYLAYER", "", 0, 0, 0.0));
		dxf.writeLinetype(*dw, DL_LinetypeData("BYBLOCK", "", 0, 0, 0.0));
		dw->tableEnd();

		// LAYER:
		dw->tableLayers(paths.size());
		for (size_t i = 0; i < paths.size(); i++)
		{
			std::string str = std::to_string(i);
			dxf.writeLayer(
				*dw,
				DL_LayerData(str, 0),
				DL_Attributes("", 1, 0x00ff0000, 15, "CONTINUOUS")
			);
		}

// 		dxf.writeLayer(
// 			*dw,
// 			DL_LayerData("0", 0),
// 			DL_Attributes("", 1, 0x00ff0000, 15, "CONTINUOUS")
// 		);
// 		dxf.writeLayer(
// 			*dw,
// 			DL_LayerData("1", 0),
// 			DL_Attributes("", 1, 0x00ff0000, 15, "CONTINUOUS")
// 		);
		dw->tableEnd();

		// STYLE:
		dw->tableStyle(1);
		DL_StyleData style("Standard", 0, 0.0, 1.0, 0.0, 0, 2.5, "txt", "");
		style.bold = false;
		style.italic = false;
		dxf.writeStyle(*dw, style);
		dw->tableEnd();

		// VIEW:
		dxf.writeView(*dw);

		// UCS:
		dxf.writeUcs(*dw);

		// APPID:
		dw->tableAppid(1);
		dxf.writeAppid(*dw, "ACAD");
		dw->tableEnd();

		// DIMSTYLE:
		dxf.writeDimStyle(*dw, 2.5, 0.625, 0.625, 0.625, 2.5);

		// BLOCK_RECORD:
		dxf.writeBlockRecord(*dw);
		dw->tableEnd();

		dw->sectionEnd();

		// BLOCK:
		dw->sectionBlocks();

		for (size_t i = 0; i < paths.size(); i++)
		{
			std::string str = std::to_string(i);
			dxf.writeBlock(*dw, DL_BlockData(str, 0, 0.0, 0.0, 0.0));
			dxf.writeEndBlock(*dw, str);
		}
// 		dxf.writeBlock(*dw, DL_BlockData("*Model_Space", 0, 0.0, 0.0, 0.0));
// 		dxf.writeEndBlock(*dw, "*Model_Space");
// 		dxf.writeBlock(*dw, DL_BlockData("*Paper_Space", 0, 0.0, 0.0, 0.0));
// 		dxf.writeEndBlock(*dw, "*Paper_Space");
// 		dxf.writeBlock(*dw, DL_BlockData("*Paper_Space0", 0, 0.0, 0.0, 0.0));
// 		dxf.writeEndBlock(*dw, "*Paper_Space0");
		dw->sectionEnd();

		// ENTITIES:
		dw->sectionEntities();



/*		DL_Attributes attributes1("1", 256, -1, -1, "BYLAYER");*/

		// LINE:
		for (size_t i=0;i<paths.size();i++)
		{
			std::string str = std::to_string(i);
			DL_Attributes attributes(str, 256, -1, -1, "BYLAYER");
			for (size_t j = 0; j < paths.at(i)->size()-1; j++)
			{
				DL_LineData lineData(paths.at(i)->at(j).X, paths.at(i)->at(j).Y, 0, paths.at(i)->at(j+1).X, paths.at(i)->at(j+1).Y, 0);
				dxf.writeLine(*dw, lineData, attributes);
			}
			if (paths.at(i)->size()>0)
			{
				DL_LineData lineData(paths.at(i)->at(paths.at(i)->size() - 1).X, paths.at(i)->at(paths.at(i)->size() - 1).Y, 0, paths.at(i)->at(0).X, paths.at(i)->at(0).Y, 0);
				dxf.writeLine(*dw, lineData, attributes);
			}
		}

// 		DL_LineData lineData(10, 5, 0, 30, 5, 0);
// 		dxf.writeLine(*dw, lineData, attributes);

// 		DL_LineData lineData1(5, 1, 0, 20, 1, 0);
// 		dxf.writeLine(*dw, lineData1, attributes1);

// 		//point
// 		dxf.writePoint(*dw,
// 			DL_PointData(10.0, 45.0, 0.0),
// 			attributes);
// 
// 		dxf.writePoint(*dw,
// 			DL_PointData(10.0, 46.0, 0.0),
// 			attributes);
// 
// 		dxf.writePoint(*dw,
// 			DL_PointData(10.0, 49.0, 0.0),
// 			attributes);

// 		// DIMENSION:
// 		DL_DimensionData dimData(10.0,                  // def point (dimension line pos)
// 			50.0,
// 			0.0,
// 			0,                     // text pos (irrelevant if flag 0x80 (128) set for type
// 			0,
// 			0.0,
// 			0x1,                   // type: aligned with auto text pos (0x80 NOT set)
// 			8,                     // attachment point: bottom center
// 			2,                     // line spacing: exact
// 			1.0,                   // line spacing factor
// 			"",                    // text
// 			"Standard",            // style
// 			0.0,                   // text angle
// 			1.0,                   // linear factor
// 			1.0);                  // dim scale
// 
// 		DL_DimAlignedData dimAlignedData(10.0,          // extension point 1
// 			5.0,
// 			0.0,
// 			30.0,          // extension point 2
// 			5.0,
// 			0.0);
// 
// 		dxf.writeDimAligned(*dw, dimData, dimAlignedData, attributes);

		// end section ENTITIES:
		dw->sectionEnd();
		dxf.writeObjects(*dw, "MY_OBJECTS");
		dxf.writeObjectsEnd(*dw);

		dw->dxfEOF();
		dw->close();
		delete dw;
	}


// 	void convert(DXFSpline* spline, ClipperLib::Path* path)
// 	{
// 		if (!spline || !path)
// 			return;
// 
// 		if (spline->degree < 1 || spline->degree>3) 
// 		{
// 			printf("invalid degree: %d", spline->degree);
// 			return;
// 		}
// 
// 		if (spline->controlPoints.size() < spline->degree + 1) 
// 		{
// 			printf("not enough control points");
// 			return;
// 		}
// 
// 		const size_t nCtrl = spline->nControl;
// 		// order:
// 		const size_t  order = spline->degree + 1;
// 		// resolution:
// 		const size_t  n = 8 * nCtrl;
// 
// 		std::vector<double> h(nCtrl + 1, 1.0);
// 		std::vector<Point> fitPoints(n);
// 
// 		rbspline(nCtrl, order, n, spline->controlPoints, h, spline->knots, fitPoints);
// 
// 		if (n > 0)
// 		{
// 			path->resize(n);
// 			for (size_t i = 0; i < n; ++i)
// 			{
// 				ClipperLib::IntPoint& p = path->at(i);
// 				Point& pp = fitPoints.at(i);
// 				p.X = (ClipperLib::cInt)(1000.0 * pp.x);
// 				p.Y = (ClipperLib::cInt)(1000.0 * pp.y);
// 			}
// 		}
// 	}
// 
// 	void rbspline(size_t nCtrls, size_t order, size_t n,
// 		const std::vector<Point>& ctrlPoints,
// 		const std::vector<double>& h,
// 		const std::vector<double>& knots,
// 		std::vector<Point>& fitPoints)
// 	{
// 		size_t const nplusc = nCtrls + order;
// 
// 		// generate the open knot vector
// 		auto const x = knot(nCtrls, order, knots);
// 
// 		// calculate the points on the rational B-spline curve
// 		double t{ x[0] };
// 		double const step{ (x[nplusc - 1] - t) / (n - 1) };
// 
// 		for (Point& vp : fitPoints)
// 		{
// 			if (x[nplusc - 1] - t < 5e-6)
// 				t = x[nplusc - 1];
// 
// 			// generate the basis function for this value of t
// 			auto const nbasis = rbasis(order, t, nCtrls, x, h);
// 
// 			// generate a point on the curve
// 			for (size_t i = 0; i < nCtrls; i++)
// 			{
// 				vp.x += ctrlPoints[i].x * nbasis[i];
// 				vp.y += ctrlPoints[i].y * nbasis[i];
// 			}
// 			t += step;
// 		}
// 	}
// 
// 	std::vector<double> knot(size_t num, size_t order, const std::vector<double>& knots)
// 	{
// 		if (knots.size() == num + order)
// 		{
// 			//use custom knot vector
// 			return knots;
// 		}
// 
// 		std::vector<double> knotVector(num + order, 0.);
// 		//use uniform knots
// 		std::iota(knotVector.begin() + order, knotVector.begin() + num + 1, 1);
// 		std::fill(knotVector.begin() + num + 1, knotVector.end(), knotVector[num]);
// 		return knotVector;
// 	}
// 
// 	std::vector<double> rbasis(int c, double t, int npts,
// 		const std::vector<double>& x,
// 		const std::vector<double>& h) 
// 	{
// 		int const nplusc = npts + c;
// 
// 		std::vector<double> temp(nplusc, 0.);
// 
// 		// calculate the first order nonrational basis functions n[i]
// 		for (int i = 0; i < nplusc - 1; i++)
// 			if ((t >= x[i]) && (t < x[i + 1])) temp[i] = 1;
// 
// 		/* calculate the higher order nonrational basis functions */
// 
// 		for (int k = 2; k <= c; k++) 
// 		{
// 			for (int i = 0; i < nplusc - k; i++) 
// 			{
// 				// if the lower order basis function is zero skip the calculation
// 				if (temp[i] != 0)
// 					temp[i] = ((t - x[i]) * temp[i]) / (x[i + k - 1] - x[i]);
// 				// if the lower order basis function is zero skip the calculation
// 				if (temp[i + 1] != 0)
// 					temp[i] += ((x[i + k] - t) * temp[i + 1]) / (x[i + k] - x[i + 1]);
// 			}
// 		}
// 
// 		// pick up last point
// 		if (t >= x[nplusc - 1])
// 			temp[npts - 1] = 1;
// 
// 		// calculate sum for denominator of rational basis functions
// 		double sum = 0.;
// 		for (int i = 0; i < npts; i++) 
// 		{
// 			sum += temp[i] * h[i];
// 		}
// 
// 		std::vector<double> r(npts, 0);
// 		// form rational basis functions and put in r vector
// 		if (sum != 0) 
// 		{
// 			for (int i = 0; i < npts; i++)
// 				r[i] = (temp[i] * h[i]) / sum;
// 		}
// 		return r;
// 	}
}