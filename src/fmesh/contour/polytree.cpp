#include "polytree.h"
#include "mmesh/clipper/circurlar.h"

#include "fmesh/generate/polyfiller.h"
#include "fmesh/generate/triangularization.h"
#include "mmesh/cgal/roof.h"
#include <algorithm>
#include "skeleton2polytree.h"

namespace fmesh
{
	int testPolyNodeDepth(ClipperLib::PolyNode* node)
	{
		if (!node || !node->Parent)
			return 0;

		int depth = 0;
		ClipperLib::PolyNode* parent = node->Parent;
		while (parent)
		{
			++depth;
			parent = parent->Parent;
		}
		return depth;
	}

	void seperatePolyTree1234(ClipperLib::PolyTree* tree, std::vector<std::vector<ClipperLib::PolyNode*>>& depthNodes)
	{
		depthNodes.resize(8);
		auto f = [&depthNodes](ClipperLib::PolyNode* node) {
			int depth = testPolyNodeDepth(node);
			if (depth >= 1 && depth <= 8)
				depthNodes.at(depth - 1).push_back(node);
		};

		mmesh::loopPolyTree(f, tree);
	}

	void convertPaths2PolyTree(ClipperLib::Paths* paths, ClipperLib::PolyTree& polyTree)
	{
		ClipperLib::Clipper clipper;
		clipper.AddPaths(*paths, ClipperLib::ptSubject, true);
		clipper.Execute(ClipperLib::ctUnion, polyTree, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

		polyNodeFunc func = [&func, &clipper](ClipperLib::PolyNode* node) {
			ClipperLib::CleanPolygon(node->Contour);//remove Repeat point

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};
		func(&polyTree);
	}

	void copy2PolyTree(ClipperLib::PolyTree& source, ClipperLib::PolyTree& dest)
	{
		ClipperLib::Clipper clipper;
		polyNodeFunc func = [&func, &clipper](ClipperLib::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLib::ptSubject, true);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);
		clipper.Execute(ClipperLib::ctUnion, dest, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
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

	void extendPolyTreeOpen(ClipperLib::Paths& paths, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLib::ClipperOffset offset;
		for (ClipperLib::Path path : paths)
		{
			offset.AddPath(path, ClipperLib::jtSquare, ClipperLib::EndType::etOpenSquare);
		}

		offset.Execute(dest, microDelta);
	}

	
	void extendPolyTreeNew(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		//get origin poly
		ClipperLib::PolyTree source1;
		ClipperLib::PolyTree source2;
		ClipperLib::ClipperOffset offset;
		bool bReverse = false;
		polyNodeFunc func = [&func, &offset, &bReverse](ClipperLib::PolyNode* node) {
			if (bReverse)
				ClipperLib::ReversePath(node->Contour);
			offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};
		func(&source);
		offset.Execute(source1, -microDelta);

		offset.Execute(source2, -microDelta);
		offset.Clear();
		func(&source2);
		offset.Execute(source2, +microDelta);

		offset.Clear();
		func(&source1);
		bReverse = true;
		func(&source2);
		offset.Execute(dest, 0);
	}

	void extendPolyTreeMiter(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLib::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLib::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLib::jtMiter, ClipperLib::EndType::etClosedLine);
			offset.MiterLimit = 10.0f;
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

	void offsetPolyType(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest, bool isBluntSharp)
	{	
		if (isBluntSharp)
		{		
			if (delta < 0)//Zoom in and out
			{
				offsetPolyTree(source, delta*2, dest);
				offsetPolyTree(dest, - delta , dest);
			}
			else
				offsetPolyTree(source, delta, dest);
		}
		else 
		{
			if (delta < 0)//Zoom in and out
			{
				offsetPolyTreeMiter(source, delta*2, dest);
				offsetPolyTreeMiter(dest, -delta, dest);
			}
			else
				offsetPolyTreeMiter(source, delta, dest);
		}
	}

	void passivationPolyTree(ClipperLib::PolyTree& source, double delta)
	{
		double microDelta = 1000.0 * delta;

		//get origin poly
		ClipperLib::PolyTree source2;
		ClipperLib::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLib::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};
		func(&source);
		offset.Execute(source2, microDelta);
		offset.Clear();
		func(&source2);
		offset.Execute(source, -microDelta);
	}

	void offsetPolyTreeMiter(ClipperLib::PolyTree& source, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLib::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLib::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLib::jtMiter, ClipperLib::EndType::etClosedPolygon);
			offset.MiterLimit = 10.0f;
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

	void offsetPolyNodes(const std::vector<ClipperLib::PolyNode*>& polyNodes, double delta, ClipperLib::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLib::cInt z = 0;
		if (polyNodes.size() > 0 && polyNodes.at(0)->Contour.size() > 0)
			z = polyNodes.at(0)->Contour.at(0).Z;

		ClipperLib::ClipperOffset offset;
		for (ClipperLib::PolyNode* node : polyNodes)
			offset.AddPath(node->Contour, ClipperLib::jtRound, ClipperLib::EndType::etClosedPolygon);

		offset.Execute(dest, microDelta);

		setPolyTreeZ(dest, z);
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
			offsetPolyTree(source, -delta, dest);
			extendPolyTree(dest, delta, dest);
		}
		else
		{
			offsetPolyTree(source, offset - delta, dest);
			extendPolyTree(dest, delta, dest);
		}
		polyNodeFunc func = [&func, &z](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& point : node->Contour)
				point.Z = (int)(1000.0 * z);
		};

		mmesh::loopPolyTree(func, &dest);
	}

	void offsetAndExtendPolyTreeMiter(ClipperLib::PolyTree& source, double offset, double delta, double z, ClipperLib::PolyTree& dest)
	{
		if (offset == 0.0)
		{
			offsetPolyTreeMiter(source, -delta, dest);
			extendPolyTreeMiter(dest, delta, dest);
		}
		else
		{
			offsetPolyTreeMiter(source, offset - delta, dest);
			extendPolyTreeMiter(dest, delta, dest);
		}
		polyNodeFunc func = [&func, &z](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& point : node->Contour)
				point.Z = (int)(1000.0 * z);
		};

		mmesh::loopPolyTree(func, &dest);
	}

	void offsetAndExtendPolyTreeNew(ClipperLib::PolyTree& source, double offset, double delta, double z, ClipperLib::PolyTree& dest)
	{
		if (offset == 0.0)
		{
			extendPolyTreeNew(source, delta, dest);
		}
		else
		{
			offsetPolyTree(source, offset, dest);
			extendPolyTreeNew(dest, delta, dest);
		}
		polyNodeFunc func = [&func, &z](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& point : node->Contour)
				point.Z += (int)(1000.0 * z);
		};

		mmesh::loopPolyTree(func, &dest);
	}

	void offsetAndExtendPolyTree(ClipperLib::PolyTree& source, double offset, double delta, ClipperLib::PolyTree& dest)
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
	}

	void offsetAndExtendPolyTreeMiter(ClipperLib::PolyTree& source, double offset, double delta, ClipperLib::PolyTree& dest)
	{
		if (offset == 0.0)
		{
			extendPolyTreeMiter(source, delta, dest);
		}
		else
		{
			offsetPolyTreeMiter(source, offset, dest);
			extendPolyTreeMiter(dest, delta, dest);
		}
	}

	void offsetAndExtendpolyType(ClipperLib::PolyTree& source, double offset, double delta, double z, ClipperLib::PolyTree& dest, bool isBluntSharp)
	{
		if (isBluntSharp)
		{
			offsetAndExtendPolyTree(source, offset, delta, z, dest);
		}
		else
		{
			offsetAndExtendPolyTreeMiter(source, offset, delta, z, dest);
		}
	}

	void setPolyTreeZ(ClipperLib::PolyTree& tree, double z)
	{
		ClipperLib::cInt cZ = (int)(1000.0 * z);
		setPolyTreeZ(tree, cZ);
	}

	void setPolyTreeZ(ClipperLib::PolyTree& tree, ClipperLib::cInt z)
	{
		polyNodeFunc func = [&func, &z](ClipperLib::PolyNode* node) {
			for (ClipperLib::IntPoint& point : node->Contour)
				point.Z = z;
		};

		mmesh::loopPolyTree(func, &tree);
	}

	void adjustPolyTreeZ(ClipperLib::PolyTree& tree)
	{
		polyNodeFunc func = [&func](ClipperLib::PolyNode* node) {
			ClipperLib::cInt z = 0;
			for (ClipperLib::IntPoint& point : node->Contour)
			{
				if (point.Z != 0)
				{
					z = point.Z;
					break;
				}
			}

			if (z != 0)
			{
				for (ClipperLib::IntPoint& point : node->Contour)
					if(!point.Z)
						point.Z = z;
			}
		};

		mmesh::loopPolyTree(func, &tree);
	}

	void offsetExterior(ClipperLib::PolyTree& source, double offset,double z)
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
				if(offset<0)
					offset = -offset;
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
	
		setPolyTreeZ(source, z);
	}

	void offsetExteriorInner(ClipperLib::PolyTree& source, double offset, double z)
	{
		std::vector<std::vector<ClipperLib::PolyNode*>> depthNodes;
		seperatePolyTree1234(&source, depthNodes);

		ClipperLib::PolyTree extend1;
		ClipperLib::PolyTree extend2;
		ClipperLib::PolyTree extend5;
		ClipperLib::PolyTree extend6;

		offsetPolyNodes(depthNodes.at(1), -offset, extend1);
		offsetPolyNodes(depthNodes.at(2), offset, extend2);
		offsetPolyNodes(depthNodes.at(5), -offset, extend5);
		offsetPolyNodes(depthNodes.at(6), offset, extend6);

		ClipperLib::Clipper clipper;
		polyNodeFunc func = [&func, &clipper](ClipperLib::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
		};

		polyNodeFunc rFunc = [&func, &clipper](ClipperLib::PolyNode* node) {
			ClipperLib::ReversePath(node->Contour);
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
		};
		
		mmesh::loopPolyTree(func, &extend2);
		mmesh::loopPolyTree(rFunc, &extend1);
		mmesh::loopPolyTree(func, &extend6);
		mmesh::loopPolyTree(rFunc, &extend5);
		for (ClipperLib::PolyNode* node : depthNodes.at(0))
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
		for (ClipperLib::PolyNode* node : depthNodes.at(3))
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
		for (ClipperLib::PolyNode* node : depthNodes.at(4))
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
		for (ClipperLib::PolyNode* node : depthNodes.at(7))
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);

		source.Clear();
		clipper.Execute(ClipperLib::ctUnion, source, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

		setPolyTreeZ(source, z);
	}

	void skeletonPolyTree(ClipperLib::PolyTree& source, double z, std::vector<Patch*>& patches, double height, bool onePoly)
	{
		ClipperLib::PolyTree roofLine;
		ClipperLib::PolyTree roofPoint;
		ClipperLib::Paths* paths = new ClipperLib::Paths;
		mmesh::roofLine(&source, &roofLine, &roofPoint, paths, onePoly);

		for (size_t i=0;i< paths->size();i++)
		{
			ClipperLib::PolyNode pn;
			pn.Contour = paths->at(i);
			bool clockwise = false;
			for (size_t i=0;i< pn.Contour.size();i++)
			{				
				if (pn.Contour.at(i).Z == 300)
				{
					pn.Contour.at(i).Z = 0;
					clockwise = true;
				}
				else if (pn.Contour.at(i).Z == 500)
				{
					pn.Contour.at(i).Z+= height*1000;
				}

				pn.Contour.at(i).Z += z * 1000;
			}
			Patch* tpath = fillOneLevelPolyNode(&pn);

			if (clockwise)
			{
				reverse(tpath->begin(), tpath->end());
			}

			patches.push_back(tpath);
		}
	}

	double skeletonPoly(ClipperLib::PolyTree& source, ClipperLib::PolyTree& poly, double thickness)
	{
		ClipperLib::PolyTree roofLine;
		ClipperLib::PolyTree roofPoint;
		ClipperLib::Paths* paths = new ClipperLib::Paths;
		mmesh::roofLine(&source, &roofLine, &roofPoint, paths, true);

		ClipperLib::Paths* sPaths = new ClipperLib::Paths;
		dealSkeleton(roofLine, sPaths);


		double len = averageLen(source, *sPaths);

		fmesh::extendPolyTreeOpen(*sPaths, (thickness * 2 + len / 1000.0), poly);
		return len / 1000.0;
	}

	void skeletonPolyTreeSharp(ClipperLib::PolyTree& source, double z, double height, std::vector<Patch*>& patches,bool onePoly)
	{
		ClipperLib::PolyTree roofLine;
		ClipperLib::PolyTree roofPoint;
		ClipperLib::Paths* paths = new ClipperLib::Paths;
		mmesh::roofLine(&source, &roofLine, &roofPoint, paths, onePoly);

		for (size_t i = 0; i < paths->size(); i++)
		{
			ClipperLib::PolyNode pn;
			pn.Contour = paths->at(i);
			bool clockwise = false;
			for (size_t i = 0; i < pn.Contour.size(); i++)
			{
				if (pn.Contour.at(i).Z == 300)
				{
					clockwise = true;
				}
				else if (pn.Contour.at(i).Z == 500)
				{
					pn.Contour.at(i).Z = height * 1000;
				}

				pn.Contour.at(i).Z += z * 1000;
			}
			Patch* tpath = fillOneLevelPolyNode(&pn);

			if (clockwise)
			{
				reverse(tpath->begin(), tpath->end());
			}

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
		adjustPolyTreeZ(out);
	}

	void xor2PolyTrees(ClipperLib::PolyTree* outer, ClipperLib::PolyTree* inner, ClipperLib::PolyTree& out, int flag)
	{
		if (!outer || !inner)
			return;

		ClipperLib::Clipper clipper;

		polyNodeFunc func1 = [&func1, &clipper, &flag](ClipperLib::PolyNode* node) {
			if (checkFlag(node, flag))
				clipper.AddPath(node->Contour, ClipperLib::ptClip, true);

			for (ClipperLib::PolyNode* n : node->Childs)
				func1(n);
		};

		func1(outer);

		polyNodeFunc func2 = [&func2, &clipper, &flag](ClipperLib::PolyNode* node) {
			if (checkFlag(node, flag))
				clipper.AddPath(node->Contour, ClipperLib::ptSubject, true);

			for (ClipperLib::PolyNode* n : node->Childs)
				func2(n);
		};

		func2(inner);

		clipper.Execute(ClipperLib::ctXor, out, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
		adjustPolyTreeZ(out);
	}

	void xor2PolyNodes(const std::vector<ClipperLib::PolyNode*>& outer,
		const std::vector<ClipperLib::PolyNode*>& inner, ClipperLib::PolyTree& out)
	{
		ClipperLib::Clipper clipper;

		for (ClipperLib::PolyNode* node : outer)
		{
			clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
		}

		for (ClipperLib::PolyNode* node : inner)
		{
			clipper.AddPath(node->Contour, ClipperLib::ptSubject, true);
		}

		clipper.Execute(ClipperLib::ctXor, out, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
		adjustPolyTreeZ(out);
	}

	int GetPolyCount(ClipperLib::PolyTree* poly, int flag)//2: Inner  3: Outer
	{
		int num = 1;
		int index = 1;

		polyNodeFunc func = [&func, &num, &index, &flag](ClipperLib::PolyNode* node) {
			num += num * index;

			for (ClipperLib::PolyNode* n : node->Childs)
			{
				if (n->Contour.size() > 0)
					index++;
				func(n);
			}
		};

		func(poly);
		return num;
	}

	double GetPolyArea(ClipperLib::PolyTree* poly, int flag)//2: Inner  3: Outer
	{
		double area = 0.0f;
		polyNodeFunc func = [&func, &area, &flag](ClipperLib::PolyNode* node) {

			for (ClipperLib::PolyNode* n : node->Childs)
			{
				if (n->Contour.size() > 0)
					area += ClipperLib::Area(n->Contour);
				func(n);
			}
		};

		func(poly);
		return area;
	}

	void split(ClipperLib::PolyTree& source, std::vector<ClipperLib::Paths>& dests)
	{
		std::vector<ClipperLib::PolyNode*> childrens;
		polyNodeFunc func = [&func, &childrens](ClipperLib::PolyNode* node) {
			if (testPolyNodeDepth(node) % 2 == 1)
				childrens.push_back(node);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);

		size_t size = childrens.size();
		if (size > 0)
		{
			dests.resize(size);
			for (size_t i = 0; i < size; ++i)
			{
				ClipperLib::PolyNode* node = childrens.at(i);
				ClipperLib::Clipper clipper;
				clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
				for (ClipperLib::PolyNode* n : node->Childs)
					clipper.AddPath(n->Contour, ClipperLib::ptClip, true);
				clipper.Execute(ClipperLib::ctUnion, dests.at(i), ClipperLib::pftNonZero, ClipperLib::pftNonZero);
			}
		}
	}

	void split_omp(ClipperLib::PolyTree& source, std::vector<ClipperLib::Paths>& dests)
	{
		std::vector<ClipperLib::PolyNode*> childrens;
		polyNodeFunc func = [&func, &childrens](ClipperLib::PolyNode* node) {
			if (testPolyNodeDepth(node) % 2 == 1)
				childrens.push_back(node);

			for (ClipperLib::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);

		int size = (int)childrens.size();
		if (size > 0)
		{
			dests.resize(size);
#pragma omp parallel for
			for (int i = 0; i < size; ++i)
			{
				ClipperLib::PolyNode* node = childrens.at(i);
				ClipperLib::Clipper clipper;
				clipper.AddPath(node->Contour, ClipperLib::ptClip, true);
				for (ClipperLib::PolyNode* n : node->Childs)
					clipper.AddPath(n->Contour, ClipperLib::ptClip, true);
				clipper.Execute(ClipperLib::ctUnion, dests.at(i), ClipperLib::pftNonZero, ClipperLib::pftNonZero);
			}
		}
	}
}