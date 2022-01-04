#include "polytree.h"
#include "fmesh/clipper/circurlar.h"

#include "fmesh/generate/polyfiller.h"
#include "fmesh/generate/triangularization.h"
#include "cmesh/mesh/roof.h"
#include <algorithm>
#include "skeleton2polytree.h"

namespace fmesh
{
	int testPolyNodeDepth(ClipperLibXYZ::PolyNode* node)
	{
		if (!node || !node->Parent)
			return 0;

		int depth = 0;
		ClipperLibXYZ::PolyNode* parent = node->Parent;
		while (parent)
		{
			++depth;
			parent = parent->Parent;
		}
		return depth;
	}

	void seperatePolyTree1234(ClipperLibXYZ::PolyTree* tree, std::vector<std::vector<ClipperLibXYZ::PolyNode*>>& depthNodes)
	{
		depthNodes.resize(8);
		auto f = [&depthNodes](ClipperLibXYZ::PolyNode* node) {
			int depth = testPolyNodeDepth(node);
			if (depth >= 1 && depth <= 8)
				depthNodes.at(depth - 1).push_back(node);
		};

		fmesh::loopPolyTree(f, tree);
	}

	void convertPaths2PolyTree(ClipperLibXYZ::Paths* paths, ClipperLibXYZ::PolyTree& polyTree)
	{
		ClipperLibXYZ::Clipper clipper;
		clipper.AddPaths(*paths, ClipperLibXYZ::ptSubject, true);
		clipper.Execute(ClipperLibXYZ::ctUnion, polyTree, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);

		polyNodeFunc func = [&func, &clipper](ClipperLibXYZ::PolyNode* node) {
			ClipperLibXYZ::CleanPolygon(node->Contour);//remove Repeat point

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};
		func(&polyTree);
	}

	void copy2PolyTree(ClipperLibXYZ::PolyTree& source, ClipperLibXYZ::PolyTree& dest)
	{
		ClipperLibXYZ::Clipper clipper;
		polyNodeFunc func = [&func, &clipper](ClipperLibXYZ::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptSubject, true);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);
		clipper.Execute(ClipperLibXYZ::ctUnion, dest, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
	}

	void extendPolyTree(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLibXYZ::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedLine);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);
		offset.Execute(dest, microDelta);
	}

	void extendPolyTreeOpen(ClipperLibXYZ::Paths& paths, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::ClipperOffset offset;
		for (ClipperLibXYZ::Path path : paths)
		{
			offset.AddPath(path, ClipperLibXYZ::jtSquare, ClipperLibXYZ::EndType::etOpenSquare);
		}

		offset.Execute(dest, microDelta);
	}

	
	void extendPolyTreeNew(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		//get origin poly
		ClipperLibXYZ::PolyTree source1;
		ClipperLibXYZ::PolyTree source2;
		ClipperLibXYZ::ClipperOffset offset;
		bool bReverse = false;
		polyNodeFunc func = [&func, &offset, &bReverse](ClipperLibXYZ::PolyNode* node) {
			if (bReverse)
				ClipperLibXYZ::ReversePath(node->Contour);
			offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
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

	void extendPolyTreeMiter(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLibXYZ::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLibXYZ::jtMiter, ClipperLibXYZ::EndType::etClosedLine);
			offset.MiterLimit = 10.0f;
			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);
		offset.Execute(dest, microDelta);
	}

	void offsetPolyTree(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLibXYZ::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);

		offset.Execute(dest, microDelta);
	}

	void offsetPolyType(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest, bool isBluntSharp)
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

	void passivationPolyTree(ClipperLibXYZ::PolyTree& source, double delta)
	{
		double microDelta = 1000.0 * delta;

		//get origin poly
		ClipperLibXYZ::PolyTree source2;
		ClipperLibXYZ::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLibXYZ::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};
		func(&source);
		offset.Execute(source2, microDelta);
		offset.Clear();
		func(&source2);
		offset.Execute(source, -microDelta);
	}

	void offsetPolyTreeMiter(ClipperLibXYZ::PolyTree& source, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::ClipperOffset offset;
		polyNodeFunc func = [&func, &offset](ClipperLibXYZ::PolyNode* node) {
			offset.AddPath(node->Contour, ClipperLibXYZ::jtMiter, ClipperLibXYZ::EndType::etClosedPolygon);
			offset.MiterLimit = 10.0f;
			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);
		offset.Execute(dest, microDelta);
	}

	void offsetPaths(std::vector<ClipperLibXYZ::Path*>& source, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::ClipperOffset offset;
		for(ClipperLibXYZ::Path* path : source)
			offset.AddPath(*path, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);

		offset.Execute(dest, microDelta);
	}

	void offsetPolyNodes(const std::vector<ClipperLibXYZ::PolyNode*>& polyNodes, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;

		ClipperLibXYZ::cInt z = 0;
		if (polyNodes.size() > 0 && polyNodes.at(0)->Contour.size() > 0)
			z = polyNodes.at(0)->Contour.at(0).Z;

		ClipperLibXYZ::ClipperOffset offset;
		for (ClipperLibXYZ::PolyNode* node : polyNodes)
			offset.AddPath(node->Contour, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);

		offset.Execute(dest, microDelta);

		setPolyTreeZ(dest, z);
	}

	void offsetPath(ClipperLibXYZ::Path* source, double delta, ClipperLibXYZ::PolyTree& dest)
	{
		double microDelta = 1000.0 * delta;
		ClipperLibXYZ::cInt z = 0;
		if (source->size() > 0)
			z = source->at(0).Z;

		ClipperLibXYZ::ClipperOffset offset;
		offset.AddPath(*source, ClipperLibXYZ::jtRound, ClipperLibXYZ::EndType::etClosedPolygon);

		offset.Execute(dest, microDelta);

		auto f = [&z](ClipperLibXYZ::PolyNode* node){
			for (ClipperLibXYZ::IntPoint& p : node->Contour)
				p.Z = z;
		};

		fmesh::loopPolyTree(f, &dest);
	}

	void extendPolyTree(ClipperLibXYZ::PolyTree& source, double delta, polyOffsetFunc offsetFunc, ClipperLibXYZ::PolyTree& dest)
	{
		extendPolyTree(source, delta, dest);
		polyTreeOffset(dest, offsetFunc);
	}

	void offsetAndExtendPolyTree(ClipperLibXYZ::PolyTree& source, double offset, double delta, double z, ClipperLibXYZ::PolyTree& dest)
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
		polyNodeFunc func = [&func, &z](ClipperLibXYZ::PolyNode* node) {
			for (ClipperLibXYZ::IntPoint& point : node->Contour)
				point.Z = (int)(1000.0 * z);
		};

		fmesh::loopPolyTree(func, &dest);
	}

	void offsetAndExtendPolyTreeMiter(ClipperLibXYZ::PolyTree& source, double offset, double delta, double z, ClipperLibXYZ::PolyTree& dest)
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
		polyNodeFunc func = [&func, &z](ClipperLibXYZ::PolyNode* node) {
			for (ClipperLibXYZ::IntPoint& point : node->Contour)
				point.Z = (int)(1000.0 * z);
		};

		fmesh::loopPolyTree(func, &dest);
	}

	void offsetAndExtendPolyTreeNew(ClipperLibXYZ::PolyTree& source, double offset, double delta, double z, ClipperLibXYZ::PolyTree& dest)
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
		polyNodeFunc func = [&func, &z](ClipperLibXYZ::PolyNode* node) {
			for (ClipperLibXYZ::IntPoint& point : node->Contour)
				point.Z += (int)(1000.0 * z);
		};

		fmesh::loopPolyTree(func, &dest);
	}

	void offsetAndExtendPolyTree(ClipperLibXYZ::PolyTree& source, double offset, double delta, ClipperLibXYZ::PolyTree& dest)
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

	void offsetAndExtendPolyTreeMiter(ClipperLibXYZ::PolyTree& source, double offset, double delta, ClipperLibXYZ::PolyTree& dest)
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

	void offsetAndExtendpolyType(ClipperLibXYZ::PolyTree& source, double offset, double delta, double z, ClipperLibXYZ::PolyTree& dest, bool isBluntSharp)
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

	void setPolyTreeZ(ClipperLibXYZ::PolyTree& tree, double z)
	{
		ClipperLibXYZ::cInt cZ = (int)(1000.0 * z);
		setPolyTreeZ(tree, cZ);
	}

	void setPolyTreeZ(ClipperLibXYZ::PolyTree& tree, ClipperLibXYZ::cInt z)
	{
		polyNodeFunc func = [&func, &z](ClipperLibXYZ::PolyNode* node) {
			for (ClipperLibXYZ::IntPoint& point : node->Contour)
				point.Z = z;
		};

		fmesh::loopPolyTree(func, &tree);
	}

	void adjustPolyTreeZ(ClipperLibXYZ::PolyTree& tree)
	{
		polyNodeFunc func = [&func](ClipperLibXYZ::PolyNode* node) {
			ClipperLibXYZ::cInt z = 0;
			for (ClipperLibXYZ::IntPoint& point : node->Contour)
			{
				if (point.Z != 0)
				{
					z = point.Z;
					break;
				}
			}

			if (z != 0)
			{
				for (ClipperLibXYZ::IntPoint& point : node->Contour)
					if(!point.Z)
						point.Z = z;
			}
		};

		fmesh::loopPolyTree(func, &tree);
	}

	void offsetExterior(ClipperLibXYZ::PolyTree& source, double offset,double z)
	{
		std::vector<ClipperLibXYZ::Path*> exterior;
		std::vector<ClipperLibXYZ::Path*> interior;
		std::vector<int> iexterior;
		std::vector<int> iinterior;

		fmesh::seperatePolyTree(&source, exterior, interior, iexterior, iinterior);

		if(iexterior.size()>0 &&
			iexterior.size() == iinterior.size())
		{
			for (size_t i = 0; i < iexterior.size(); ++i)
			{
				ClipperLibXYZ::Path* tmp = exterior.at(iexterior[i]-1);
				exterior.at(iexterior[i] - 1) = interior.at(iinterior[i]-1);
				
				size_t size = tmp->size();
				ClipperLibXYZ::Path* invertPath=new ClipperLibXYZ::Path(size);
				for (size_t i = 0; i < size; ++i)
					invertPath->at(i) = tmp->at(size - i - 1);
				interior.at(iinterior[i] - 1) = invertPath;
			}
		}
	
		size_t outSize = exterior.size();
		std::vector<ClipperLibXYZ::PolyTree> outTrees;
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

		ClipperLibXYZ::Clipper clipper;
		polyNodeFunc func = [&func, &clipper](ClipperLibXYZ::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
		};
		
		for (size_t i = 0; i < outSize; ++i)
		{
			fmesh::loopPolyTree(func, &outTrees.at(i));
		}

		for(ClipperLibXYZ::Path* path : interior)
			clipper.AddPath(*path, ClipperLibXYZ::ptClip, true);
		clipper.Execute(ClipperLibXYZ::ctUnion, source, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
	
		setPolyTreeZ(source, z);
	}

	void offsetExteriorInner(ClipperLibXYZ::PolyTree& source, double offset, double z)
	{
		std::vector<std::vector<ClipperLibXYZ::PolyNode*>> depthNodes;
		seperatePolyTree1234(&source, depthNodes);

		ClipperLibXYZ::PolyTree extend1;
		ClipperLibXYZ::PolyTree extend2;
		ClipperLibXYZ::PolyTree extend5;
		ClipperLibXYZ::PolyTree extend6;

		offsetPolyNodes(depthNodes.at(1), -offset, extend1);
		offsetPolyNodes(depthNodes.at(2), offset, extend2);
		offsetPolyNodes(depthNodes.at(5), -offset, extend5);
		offsetPolyNodes(depthNodes.at(6), offset, extend6);

		ClipperLibXYZ::Clipper clipper;
		polyNodeFunc func = [&func, &clipper](ClipperLibXYZ::PolyNode* node) {
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
		};

		polyNodeFunc rFunc = [&func, &clipper](ClipperLibXYZ::PolyNode* node) {
			ClipperLibXYZ::ReversePath(node->Contour);
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
		};
		
		fmesh::loopPolyTree(func, &extend2);
		fmesh::loopPolyTree(rFunc, &extend1);
		fmesh::loopPolyTree(func, &extend6);
		fmesh::loopPolyTree(rFunc, &extend5);
		for (ClipperLibXYZ::PolyNode* node : depthNodes.at(0))
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
		for (ClipperLibXYZ::PolyNode* node : depthNodes.at(3))
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
		for (ClipperLibXYZ::PolyNode* node : depthNodes.at(4))
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
		for (ClipperLibXYZ::PolyNode* node : depthNodes.at(7))
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);

		source.Clear();
		clipper.Execute(ClipperLibXYZ::ctUnion, source, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);

		setPolyTreeZ(source, z);
	}

	void skeletonPolyTree(ClipperLibXYZ::PolyTree& source, double z, std::vector<Patch*>& patches, double height, bool onePoly)
	{
		ClipperLibXYZ::PolyTree roofLine;
		ClipperLibXYZ::PolyTree roofPoint;
		ClipperLibXYZ::Paths* paths = new ClipperLibXYZ::Paths;
		cmesh::roofLine(&source, &roofLine, &roofPoint, paths, onePoly);

		for (size_t i=0;i< paths->size();i++)
		{
			ClipperLibXYZ::PolyNode pn;
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

	double skeletonPoly(ClipperLibXYZ::PolyTree& source, ClipperLibXYZ::PolyTree& poly, double thickness)
	{
		ClipperLibXYZ::PolyTree roofLine;
		ClipperLibXYZ::PolyTree roofPoint;
		ClipperLibXYZ::Paths* paths = new ClipperLibXYZ::Paths;
		cmesh::roofLine(&source, &roofLine, &roofPoint, paths, true);

		ClipperLibXYZ::Paths* sPaths = new ClipperLibXYZ::Paths;
		dealSkeleton(roofLine, sPaths);


		double len = averageLen(source, *sPaths);

		fmesh::extendPolyTreeOpen(*sPaths, (thickness * 2 + len / 1000.0), poly);
		return len / 1000.0;
	}

	void skeletonPolyTreeSharp(ClipperLibXYZ::PolyTree& source, double z, double height, std::vector<Patch*>& patches,bool onePoly)
	{
		ClipperLibXYZ::PolyTree roofLine;
		ClipperLibXYZ::PolyTree roofPoint;
		ClipperLibXYZ::Paths* paths = new ClipperLibXYZ::Paths;
		cmesh::roofLine(&source, &roofLine, &roofPoint, paths, onePoly);

		for (size_t i = 0; i < paths->size(); i++)
		{
			ClipperLibXYZ::PolyNode pn;
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

	void polyTreeOffset(ClipperLibXYZ::PolyTree& source, polyOffsetFunc offsetFunc)
	{
		polyNodeFunc func = [&func, &offsetFunc](ClipperLibXYZ::PolyNode* node) {
			for (ClipperLibXYZ::IntPoint& point : node->Contour)
				point = offsetFunc(point);
		};

		fmesh::loopPolyTree(func, &source);
	}

	void xor2PolyTrees(ClipperLibXYZ::PolyTree* outer, ClipperLibXYZ::PolyTree* inner, ClipperLibXYZ::PolyTree& out)
	{
		if (!outer || !inner)
			return;

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

		clipper.Execute(ClipperLibXYZ::ctXor, out, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
		adjustPolyTreeZ(out);
	}

	void xor2PolyTrees(ClipperLibXYZ::PolyTree* outer, ClipperLibXYZ::PolyTree* inner, ClipperLibXYZ::PolyTree& out, int flag)
	{
		if (!outer || !inner)
			return;

		ClipperLibXYZ::Clipper clipper;

		polyNodeFunc func1 = [&func1, &clipper, &flag](ClipperLibXYZ::PolyNode* node) {
			if (checkFlag(node, flag))
				clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func1(n);
		};

		func1(outer);

		polyNodeFunc func2 = [&func2, &clipper, &flag](ClipperLibXYZ::PolyNode* node) {
			if (checkFlag(node, flag))
				clipper.AddPath(node->Contour, ClipperLibXYZ::ptSubject, true);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func2(n);
		};

		func2(inner);

		clipper.Execute(ClipperLibXYZ::ctXor, out, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
		adjustPolyTreeZ(out);
	}

	void xor2PolyNodes(const std::vector<ClipperLibXYZ::PolyNode*>& outer,
		const std::vector<ClipperLibXYZ::PolyNode*>& inner, ClipperLibXYZ::PolyTree& out)
	{
		ClipperLibXYZ::Clipper clipper;

		for (ClipperLibXYZ::PolyNode* node : outer)
		{
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
		}

		for (ClipperLibXYZ::PolyNode* node : inner)
		{
			clipper.AddPath(node->Contour, ClipperLibXYZ::ptSubject, true);
		}

		clipper.Execute(ClipperLibXYZ::ctXor, out, ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
		adjustPolyTreeZ(out);
	}

	int GetPolyCount(ClipperLibXYZ::PolyTree* poly, int flag)//2: Inner  3: Outer
	{
		int num = 1;
		int index = 1;

		polyNodeFunc func = [&func, &num, &index, &flag](ClipperLibXYZ::PolyNode* node) {
			num += num * index;

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
			{
				if (n->Contour.size() > 0)
					index++;
				func(n);
			}
		};

		func(poly);
		return num;
	}

	double GetPolyArea(ClipperLibXYZ::PolyTree* poly, int flag)//2: Inner  3: Outer
	{
		double area = 0.0f;
		polyNodeFunc func = [&func, &area, &flag](ClipperLibXYZ::PolyNode* node) {

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
			{
				if (n->Contour.size() > 0)
					area += ClipperLibXYZ::Area(n->Contour);
				func(n);
			}
		};

		func(poly);
		return area;
	}

	void split(ClipperLibXYZ::PolyTree& source, std::vector<ClipperLibXYZ::Paths>& dests)
	{
		std::vector<ClipperLibXYZ::PolyNode*> childrens;
		polyNodeFunc func = [&func, &childrens](ClipperLibXYZ::PolyNode* node) {
			if (testPolyNodeDepth(node) % 2 == 1)
				childrens.push_back(node);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
				func(n);
		};

		func(&source);

		size_t size = childrens.size();
		if (size > 0)
		{
			dests.resize(size);
			for (size_t i = 0; i < size; ++i)
			{
				ClipperLibXYZ::PolyNode* node = childrens.at(i);
				ClipperLibXYZ::Clipper clipper;
				clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
				for (ClipperLibXYZ::PolyNode* n : node->Childs)
					clipper.AddPath(n->Contour, ClipperLibXYZ::ptClip, true);
				clipper.Execute(ClipperLibXYZ::ctUnion, dests.at(i), ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
			}
		}
	}

	void split_omp(ClipperLibXYZ::PolyTree& source, std::vector<ClipperLibXYZ::Paths>& dests)
	{
		std::vector<ClipperLibXYZ::PolyNode*> childrens;
		polyNodeFunc func = [&func, &childrens](ClipperLibXYZ::PolyNode* node) {
			if (testPolyNodeDepth(node) % 2 == 1)
				childrens.push_back(node);

			for (ClipperLibXYZ::PolyNode* n : node->Childs)
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
				ClipperLibXYZ::PolyNode* node = childrens.at(i);
				ClipperLibXYZ::Clipper clipper;
				clipper.AddPath(node->Contour, ClipperLibXYZ::ptClip, true);
				for (ClipperLibXYZ::PolyNode* n : node->Childs)
					clipper.AddPath(n->Contour, ClipperLibXYZ::ptClip, true);
				clipper.Execute(ClipperLibXYZ::ctUnion, dests.at(i), ClipperLibXYZ::pftNonZero, ClipperLibXYZ::pftNonZero);
			}
		}
	}
}