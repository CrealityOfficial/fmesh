#include "triangularization.h"
#include "mmesh/clipper/circurlar.h"
#include "fmesh/generate/wovener.h"
#include "fmesh/contour/polytree.h"

namespace fmesh
{
	bool checkFlag(ClipperLib::PolyNode* node, int flag)
	{
		if (flag == 1 && node->IsHole())
			return false;

		if (flag == 2 && !node->IsHole() && !node->Parent)
			return false;

		int depth = testPolyNodeDepth(node);
		if (flag == 3 && depth != 1 && depth != 4)      // 1, 4
			return false;
	
		if (flag == 4)      // 2, 3
			return false;
		return true;
	}

	Patch* buildFromSamePolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, int flag)
	{
		if (!treeUp || !treeLower)
			return nullptr;

		std::vector<ClipperLib::Path*> pathsUp;
		std::vector<ClipperLib::Path*> pathsLower;
		size_t count = 0;

		auto f = [&count, &pathsUp, &flag](ClipperLib::PolyNode* node) {
			if (!checkFlag(node, flag))
			{
				return;
			}
			size_t size = node->Contour.size();
			if (size > 2)
				count += 2 * size;
			pathsUp.push_back(&node->Contour);
		};

		auto f1 = [&pathsLower, &flag](ClipperLib::PolyNode* node) {
			if (!checkFlag(node, flag))
			{
				return;
			}
			pathsLower.push_back(&node->Contour);
		};

		mmesh::loopPolyTree(f, treeUp);
		if (count == 0)
			return nullptr;

		Patch* patch = new Patch();
		patch->reserve(3 * count);

		mmesh::loopPolyTree(f1, treeLower);
		buildFromPathes(pathsLower, pathsUp, *patch);
		return patch;
	}

	void buildFromDiffPolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag)
	{
		std::vector<ClipperLib::Path*> pathsUp;
		std::vector<ClipperLib::Path*> pathsLower;
		size_t count = 0;
		auto f = [&count, &pathsUp, &flag](ClipperLib::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsUp.push_back(&node->Contour);
		};
		auto f1 = [&pathsLower, &flag](ClipperLib::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsLower.push_back(&node->Contour);
		};

		mmesh::loopPolyTree(f, treeUp);
		mmesh::loopPolyTree(f1, treeLower);

		size_t size = pathsUp.size();
		if (size != pathsLower.size() && size > 0)
			return ;

		std::vector<Patch*> tmp(size);
		for (size_t i = 0; i < size; ++i)
			tmp.at(i) = buildFromDiffPath(pathsLower.at(i), pathsUp.at(i));

		for (Patch* patch : tmp)
			if (patch)
				patches.push_back(patch);
	}

	Patch* buildFromDiffPath(ClipperLib::Path* pathLower, ClipperLib::Path* pathUp)
	{
		Wovener wovener;
		return wovener.woven(pathLower, pathUp);
	}

	void buildFromPathes(std::vector<ClipperLib::Path*>& pathsLower, std::vector<ClipperLib::Path*>& pathsUp, Patch& patch)
	{
		size_t size = pathsUp.size();
		if (size == pathsLower.size())
		{
			for (size_t i = 0; i < size; ++i)
				buildFromPath(pathsUp.at(i), pathsLower.at(i), patch);
		}
	}

	void buildFromPath(ClipperLib::Path* pathLower, ClipperLib::Path* pathUp, Patch& patch)
	{
		size_t size = pathUp->size();
		if (size == pathLower->size() && size > 2)
		{
			for (int i = 0; i < size; ++i)
			{
				int i1 = i;
				int i3 = i;
				int i2 = i1 + 1;
				int i4 = i3 + 1;

				if (i == size - 1)
				{
					i2 = 0;
					i4 = 0;
				}

				patch.push_back(CInt2V(pathUp->at(i1)));
				patch.push_back(CInt2V(pathUp->at(i2)));
				patch.push_back(CInt2V(pathLower->at(i4)));
				patch.push_back(CInt2V(pathUp->at(i1)));
				patch.push_back(CInt2V(pathLower->at(i4)));
				patch.push_back(CInt2V(pathLower->at(i3)));
			}
		}
	}

	void buildFromDiffPolyTree_drum(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, std::vector<Patch*>& patches, int flag, ClipperLib::PolyTree& out)
	{
		std::vector<ClipperLib::Path*> pathsUp;
		std::vector<ClipperLib::Path*> pathsLower;
		size_t count = 0;
		auto f = [&count, &pathsUp, &flag](ClipperLib::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsUp.push_back(&node->Contour);
		};
		auto f1 = [&pathsLower, &flag](ClipperLib::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsLower.push_back(&node->Contour);
		};

		mmesh::loopPolyTree(f, treeUp);
		mmesh::loopPolyTree(f1, treeLower);

		size_t size = pathsUp.size();
		//if (size != pathsLower.size() && size > 0)
		{
			fmesh::xor2PolyTrees(treeUp, treeLower, out);
			return;
		}

		std::vector<Patch*> tmp(size);
		for (size_t i = 0; i < size; ++i)
			tmp.at(i) = buildFromDiffPath(pathsLower.at(i), pathsUp.at(i));

		for (Patch* patch : tmp)
			if (patch)
				patches.push_back(patch);
	}

	void buildXORFrom2PolyTree(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, ClipperLib::PolyTree& out, int flag)
	{
		std::vector<ClipperLib::Path*> pathsUp;
		std::vector<ClipperLib::Path*> pathsLower;
		size_t count = 0;
		auto f = [&count, &pathsUp, &flag](ClipperLib::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsUp.push_back(&node->Contour);
		};
		auto f1 = [&pathsLower, &flag](ClipperLib::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsLower.push_back(&node->Contour);
		};

		mmesh::loopPolyTree(f, treeUp);
		mmesh::loopPolyTree(f1, treeLower);

		fmesh::xor2PolyTrees(treeUp, treeLower, out);
	}
}