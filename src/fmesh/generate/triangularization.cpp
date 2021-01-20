#include "triangularization.h"
#include "mmesh/clipper/circurlar.h"
#include "fmesh/generate/wovener.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/contour/path.h"
namespace fmesh
{
	bool checkFlag(ClipperLib::PolyNode* node, int flag)
	{
		int depth = testPolyNodeDepth(node);
		if (flag == 1 && node->IsHole())
			return false;

		if (flag == 2 && depth != 2 && depth != 3)
			return false;

		if (flag == 3 && depth != 1 && depth != 4 && depth != 5 && depth != 8)      // 1, 4
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

	void buildFromDiffPolyTree_SameAndDiffSafty(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag, ClipperLib::PolyTree& out, double delta)
	{
// 		std::vector<ClipperLib::Path*> pathsUp;
// 		std::vector<ClipperLib::Path*> pathsLower;
// 		size_t count = 0;
// 		auto f = [&count, &pathsUp, &flag](ClipperLib::PolyNode* node) {
// 			if (!checkFlag(node, flag))
// 				return;
// 			pathsUp.push_back(&node->Contour);
// 		};
// 		auto f1 = [&pathsLower, &flag](ClipperLib::PolyNode* node) {
// 			if (!checkFlag(node, flag))
// 				return;
// 			pathsLower.push_back(&node->Contour);
// 		};
// 
// 		mmesh::loopPolyTree(f, treeUp);
// 		mmesh::loopPolyTree(f1, treeLower);
		
		if (GetPolyCount(treeLower) == GetPolyCount(treeUp))
			buildFromDiffPolyTreeSafty(treeLower, treeUp, patches, delta, flag);
		else
			fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);		
	}

	void buildFromDiffPolyTreeSafty(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp,
		std::vector<Patch*>& patches, double delta, int flag)
	{
		std::vector<PolyTreeOppoPair> sources;
		PolyTreeOppoPair startPair;

		startPair.lower = treeLower;
		startPair.upper = treeUp;
		sources.push_back(startPair);
		std::vector<PolyTreeOppoPair> tmp;
		std::vector<PolyTreeOppoPair> pairs;

		while (sources.size() > 0)
		{
			for(PolyTreeOppoPair& pair : sources)
				findPolyTreePairFromNode(pair.lower, pair.upper, tmp, delta);

			for (PolyTreeOppoPair& pair : tmp)
			{
				if(checkFlag(pair.upper, flag)
					&& checkFlag(pair.lower, flag))
					pairs.push_back(pair);
			}

			tmp.swap(sources);
			tmp.clear();
		}

		size_t size = pairs.size();
		if (size > 0)
		{
			std::vector<Patch*> tmp(size);
			for (size_t i = 0; i < size; ++i)
				tmp.at(i) = buildFromDiffPath(&pairs.at(i).lower->Contour, &pairs.at(i).upper->Contour);

			for (Patch* patch : tmp)
				if (patch)
					patches.push_back(patch);
		}
	}

	struct CompInfo
	{
		ClipperLib::IntPoint bmin;
		ClipperLib::IntPoint bmax;
		float area;
	};
	typedef std::function<bool(CompInfo& info1, CompInfo& info2)> sortFunc;
	void findPolyTreePairFromNode(ClipperLib::PolyNode* nodeLower, ClipperLib::PolyNode* nodeUp,
		std::vector<PolyTreeOppoPair>& pairs, double delta)
	{
		ClipperLib::PolyNodes inner = nodeLower->Childs;
		ClipperLib::PolyNodes outer = nodeUp->Childs;

		ClipperLib::cInt iDelta = (int)(1000.0 * delta);
		if (inner.size() > 0 && inner.size() == outer.size())
		{
			size_t size = inner.size();
			std::vector<CompInfo> iInfos(size);
			std::vector<CompInfo> oInfos(size);
			for (size_t i = 0; i < size; ++i)
			{
				pathBox(inner.at(i)->Contour, iInfos.at(i).bmin, iInfos.at(i).bmax);
				pathBox(outer.at(i)->Contour, oInfos.at(i).bmin, oInfos.at(i).bmax);
				//iInfos.at(i).area = ClipperLib::Area(inner.at(i)->Contour);
				//oInfos.at(i).area = ClipperLib::Area(outer.at(i)->Contour);
			}
			std::vector<int> imapIndex;
			for (size_t i = 0; i < size; ++i)
				imapIndex.push_back((int)i);
			std::vector<int> omapIndex = imapIndex;

			auto test = [](CompInfo& info1, CompInfo& info2)->bool {
				if (info1.bmin.X < info2.bmin.X)
					return true;
				if (info1.bmin.X > info2.bmin.X)
					return false;

				if (info1.bmin.Y < info2.bmin.Y)
					return true;
				if (info1.bmin.Y > info2.bmin.Y)
					return false;

				if (info1.bmax.X < info2.bmax.X)
					return true;
				if (info1.bmax.X > info2.bmax.X)
					return false;

				if (info1.bmax.Y < info2.bmax.Y)
					return true;
				if (info1.bmax.Y > info2.bmax.Y)
					return false;

				return false;
			};
			std::sort(imapIndex.begin(), imapIndex.end(), [&iInfos, &test](int i1, int i2)->bool {
				CompInfo& info1 = iInfos.at(i1);
				CompInfo& info2 = iInfos.at(i2);
				return test(info1, info2);
				});
			std::sort(omapIndex.begin(), omapIndex.end(), [&oInfos, &test](int i1, int i2)->bool {
				CompInfo& info1 = oInfos.at(i1);
				CompInfo& info2 = oInfos.at(i2);
				return test(info1, info2);
				});

			std::vector<int> secondI;
			std::vector<int> secondO;
			for (size_t i = 0; i < size; ++i)
			{
				int index1 = imapIndex.at(i);
				int index2 = omapIndex.at(i);

				ClipperLib::PolyNode* inode = inner.at(index1);
				ClipperLib::PolyNode* onode = outer.at(index2);
				CompInfo& iinfo = iInfos.at(index1);
				CompInfo& oinfo = oInfos.at(index2);

				if (onode && onode->ChildCount() == inode->ChildCount())
				{
					if (std::abs(iinfo.bmin.X - oinfo.bmin.X) > iDelta ||
						std::abs(iinfo.bmin.Y - oinfo.bmin.Y) > iDelta ||
						std::abs(iinfo.bmax.X - oinfo.bmax.X) > iDelta ||
						std::abs(iinfo.bmax.Y - oinfo.bmax.Y) > iDelta)
					{
						secondI.push_back(index1);
						secondO.push_back(index2);
						continue;
					}
					PolyTreeOppoPair pair;
					pair.lower = inode;
					pair.upper = onode;
					pairs.push_back(pair);
				}
			}

			if (secondI.size() > 0)
			{
				auto secondTest = [](CompInfo& info1, CompInfo& info2)->bool {
					if (info1.bmin.Y < info2.bmin.Y)
						return true;
					if (info1.bmin.Y > info2.bmin.Y)
						return false;

					if (info1.bmin.X < info2.bmin.X)
						return true;
					if (info1.bmin.X > info2.bmin.X)
						return false;

					if (info1.bmax.Y < info2.bmax.Y)
						return true;
					if (info1.bmax.Y > info2.bmax.Y)
						return false;

					if (info1.bmax.X < info2.bmax.X)
						return true;
					if (info1.bmax.X > info2.bmax.X)
						return false;

					return false;
				};
				std::sort(secondI.begin(), secondI.end(), [&iInfos, &secondTest](int i1, int i2)->bool {
					CompInfo& info1 = iInfos.at(i1);
					CompInfo& info2 = iInfos.at(i2);
					return secondTest(info1, info2);
					});
				std::sort(secondO.begin(), secondO.end(), [&oInfos, &secondTest](int i1, int i2)->bool {
					CompInfo& info1 = oInfos.at(i1);
					CompInfo& info2 = oInfos.at(i2);
					return secondTest(info1, info2);
					});

				for (size_t i = 0; i < secondI.size(); ++i)
				{
					int index1 = secondI.at(i);
					int index2 = secondO.at(i);

					ClipperLib::PolyNode* inode = inner.at(index1);
					ClipperLib::PolyNode* onode = outer.at(index2);

					if (onode && onode->ChildCount() == inode->ChildCount())
					{
						PolyTreeOppoPair pair;
						pair.lower = inode;
						pair.upper = onode;
						pairs.push_back(pair);
					}
				}
			}
		}
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
			fmesh::xor2PolyTrees(treeUp, treeLower, out,flag);
			return;
		}

		std::vector<Patch*> tmp(size);
		for (size_t i = 0; i < size; ++i)
			tmp.at(i) = buildFromDiffPath(pathsLower.at(i), pathsUp.at(i));

		for (Patch* patch : tmp)
			if (patch)
				patches.push_back(patch);
	}

	void buildFromDiffPolyTree_SameAndDiff(ClipperLib::PolyTree* treeLower, ClipperLib::PolyTree* treeUp, std::vector<Patch*>& patches, int flag, ClipperLib::PolyTree& out)
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
		{
			//diff
			fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);
			return;
		}

		//same
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