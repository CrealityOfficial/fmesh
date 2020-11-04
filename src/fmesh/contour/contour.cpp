#include "contour.h"
#include "fmesh/font/outline.h"
#include <fstream>

namespace fmesh
{
	ClipperLib::PolyTree* convertOutline2PolyTree(Outline* outline)
	{
		if (!outline) return nullptr;

		ClipperLib::PolyTree* polyTree = new ClipperLib::PolyTree();
		ClipperLib::Clipper clipper;
		clipper.AddPaths(outline->pathes(), ClipperLib::ptSubject, true);
		if (clipper.Execute(ClipperLib::ctUnion, *polyTree, ClipperLib::pftNonZero, ClipperLib::pftNonZero))
			return polyTree;

		delete polyTree;
		return nullptr;
	}

	ClipperLib::PolyTree* convertPaths2PolyTree(ClipperLib::Paths* paths)
	{
		if (!paths) return nullptr;

		ClipperLib::PolyTree* polyTree = new ClipperLib::PolyTree();
		ClipperLib::Clipper clipper;
		clipper.AddPaths(*paths, ClipperLib::ptSubject, true);
		if (clipper.Execute(ClipperLib::ctUnion, *polyTree, ClipperLib::pftNonZero, ClipperLib::pftNonZero))
			return polyTree;

		delete polyTree;
		return nullptr;
	}

	ClipperLib::PolyTree* convertPolyTrees2PolyTree(std::vector<ClipperLib::PolyTree*>& trees)
	{
		if (trees.size() == 0)
			return nullptr;

		ClipperLib::PolyTree* polyTree = new ClipperLib::PolyTree();
		ClipperLib::Clipper clipper;

		polyNodeFunc func = [&func, &clipper](ClipperLib::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLib::ptSubject, true);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		for(ClipperLib::PolyTree* tree : trees)
			func(tree);

		if (clipper.Execute(ClipperLib::ctUnion, *polyTree, ClipperLib::pftNonZero, ClipperLib::pftNonZero))
			return polyTree;

		delete polyTree;
		return nullptr;
	}

	ClipperLib::PolyTree* merge2PolyTrees(ClipperLib::PolyTree* outer, ClipperLib::PolyTree* inner)
	{
		if (!outer || !inner)
			return nullptr;

		ClipperLib::PolyTree* polyTree = new ClipperLib::PolyTree();
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

		if (clipper.Execute(ClipperLib::ctXor, *polyTree, ClipperLib::pftNonZero, ClipperLib::pftNonZero))
			return polyTree;

		delete polyTree;
		return nullptr;
	}

	ClipperLib::PolyTree* extendPaths2PolyTree(ClipperLib::Paths* paths, float delta)
	{
		if (!paths) return nullptr;

		double microDelta = 1000.0 * delta;

		ClipperLib::PolyTree* polyTree = new ClipperLib::PolyTree();
		ClipperLib::ClipperOffset offset;
		offset.AddPaths(*paths, ClipperLib::jtRound, ClipperLib::EndType::etClosedLine);
		offset.Execute(*polyTree, microDelta);
		return polyTree;
	}

	ClipperLib::PolyTree* extendPolyTree2PolyTree(ClipperLib::PolyTree* poly, float delta)
	{
		if (!poly) return nullptr;

		double microDelta = 1000.0 * delta;

		ClipperLib::PolyTree* polyTree = new ClipperLib::PolyTree();
		ClipperLib::ClipperOffset offset;
		
		polyNodeFunc func = [&func, &offset](ClipperLib::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedLine);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		func(poly);

		offset.Execute(*polyTree, microDelta);
		return polyTree;
	}

	ClipperLib::PolyTree* offsetPolyTree(ClipperLib::PolyTree* poly, float delta)
	{
		if (!poly) return nullptr;

		double microDelta = 1000.0 * delta;

		ClipperLib::PolyTree* polyTree = new ClipperLib::PolyTree();
		ClipperLib::ClipperOffset offset;

		polyNodeFunc func = [&func, &offset](ClipperLib::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		func(poly);
		offset.Execute(*polyTree, microDelta);
		return polyTree;
	}

	void savePolyTree(ClipperLib::PolyTree* poly, const char* fileName)
	{
		std::fstream out(fileName, std::ios::binary | std::ios::out);
		if (out.is_open())
		{

		}

		out.close();
	}

	ClipperLib::PolyTree* loadPolyTree(const char* fileName)
	{
		return nullptr;
	}

	ClipperLib::PolyTree* offsetAndExtend(ClipperLib::PolyTree* poly, double offset, double extend)
	{
		ClipperLib::PolyTree* tree = fmesh::offsetPolyTree(poly, offset);
		ClipperLib::PolyTree* extendTree = fmesh::extendPolyTree2PolyTree(tree, extend);
		delete tree;

		return extendTree;
	}
}