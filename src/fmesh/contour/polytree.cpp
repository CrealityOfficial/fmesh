#include "polytree.h"
#include "mmesh/clipper/circurlar.h"

namespace fmesh
{
	void convertPaths2PolyTree(ClipperLib::Paths* paths, ClipperLib::PolyTree& polyTree)
	{
		ClipperLib::Clipper clipper;
		clipper.AddPaths(*paths, ClipperLib::ptSubject, true);
		clipper.Execute(ClipperLib::ctUnion, polyTree, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	}

	void extendPolyTree(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLib::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLib::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedLine);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);
		offset.Execute(dest, microDelta);
	}

	void offsetPolyTree(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLib::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLib::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);
		offset.Execute(dest, microDelta);
	}

	void extendPolyTree(ClipperLib::PolyTree& source, double delta, polyOffsetFunc offsetFunc, ClipperLib::PolyTree& dest)
	{
		extendPolyTree(source, delta, dest);
		polyTreeOffset(dest, offsetFunc);
	}

	void offsetAndExtendPolyTree(ClipperLib::PolyTree& source, double offset, double delta, double z, ClipperLib::PolyTree& dest)
	{
		offsetPolyTree(source, offset, dest);
		extendPolyTree(dest, delta, dest);

		polyNodeFunc func = [&func, &z](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& point : node->Contour)
				point.Z += (int)(1000.0 * z);
		};

		mmesh::loopPolyTree(func, &dest);
	}

	void polyTreeOffset(ClipperLib::PolyTree& source, polyOffsetFunc offsetFunc)
	{
		polyNodeFunc func = [&func, &offsetFunc](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& point : node->Contour)
				point = offsetFunc(point);
		};

		mmesh::loopPolyTree(func, &source);
	}

	void xor2PolyTrees(ClipperLib::PolyTree* outer, ClipperLib::PolyTree* inner, ClipperLib::PolyTree& out)
	{
		if (!outer || !inner)
			return;

		ClipperLib::Clipper clipper;

		polyNodeFunc func1 = [&func1, &clipper](ClipperLib::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);

			for (ClipperLib::PolyNode* n : node->Childs)
				func1(n);
		};

		func1(outer);

		polyNodeFunc func2 = [&func2, &clipper](ClipperLib::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLib::ptSubject, true);

			for (ClipperLib::PolyNode* n : node->Childs)
				func2(n);
		};

		func2(inner);

		clipper.Execute(ClipperLib::ctXor, out, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	}
}