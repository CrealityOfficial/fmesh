#include "path.h"

namespace fmesh
{
	void scalePath2ExpectLen(ClipperLib::Paths* paths, double expectLen) //mm
	{
		if (expectLen <= 0.0)
			return;

		ClipperLib::IntPoint bmin;
		ClipperLib::IntPoint bmax;
		calculatePathBox(paths, bmin, bmax);
		//scale
		double xLen = (double)(bmax.X - bmin.X);
		double yLen = (double)(bmax.Y - bmin.Y);
		double scale = 1000.0 * expectLen / std::min(xLen, yLen);
		for (ClipperLib::Path& path : *paths)
		{
			for (ClipperLib::IntPoint& p : path)
			{
				ClipperLib::IntPoint ep;
				ep.X = (int)(scale * ((double)p.X - (double)bmin.X) + (double)bmin.X);
				ep.Y = (int)(scale * ((double)p.Y - (double)bmin.Y) + (double)bmin.Y);

				p = ep;
			}
		}
	}

	void calculatePathBox(ClipperLib::Paths* paths, ClipperLib::IntPoint& outMin, ClipperLib::IntPoint& outMax)
	{
		outMin = ClipperLib::IntPoint(99999999, 99999999, 99999999);
		outMax = ClipperLib::IntPoint(-99999999, -99999999, -99999999);
		for (ClipperLib::Path& path : *paths)
		{
			for (ClipperLib::IntPoint& p : path)
			{
				outMin.X = std::min(outMin.X, p.X);
				outMin.Y = std::min(outMin.Y, p.Y);
				outMin.Z = std::min(outMin.Z, p.Z);
				outMax.X = std::max(outMax.X, p.X);
				outMax.Y = std::max(outMax.Y, p.Y);
				outMax.Z = std::max(outMax.Z, p.Z);
			}
		}

	}
}