#include "contour.h"
#include "fmesh/font/outline.h"
#include <fstream>

namespace fmesh
{
	ClipperLibXYZ::PolyTree* convertOutline2PolyTree(Outline* outline)
	{
		if (!outline) return nullptr;

		ClipperLibXYZ::PolyTree* polyTree = new ClipperLibXYZ::PolyTree();
		ClipperLibXYZ::Clipper clipper;
		clipper.AddPaths(outline->pathes(), ClipperLibXYZ::ptSubject, true);
		if (clipper.Execute(ClipperLibXYZ::ctUnion, *polyTree, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero))
			return polyTree;

		delete polyTree;
		return nullptr;
	}

	ClipperLibXYZ::PolyTree* convertPaths2PolyTree(ClipperLibXYZ::Paths* paths)
	{
		if (!paths) return nullptr;

		ClipperLibXYZ::PolyTree* polyTree = new ClipperLibXYZ::PolyTree();
		ClipperLibXYZ::Clipper clipper;
		clipper.AddPaths(*paths, ClipperLibXYZ::ptSubject, true);
		if (clipper.Execute(ClipperLibXYZ::ctUnion, *polyTree, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero))
			return polyTree;

		delete polyTree;
		return nullptr;
	}

	ClipperLibXYZ::PolyTree* convertPolyTrees2PolyTree(std::vector<ClipperLibXYZ::PolyTree*>& trees)
	{
		if (trees.size() == 0)
			return nullptr;

		ClipperLibXYZ::PolyTree* polyTree = new ClipperLibXYZ::PolyTree();
		ClipperLibXYZ::Clipper clipper;

		polyNodeFunc func = [&func, &clipper](ClipperLibXYZ::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptSubject, true);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		for(ClipperLibXYZ::PolyTree* tree : trees)
			func(tree);

		if (clipper.Execute(ClipperLibXYZ::ctUnion, *polyTree, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero))
			return polyTree;

		delete polyTree;
		return nullptr;
	}

	ClipperLibXYZ::PolyTree* merge2PolyTrees(ClipperLibXYZ::PolyTree* outer, ClipperLibXYZ::PolyTree* inner)
	{
		if (!outer || !inner)
			return nullptr;

		ClipperLibXYZ::PolyTree* polyTree = new ClipperLibXYZ::PolyTree();
		ClipperLibXYZ::Clipper clipper;

		polyNodeFunc func1 = [&func1, &clipper](ClipperLibXYZ::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func1(n);
		};

		func1(outer);

		polyNodeFunc func2 = [&func2, &clipper](ClipperLibXYZ::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptSubject, true);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func2(n);
		};

		func2(inner);

		if (clipper.Execute(ClipperLibXYZ::ctXor, *polyTree, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero))
			return polyTree;

		delete polyTree;
		return nullptr;
	}

	ClipperLibXYZ::PolyTree* extendPaths2PolyTree(ClipperLibXYZ::Paths* paths, float delta)
	{
		if (!paths) return nullptr;

		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::PolyTree* polyTree = new ClipperLibXYZ::PolyTree();
		ClipperLibXYZ::ClipperOffset offset;
		offset.AddPaths(*paths, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedLine);
		offset.Execute(*polyTree, microDelta);
		return polyTree;
	}

	ClipperLibXYZ::PolyTree* extendPolyTree2PolyTree(ClipperLibXYZ::PolyTree* poly, float delta)
	{
		if (!poly) return nullptr;

		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::PolyTree* polyTree = new ClipperLibXYZ::PolyTree();
		ClipperLibXYZ::ClipperOffset offset;
		
		polyNodeFunc func = [&func, &offset](ClipperLibXYZ::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedLine);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		func(poly);

		offset.Execute(*polyTree, microDelta);
		return polyTree;
	}

	ClipperLibXYZ::PolyTree* offsetPolyTree(ClipperLibXYZ::PolyTree* poly, float delta)
	{
		if (!poly) return nullptr;

		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::PolyTree* polyTree = new ClipperLibXYZ::PolyTree();
		ClipperLibXYZ::ClipperOffset offset;

		polyNodeFunc func = [&func, &offset](ClipperLibXYZ::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		func(poly);
		offset.Execute(*polyTree, microDelta);
		return polyTree;
	}

	void savePolyTree(ClipperLibXYZ::PolyTree* poly, const char* fileName)
	{
		std::fstream out(fileName, std::ios::binary | std::ios::out);
		if (out.is_open())
		{
			polyNodeFunc func = [&out, &func](ClipperLibXYZ::PolyNode* node) {
				int size = (int)node->Contour.size();
				out.write((const char*)&size, sizeof(int));
				for (ClipperLibXYZ::IntPoint& point : node->Contour)
					out.write((const char*)(&point), sizeof(ClipperLibXYZ::IntPoint));

				for (ClipperLibXYZ::PolyNode* n : node->Childs)
					func(n);
			};

			func(poly);
		}
		out.close();
	}

	ClipperLibXYZ::PolyTree* loadPolyTree(const char* fileName)
	{
		ClipperLibXYZ::PolyTree* tree = nullptr;
		std::fstream in(fileName, std::ios::binary | std::ios::in);
		if (in.is_open())
		{
			ClipperLibXYZ::Clipper clipper;
			tree = new ClipperLibXYZ::PolyTree();
			while (!in.eof() && in.good())
			{
				int size = 0;
				in.read((char*)&size, sizeof(int));
				if (size > 0)
				{
					ClipperLibXYZ::Path path;
					for (int i = 0; i < size; ++i)
					{
						ClipperLibXYZ::IntPoint point;
						in.read((char*)(&point), sizeof(ClipperLibXYZ::IntPoint));
						path.push_back(point);
					}

					clipper.AddPath(path, ClipperLibXYZ::ptClip, true);
				}
			}

			if (!clipper.Execute(ClipperLibXYZ::ctUnion, *tree, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero))
			{
				delete tree;
				tree = nullptr;
			}
		}

		in.close();
		return tree;
	}

	ClipperLibXYZ::PolyTree* offsetAndExtend(ClipperLibXYZ::PolyTree* poly, double offset, double extend)
	{
		ClipperLibXYZ::PolyTree* tree = fmesh::offsetPolyTree(poly, offset);
		ClipperLibXYZ::PolyTree* extendTree = fmesh::extendPolyTree2PolyTree(tree, extend);
		delete tree;

		return extendTree;
	}
}