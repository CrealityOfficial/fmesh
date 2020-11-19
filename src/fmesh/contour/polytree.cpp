#include "polytree.h"
#include "mmesh/clipper/circurlar.h"
#include "fmesh/skeleton/tridegline.h"
#include "fmesh/roof/roof.h"

#include "fmesh/generate/polyfiller.h"

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
			offset.AddPath(node->Contour, ClipperLib::jtSquare, ClipperLib::EndType::etClosedLine);

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
			offset.AddPath(node->Contour, ClipperLib::jtSquare, ClipperLib::EndType::etClosedPolygon);

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
		std::vector<int> iexterior;
		std::vector<int> iinterior;

		mmesh::seperatePolyTree(&source, exterior, interior, iexterior, iinterior);

		if(iexterior.size()>0 &&
			iexterior.size() == iinterior.size())
		{
			for (size_t i = 0; i < iexterior.size(); ++i)
			{
				ClipperLib::Path* tmp = exterior.at(iexterior[i]-1);
				exterior.at(iexterior[i] - 1) = interior.at(iinterior[i]-1);
				
				size_t size = tmp->size();
				ClipperLib::Path* invertPath=new ClipperLib::Path(size);
				for (size_t i = 0; i < size; ++i)
					invertPath->at(i) = tmp->at(size - i - 1);
				interior.at(iinterior[i] - 1) = invertPath;
			}
		}
	
		size_t outSize = exterior.size();
		std::vector<ClipperLib::PolyTree> outTrees;
		if (outSize > 0)
		{
			outTrees.resize(outSize);
			for (size_t i = 0; i < outSize; ++i)
			{
				for (size_t j = 0; j < iinterior.size(); j++)
				{
					if (i == iinterior[j]-1)
					{
						offset = -offset;
						break;
					}
				}
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

	void offsetExteriorInner(ClipperLib::PolyTree& source, double offset)
	{
		std::vector<ClipperLib::Path*> exterior;
		std::vector<ClipperLib::Path*> interior;
		std::vector<int> iexterior;
		std::vector<int> iinterior;

		mmesh::seperatePolyTree(&source, exterior, interior, iexterior, iinterior);

		if (iexterior.size() > 0 &&
			iexterior.size() == iinterior.size())
		{
			for (size_t i = 0; i < iexterior.size(); ++i)
			{
				ClipperLib::Path* tmp = interior.at(iinterior[i] - 1);
				interior.at(iinterior[i] - 1) = exterior.at(iexterior[i] - 1);

				size_t size = tmp->size();
				ClipperLib::Path* invertPath = new ClipperLib::Path(size);
				for (size_t i = 0; i < size; ++i)
					invertPath->at(i) = tmp->at(size - i - 1);
				exterior.at(iexterior[i] - 1) = invertPath;
			}
		}
		
		size_t outSize = interior.size();
		std::vector<ClipperLib::PolyTree> inTrees;
		if (outSize > 0)
		{
			inTrees.resize(outSize);
			for (size_t i = 0; i < outSize; ++i)
			{
				for (size_t j = 0; j < iexterior.size(); j++)
				{
					if (i == iexterior[j] - 1)
					{
						offset = -offset;
						break;
					}
				}
				offsetPath(interior.at(i), offset, inTrees.at(i));
			}
		}

		ClipperLib::Clipper clipper;
		polyNodeFunc func = [&func, &clipper](ClipperLib::PolyNode* node) {

			size_t size = node->Contour.size();
			if (size > 0)
			{
				ClipperLib::Path invertPath(size);
				for (size_t i = 0; i < size; ++i)
					invertPath.at(i) = node->Contour.at(size - i - 1);
				clipper.AddPath(invertPath, ClipperLib::ptClip, true);
			}
		};

		for (size_t i = 0; i < outSize; ++i)
		{
			mmesh::loopPolyTree(func, &inTrees.at(i));
		}

		for (ClipperLib::Path* path : exterior)
			clipper.AddPath(*path, ClipperLib::ptClip, true);
		clipper.Execute(ClipperLib::ctUnion, source, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
	}

	void skeletonPolyTree(ClipperLib::PolyTree& source, double z, std::vector<Patch*>& patches)
	{
		ClipperLib::PolyTree roofLine;
		ClipperLib::PolyTree roofPoint;
		ClipperLib::Paths* paths = new ClipperLib::Paths;
		fmesh::roofLine(&source, &roofLine, &roofPoint, nullptr);

		for (size_t i=0;i< paths->size();i++)
		{
			ClipperLib::PolyNode pn;
			pn.Contour = paths->at(i);
			for (size_t i=0;i< pn.Contour.size();i++)
			{				
				if (pn.Contour.at(i).Z)
				{
					pn.Contour.at(i).Z = z*1000 + 5000;
				}
				else
				{
					pn.Contour.at(i).Z = z * 1000;
				}
			}
			Patch* tpath = fillOneLevelPolyNode(&pn);
			patches.push_back(tpath);
		}
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

	void seperate1423(ClipperLib::PolyTree* polyTree, std::vector<PolyPair*>& polyPairs)
	{
		for (ClipperLib::PolyNode* node1 : polyTree->Childs)
		{
			std::vector<ClipperLib::PolyNode*>& node2 = node1->Childs;
			std::vector<ClipperLib::PolyNode*> node3;
			for (ClipperLib::PolyNode* n : node2)
				node3.insert(node3.end(), n->Childs.begin(), n->Childs.end());
			std::vector<ClipperLib::PolyNode*> node4;
			for (ClipperLib::PolyNode* n : node3)
				node4.insert(node4.end(), n->Childs.begin(), n->Childs.end());

			PolyPair* pair1 = new PolyPair();
			pair1->clockwise = false;
			pair1->outer = node1;
			pair1->inner.swap(node4);
			polyPairs.push_back(pair1);

			for (ClipperLib::PolyNode* n : node2)
			{
				PolyPair* pair = new PolyPair();
				pair->clockwise = true;
				pair->outer = n;
				pair->inner = n->Childs;
				polyPairs.push_back(pair);
			}
		}
	}
}