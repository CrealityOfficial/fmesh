#include "triangularization.h"
#include "fmesh/clipper/circurlar.h"
#include "fmesh/generate/wovener.h"
#include "fmesh/contour/polytree.h"
#include "fmesh/contour/path.h"

#include "fmesh/contour/contour.h"
namespace fmesh
{
	bool checkFlag(ClipperLibXYZ::PolyNode* node, int flag)
	{
		int depth = testPolyNodeDepth(node);
		if (flag == 1 && node->IsHole())
			return false;

		if (flag == 2 && depth != 2 && depth != 3 && depth != 6 && depth != 7)   //23
			return false;

		if (flag == 3 && depth != 1 && depth != 4 && depth != 5 && depth != 8)      // 14
			return false;
	
		if (flag == 4)      // 2, 3
			return false;
		return true;
	}

	Patch* buildFromSamePolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, int flag)
	{
		if (!treeUp || !treeLower)
			return nullptr;

		std::vector<ClipperLibXYZ::Path*> pathsUp;
		std::vector<ClipperLibXYZ::Path*> pathsLower;
		size_t count = 0;

		auto f = [&count, &pathsUp, &flag](ClipperLibXYZ::PolyNode* node) {
			if (!checkFlag(node, flag))
			{
				return;
			}
			size_t size = node->Contour.size();
			if (size > 2)
				count += 2 * size;
			pathsUp.push_back(&node->Contour);
		};

		auto f1 = [&pathsLower, &flag](ClipperLibXYZ::PolyNode* node) {
			if (!checkFlag(node, flag))
			{
				return;
			}
			pathsLower.push_back(&node->Contour);
		};

		fmesh::loopPolyTree(f, treeUp);
		if (count == 0)
			return nullptr;

		Patch* patch = new Patch();
		patch->reserve(3 * count);

		fmesh::loopPolyTree(f1, treeLower);
		buildFromPathes(pathsLower, pathsUp, *patch);
		return patch;
	}

	void buildFromDiffPolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag)
	{
		std::vector<ClipperLibXYZ::Path*> pathsUp;
		std::vector<ClipperLibXYZ::Path*> pathsLower;
		size_t count = 0;
		auto f = [&count, &pathsUp, &flag](ClipperLibXYZ::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsUp.push_back(&node->Contour);
		};
		auto f1 = [&pathsLower, &flag](ClipperLibXYZ::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsLower.push_back(&node->Contour);
		};

		fmesh::loopPolyTree(f, treeUp);
		fmesh::loopPolyTree(f1, treeLower);

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

	void buildFromDiffPolyTree_SameAndDiffSafty(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag, ClipperLibXYZ::PolyTree& out, double delta)
	{		
		if (GetPolyCount(treeLower) == GetPolyCount(treeUp))
			buildFromDiffPolyTreeSafty(treeLower, treeUp, patches, delta, flag);
		else
			fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);			
	}

	void buildFromSameAndDiff(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp,
		std::vector<Patch*>& patches, int flag, double delta, ClipperLibXYZ::PolyTree& out, ClipperLibXYZ::PolyTree& Inner)
	{
//#ifdef 1
		////save path
		//static int j = 0;
		//char a[128];
		//sprintf(a, "%d", j++);
		//std::string file = "f:/0/p";
		//file += a;
		//savePolyTree(treeLower, file.c_str());
		//file += "_1";
		//savePolyTree(treeUp, file.c_str());
		////save end
//#endif // 1
		
		if (GetPolyCount(treeLower) == GetPolyCount(treeUp))
			buildFromDiffPolyTreeSafty(treeLower, treeUp, patches, delta, flag);
		else
		{
			fmesh::xor2PolyTrees(treeUp, treeLower, out, flag);
		}
	}

	void buildFromDiffPolyTreeSafty(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp,
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
		ClipperLibXYZ::IntPoint bmin;
		ClipperLibXYZ::IntPoint bmax;
		float area;
	};
	typedef std::function<bool(CompInfo& info1, CompInfo& info2)> sortFunc;
	void findPolyTreePairFromNode(ClipperLibXYZ::PolyNode* nodeLower, ClipperLibXYZ::PolyNode* nodeUp,
		std::vector<PolyTreeOppoPair>& pairs, double delta)
	{
		ClipperLibXYZ::PolyNodes inner = nodeLower->Childs;
		ClipperLibXYZ::PolyNodes outer = nodeUp->Childs;

		ClipperLibXYZ::cInt iDelta = (int)(1000.0 * delta);
		if (inner.size() > 0 && inner.size() == outer.size())
		{
			size_t size = inner.size();
			std::vector<CompInfo> iInfos(size);
			std::vector<CompInfo> oInfos(size);
			for (size_t i = 0; i < size; ++i)
			{
				pathBox(inner.at(i)->Contour, iInfos.at(i).bmin, iInfos.at(i).bmax);
				pathBox(outer.at(i)->Contour, oInfos.at(i).bmin, oInfos.at(i).bmax);
				//iInfos.at(i).area = ClipperLibXYZ::Area(inner.at(i)->Contour);
				//oInfos.at(i).area = ClipperLibXYZ::Area(outer.at(i)->Contour);
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

				ClipperLibXYZ::PolyNode* inode = inner.at(index1);
				ClipperLibXYZ::PolyNode* onode = outer.at(index2);
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

					ClipperLibXYZ::PolyNode* inode = inner.at(index1);
					ClipperLibXYZ::PolyNode* onode = outer.at(index2);

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

	Patch* buildFromDiffPath(ClipperLibXYZ::Path* pathLower, ClipperLibXYZ::Path* pathUp)
	{
		Wovener wovener;
		return wovener.woven(pathLower, pathUp);
	}

	void buildFromPathes(std::vector<ClipperLibXYZ::Path*>& pathsLower, std::vector<ClipperLibXYZ::Path*>& pathsUp, Patch& patch)
	{
		size_t size = pathsUp.size();
		if (size == pathsLower.size())
		{
			for (size_t i = 0; i < size; ++i)
				buildFromPath(pathsUp.at(i), pathsLower.at(i), patch);
		}
	}

	void buildFromPath(ClipperLibXYZ::Path* pathLower, ClipperLibXYZ::Path* pathUp, Patch& patch)
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

	void buildXORFrom2PolyTree(ClipperLibXYZ::PolyTree* treeLower, ClipperLibXYZ::PolyTree* treeUp, ClipperLibXYZ::PolyTree& out, int flag)
	{
		std::vector<ClipperLibXYZ::Path*> pathsUp;
		std::vector<ClipperLibXYZ::Path*> pathsLower;
		size_t count = 0;
		auto f = [&count, &pathsUp, &flag](ClipperLibXYZ::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsUp.push_back(&node->Contour);
		};
		auto f1 = [&pathsLower, &flag](ClipperLibXYZ::PolyNode* node) {
			if (!checkFlag(node, flag))
				return;
			pathsLower.push_back(&node->Contour);
		};

		fmesh::loopPolyTree(f, treeUp);
		fmesh::loopPolyTree(f1, treeLower);

		fmesh::xor2PolyTrees(treeUp, treeLower, out);
	}
}