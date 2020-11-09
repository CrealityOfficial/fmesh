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

	void offsetPaths(std::vector<ClipperLib::Path*>& source, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLib::ClipperOffset offset;
		for(ClipperLib::Path* path : source)
			offset.AddPath(*path, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);

		offset.Execute(dest, microDelta);
	}

	void offsetPath(ClipperLib::Path* source, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;
		ClipperLib::cInt z = 0;
		if (source->size() > 0)
			z = source->at(0).Z;

		ClipperLib::ClipperOffset offset;
		offset.AddPath(*source, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);

		offset.Execute(dest, microDelta);

		auto f = [&z](ClipperLib::PolyNode* node){
			for (ClipperLib::IntPoint& p : node->Contour)
				p.Z = z;
		};

		mmesh::loopPolyTree(f, &dest);
	}

	void extendPolyTree(ClipperLib::PolyTree& source, double delta, polyOffsetFunc offsetFunc, ClipperLib::PolyTree& dest)
	{
		extendPolyTree(source, delta, dest);
		polyTreeOffset(dest, offsetFunc);
	}

	void offsetAndExtendPolyTree(ClipperLib::PolyTree& source, double offset, double delta, double z, ClipperLib::PolyTree& dest)
	{
		if (offset == 0.0)
		{
			extendPolyTree(source, delta, dest);
		}
		else
		{
			offsetPolyTree(source, offset, dest);
			extendPolyTree(dest, delta, dest);
		}
		polyNodeFunc func = [&func, &z](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& point : node->Contour)
				point.Z += (int)(1000.0 * z);
		};

		mmesh::loopPolyTree(func, &dest);
	}

	void offsetExterior(ClipperLib::PolyTree& source, double offset)
	{
		std::vector<ClipperLib::Path*> exterior;
		std::vector<ClipperLib::Path*> interior;

		mmesh::seperatePolyTree(&source, exterior, interior);

		size_t outSize = exterior.size();
		std::vector<ClipperLib::PolyTree> outTrees;
		if (outSize > 0)
		{
			outTrees.resize(outSize);
			for (size_t i = 0; i < outSize; ++i)
			{
				offsetPath(exterior.at(i), offset, outTrees.at(i));
			}
		}

		ClipperLib::Clipper clipper;
		polyNodeFunc func = [&func, &clipper](ClipperLib::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
		};
		
		for (size_t i = 0; i < outSize; ++i)
		{
			mmesh::loopPolyTree(func, &outTrees.at(i));
		}

		for(ClipperLib::Path* path : interior)
			clipper.AddPath(*path, ClipperLib::ptClip, true);
		clipper.Execute(ClipperLib::ctUnion, source, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
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