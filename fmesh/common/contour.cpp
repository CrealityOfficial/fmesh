#include "contour.h"

namespace fmesh
{
	void convert(const ClipperLibXYZ::Paths& inPaths, ContourPaths& outPaths)
	{
		size_t size = inPaths.size();
		if (size > 0)
		{
			outPaths.resize(size);
			for (size_t i = 0; i < size; ++i)
				convert(inPaths.at(i), outPaths.at(i));
		}
	}

	void convert(const ClipperLibXYZ::Path& inPath, ContourPath& outPath)
	{
		size_t size = inPath.size();
		if (size > 0)
		{
			outPath.resize(size);
			for (size_t i = 0; i < size; ++i)
			{
				const ClipperLibXYZ::IntPoint& point = inPath.at(i);
				trimesh::dvec2& v = outPath.at(i);
				v.x = (double)point.X / 1000.0;
				v.y = (double)point.Y / 1000.0;
			}
		}
	}

	void convert(const ContourPaths& inPaths, ClipperLibXYZ::Paths& outPaths)
	{
		size_t size = inPaths.size();
		if (size > 0)
		{
			outPaths.resize(size);
			for (size_t i = 0; i < size; ++i)
				convert(inPaths.at(i), outPaths.at(i));
		}
	}

	void convert(const ContourPath& inPath, ClipperLibXYZ::Path& outPath)
	{
		size_t size = inPath.size();
		if (size > 0)
		{
			outPath.resize(size);
			for (size_t i = 0; i < size; ++i)
			{
				const trimesh::dvec2& v = inPath.at(i);
				ClipperLibXYZ::IntPoint& point = outPath.at(i);
				point.X = (int)(v.x * 1000.0);
				point.Y = (int)(v.y * 1000.0);
			}
		}
	}

	void calculateContourBox(ContourPaths* paths, trimesh::dvec3& outMin, trimesh::dvec3& outMax)
	{
		outMin = trimesh::dvec3(99999999.0, 99999999.0, 0.0);
		outMax = trimesh::dvec3(-99999999.0, -99999999.0, 0.0);
		for (ContourPath& path : *paths)
		{
			for (trimesh::dvec2& p : path)
			{
				outMin.x = std::min(outMin.x, p.x);
				outMin.y = std::min(outMin.y, p.y);
				outMax.x = std::max(outMax.x, p.x);
				outMax.y = std::max(outMax.y, p.y);
			}
		}
	}
}