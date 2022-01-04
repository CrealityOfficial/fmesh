#include "polyfiller.h"
#include "fmesh/generate/earpolygon.h"
#include "fmesh/generate/triangularization.h"
#include "fmesh/common/dvecutil.h"
#include "fmesh/clipper/circurlar.h"
#include "fmesh/contour/contour.h"
#include "fmesh/contour/polytree.h"
#include <algorithm>

namespace fmesh
{
	Patch* fillSimplePolyTree(SimplePoly* poly)
	{
		EarPolygon ear;
		return ear.earClippingFromRefPoly(poly);
	}

	Patch* fillSimplePolyTree(ClipperLibXYZ::Path* path)
	{
		EarPolygon ear;
		return ear.earClippingFromPath(path);
	}

	Patch* fillSimplePolyTree(ClipperLibXYZ::PolyNode* poly)
	{
		EarPolygon ear;
		return ear.earClippingFromRefPoly(poly);
	}

	void fillComplexPolyTree(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool muesEven)
	{
		if (!polyTree)
			return;

		polyNodeFunc func = [&muesEven , &patches](ClipperLibXYZ::PolyNode* node) {
			if (muesEven && node->ChildCount() == 0)
				return;

			if (!node->IsHole())
			{
				Patch* patch = fillOneLevelPolyNode(node);
				if (patch) 
					patches.push_back(patch);
			}
		};

		fmesh::loopPolyTree(func, polyTree);
	}

	void fillComplexPolyTreeReverseInner(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool invert)
	{
		if (!polyTree)
			return;

		//savePolyTree(polyTree, "xxxxx");
		std::vector<ClipperLibXYZ::PolyNode*> source;
		std::vector<ClipperLibXYZ::PolyNode*> tmp;

		for (ClipperLibXYZ::PolyNode* node : polyTree->Childs)
			if (!node->IsHole())
				source.push_back(node);

		int parentChilds = 0;
		while (source.size() > 0)
		{
			for (ClipperLibXYZ::PolyNode* node : source)
			{
				if (node->ChildCount() == 0)
					continue;
#ifdef _DEBUG
				//double area = ClipperLibXYZ::Area(node->Contour);
				//std::cout << area << std::endl;
#endif
				Patch* patch = fillOneLevelPolyNode(node);
				if (patch)
				{
					bool outer = parentChilds % 2 == 0;
					if (outer)
					{
						int gson = 0;
						for (ClipperLibXYZ::PolyNode* n : node->Childs)
							gson += (int)n->ChildCount();
						if(gson == 0)
							outer = false;
					}

					bool lReverse = false;
					if (node->Contour.size() > 0 && node->ChildCount() > 0
						&& node->Childs.at(0)->Contour.size() > 0)
					{		
						if (pathMaxZ(node->Contour)>=pathMaxZ(node))
							lReverse = true;

						if (invert && outer)
						{
							if (pathMaxZ(node->Contour) <= pathMaxZ(node->Childs.at(0)->Contour))
							{
								lReverse = true;
							}
						}
						else if(invert&& !outer)
						{
							if (pathMaxZ(node->Contour) > pathMaxZ(node->Childs.at(0)->Contour))
							{
								lReverse = false;
							}
						}
					}
					
					if ((outer && lReverse) || (!outer && !lReverse))
						std::reverse(patch->begin(), patch->end());
					else
						int i = 0;
					patches.push_back(patch);
				}

				for (ClipperLibXYZ::PolyNode* n : node->Childs)
				{
					for (ClipperLibXYZ::PolyNode* cn : n->Childs)
					{
						if (!cn->IsHole())
							tmp.push_back(cn);
					}
				}
			}

			source.swap(tmp);
			tmp.clear();
			++parentChilds;
		}
	}
	
	void fillComplexPolyTreeReverse(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool isOuter)
	{
		if (!polyTree)
			return;

		//savePolyTree(polyTree, "f:/mypoly");
		std::vector<ClipperLibXYZ::PolyNode*> source;
		std::vector<ClipperLibXYZ::PolyNode*> tmp;

		for (ClipperLibXYZ::PolyNode* node : polyTree->Childs)
			if (!node->IsHole())
				source.push_back(node);

		int parentChilds = 0;
		while (source.size() > 0)
		{
			for (ClipperLibXYZ::PolyNode* node : source)
			{
				if (node->ChildCount() == 0)
					continue;
#ifdef _DEBUG
				//double area = ClipperLibXYZ::Area(node->Contour);
				//std::cout << area << std::endl;
#endif
				Patch* patch = fillOneLevelPolyNode(node);
				if (patch)
				{
					bool lReverse = false;
					if (node->Contour.size() > 0 && node->ChildCount() > 0
						&& node->Childs.at(0)->Contour.size() > 0)
					{
						if (node->Contour.at(0).Z >= node->Childs.at(0)->Contour.at(0).Z)
							lReverse = true;
					}

					if ((isOuter && lReverse) || (!isOuter && !lReverse))
						std::reverse(patch->begin(), patch->end());
					else
						int i = 0;
					patches.push_back(patch);
				}

				for (ClipperLibXYZ::PolyNode* n : node->Childs)
				{
					for (ClipperLibXYZ::PolyNode* cn : n->Childs)
					{
						if (!cn->IsHole())
							tmp.push_back(cn);
					}
				}
			}

			source.swap(tmp);
			tmp.clear();
			++parentChilds;
		}
	}

	void fillComplexPolyTreeReverseInnerNew(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool invert)
	{
		if (!polyTree)
			return;

		//savePolyTree(polyTree, "f:/mypoly");
		std::vector<ClipperLibXYZ::PolyNode*> source;
		std::vector<ClipperLibXYZ::PolyNode*> tmp;

		for (ClipperLibXYZ::PolyNode* node : polyTree->Childs)
			if (!node->IsHole())
				source.push_back(node);

		int parentChilds = 0;
		while (source.size() > 0)
		{
			for (ClipperLibXYZ::PolyNode* node : source)
			{
				if (node->ChildCount() == 0)
					continue;
#ifdef _DEBUG
				//double area = ClipperLibXYZ::Area(node->Contour);
				//std::cout << area << std::endl;
#endif
				Patch* patch = fillOneLevelPolyNode(node);
				if (patch)
				{
					if (invert)
					{
						std::reverse(patch->begin(), patch->end());
					}
					patches.push_back(patch);
				}

				for (ClipperLibXYZ::PolyNode* n : node->Childs)
				{
					for (ClipperLibXYZ::PolyNode* cn : n->Childs)
					{
						if (!cn->IsHole())
							tmp.push_back(cn);
					}
				}
			}

			source.swap(tmp);
			tmp.clear();
			++parentChilds;
		}
	}

	void fillComplexPolyTree_onePloy(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches, bool invert /*= false*/)
	{
		if (!polyTree)
			return;

		//savePolyTree(polyTree, "xxxxx");
		std::vector<ClipperLibXYZ::PolyNode*> source;
		std::vector<ClipperLibXYZ::PolyNode*> tmp;

		for (ClipperLibXYZ::PolyNode* node : polyTree->Childs)
			if (!node->IsHole())
				source.push_back(node);

		int parentChilds = 0;
		while (source.size() > 0)
		{
			for (ClipperLibXYZ::PolyNode* node : source)
			{
				if (node->ChildCount() == 0)
					continue;
#ifdef _DEBUG
				//double area = ClipperLibXYZ::Area(node->Contour);
				//std::cout << area << std::endl;
#endif
				Patch* patch = fillOneLevelPolyNode(node);
				if (patch)
				{
					bool outer = parentChilds % 2 == 0;
					if (outer)
					{
 						int gson = 0;
 						for (ClipperLibXYZ::PolyNode* n : node->Childs)
 							gson += (int)n->ChildCount();
 						if (gson == 0)
 							outer = false;
					}

					bool lReverse = false;
					if (node->Contour.size() > 0 && node->ChildCount() > 0
						&& node->Childs.at(0)->Contour.size() > 0)
					{
						//if (pathMaxZ(node->Contour) >= pathMaxZ(node))
						if (node->Contour.at(0).Z >= node->Childs.at(0)->Contour.at(0).Z)
							lReverse = true;

						if (invert && outer)
						{
							if (pathMaxZ(node->Contour) <= pathMaxZ(node->Childs.at(0)->Contour))
							{
								lReverse = true;
							}
						}
						else if (invert && !outer)
						{
							if (pathMaxZ(node->Contour) > pathMaxZ(node->Childs.at(0)->Contour))
							{
								lReverse = false;
							}
						}
					}

					if ((outer && lReverse) || (!outer && !lReverse))
						std::reverse(patch->begin(), patch->end());
					else
						int i = 0;
					patches.push_back(patch);
				}

				for (ClipperLibXYZ::PolyNode* n : node->Childs)
				{
					for (ClipperLibXYZ::PolyNode* cn : n->Childs)
					{
						if (!cn->IsHole())
							tmp.push_back(cn);
					}
				}
			}

			source.swap(tmp);
			tmp.clear();
			++parentChilds;
		}
	}

	Patch* fillOneLevelPolyNode(ClipperLibXYZ::PolyNode* polyNode, bool invert)
	{
		//SimplePoly poly;
		//merge2SimplePoly(polyNode, &poly, invert);
		//std::string strfile= "f:/simple";
		//int i = polyNode->Contour.size();
		//char a[100];
		//sprintf(a,"%d",i);
		//strfile+= a;
		//saveSimplePoly(poly, strfile);

		//return fillSimplePolyTree(&poly);
		return fillSimplePolyTree(polyNode);
		
	}

	void fillFirstLevelPolyNode(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;

		for (ClipperLibXYZ::PolyNode* node : polyTree->Childs)
		{
			Patch* patch = fillSimplePolyTree(&node->Contour);
			if (patch) patches.push_back(patch);
		}
	}

	void fillPolyTreeDepth14(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;
		
		std::vector<ClipperLibXYZ::PolyNode*> nodes1;
		std::vector<ClipperLibXYZ::PolyNode*> nodes4;
		std::vector<ClipperLibXYZ::PolyNode*> nodes5;
		std::vector<ClipperLibXYZ::PolyNode*> nodes8;
		polyNodeFunc func = [&patches, &nodes1, &nodes4, &nodes5, &nodes8](ClipperLibXYZ::PolyNode* node) {
			int depth = testPolyNodeDepth(node);
			if (depth == 1)
				nodes1.push_back(node);
			if (depth == 4)
				nodes4.push_back(node);
			if (depth == 5)
				nodes5.push_back(node);
			if (depth == 8)
				nodes8.push_back(node);
		};

		fmesh::loopPolyTree(func, polyTree);

		ClipperLibXYZ::PolyTree out;
		xor2PolyNodes(nodes1, nodes4, out);

		fillComplexPolyTree(&out, patches,false);

		ClipperLibXYZ::PolyTree out_;
		xor2PolyNodes(nodes5, nodes8, out_);

		fillComplexPolyTree(&out_, patches, false);
	}

	void fillPolyTreeDepth23(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;

		std::vector<ClipperLibXYZ::PolyNode*> nodes2;
		std::vector<ClipperLibXYZ::PolyNode*> nodes3;
		std::vector<ClipperLibXYZ::PolyNode*> nodes6;
		std::vector<ClipperLibXYZ::PolyNode*> nodes7;
		polyNodeFunc func = [&patches, &nodes2, &nodes3, &nodes6, &nodes7](ClipperLibXYZ::PolyNode* node) {
			int depth = testPolyNodeDepth(node);
			if (depth == 2)
				nodes2.push_back(node);
			if (depth == 3)
				nodes3.push_back(node);
			if (depth == 6)
				nodes6.push_back(node);
			if (depth == 7)
				nodes7.push_back(node);
		};

		fmesh::loopPolyTree(func, polyTree);

		ClipperLibXYZ::PolyTree out;
		xor2PolyNodes(nodes2, nodes3, out);

		fillComplexPolyTree(&out, patches, false);

		ClipperLibXYZ::PolyTree out_;
		xor2PolyNodes(nodes6, nodes7, out_);

		fillComplexPolyTree(&out_, patches, false);
	}

	void fillPolyTreeDepthOnePoly(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;

		std::vector<ClipperLibXYZ::PolyNode*> nodes1;
		std::vector<ClipperLibXYZ::PolyNode*> nodes4;
		std::vector<ClipperLibXYZ::PolyNode*> nodes5;
		std::vector<ClipperLibXYZ::PolyNode*> nodes8;
		polyNodeFunc func = [&patches, &nodes1, &nodes4, &nodes5, &nodes8](ClipperLibXYZ::PolyNode* node) {
			int depth = testPolyNodeDepth(node);
				nodes1.push_back(node);
		};

		fmesh::loopPolyTree(func, polyTree);

		ClipperLibXYZ::PolyTree out;
		xor2PolyNodes(nodes1, nodes4, out);

		fillComplexPolyTree(&out, patches, false);

		ClipperLibXYZ::PolyTree out_;
		xor2PolyNodes(nodes5, nodes8, out_);

		fillComplexPolyTree(&out_, patches, false);
	}

	void fillPolyNodeInner(ClipperLibXYZ::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;

		std::vector<ClipperLibXYZ::PolyNode*> nodes2;
		polyNodeFunc func = [&patches, &nodes2](ClipperLibXYZ::PolyNode* node) {
			int depth = testPolyNodeDepth(node);
			if (depth == 2 || depth == 6)
				nodes2.push_back(node);
		};

		fmesh::loopPolyTree(func, polyTree);

		for (ClipperLibXYZ::PolyNode* node : nodes2)
		{
			Patch* patch = fillOneLevelPolyNode(node, true);
			if (patch)
				patches.push_back(patch);
		}
	}

	void fillPolyNodeOutline(ClipperLibXYZ::PolyTree* polyTree1, std::vector<Patch*>& patches)
	{
		if (!polyTree1 )
			return;

		SimplePoly* newploy =new SimplePoly();
		merge2SimplePoly(polyTree1->Childs.at(0), newploy, 0);
		Patch* patch = fillSimplePolyTree(newploy);
		if (patch) patches.push_back(patch);

	}

	void dealPolyTreeAxisZ(ClipperLibXYZ::PolyTree* polyTree, double slope,double min, double height)
	{
		if (!polyTree)
			return;
		polyNodeFunc func = [&slope,&height,&min](ClipperLibXYZ::PolyNode* node) {
			for (size_t i = 0; i < node->Contour.size(); i++)
			{
  				node->Contour.at(i).Z += slope * (node->Contour.at(i).Y- min*1000)+ height*1000;
			}
		};

		fmesh::loopPolyTree(func, polyTree);
	}

	ClipperLibXYZ::cInt pathMaxX(ClipperLibXYZ::Path& path)
	{
		ClipperLibXYZ::cInt x = -999999;
		for (ClipperLibXYZ::IntPoint& point : path)
		{
			if (point.X > x)
				x = point.X;
		}
		return x;
	}

	ClipperLibXYZ::cInt pathMaxZ(ClipperLibXYZ::Path& path)
	{
		ClipperLibXYZ::cInt z = -999999;
		for (ClipperLibXYZ::IntPoint& point : path)
		{
			if (point.Z > z)
				z = point.Z;
		}
		return z;
	}

	ClipperLibXYZ::cInt pathMaxZ(ClipperLibXYZ::PolyNode* node)
	{
		ClipperLibXYZ::cInt z = -999999;
		ClipperLibXYZ::cInt tmp = 0;
		for (ClipperLibXYZ::PolyNode* n : node->Childs)
			if (z < (tmp = pathMaxZ(n->Contour)))
				z = tmp;
		return z;
	}

	void merge2SimplePoly(ClipperLibXYZ::PolyNode* polyNode, SimplePoly* newPoly, bool invert)
	{
		if (!polyNode || !newPoly)
			return;

		size_t size = polyNode->Contour.size();
		size_t childCount = polyNode->Childs.size();
		std::list<ClipperLibXYZ::PolyNode*> insertedPolys;
		if (childCount > 0)
		{
			std::vector<ClipperLibXYZ::cInt> maxX(childCount);
			std::vector<int> nodes(childCount);
			for (size_t i = 0; i < childCount; ++i)
			{
				maxX.at(i) = pathMaxX(polyNode->Childs.at(i)->Contour);
				nodes.at(i) = (int)i;
				size += polyNode->Childs.at(i)->Contour.size();
			}

			std::sort(nodes.begin(), nodes.end(), [&maxX](int i, int j)->bool {
				return maxX[i] < maxX[j];
				});

			for (size_t i = 0; i < childCount; ++i)
				insertedPolys.push_back(polyNode->Childs.at(nodes.at(i)));
		}

		newPoly->reserve(size + 50);
		if (invert)
		{
			int cSize = (int)polyNode->Contour.size();
			if (cSize > 0)
			{
				for(int i = cSize - 1; i >= 0; --i)
					newPoly->push_back(&polyNode->Contour.at(i));
			}
		}
		else
		{
			for (ClipperLibXYZ::IntPoint& p : polyNode->Contour)
				newPoly->push_back(&p);
		}

		while (insertedPolys.size() > 0)
		{
			//ClipperLibXYZ::Path* insertPoly = nullptr;
			std::vector<ClipperLibXYZ::IntPoint*> insertPoly;
			int cSize = (int)insertedPolys.back()->Contour.size();
			ClipperLibXYZ::Path& insertPath = insertedPolys.back()->Contour;
			insertPoly.reserve(cSize);
			if (invert && cSize > 0)
			{
				for (int i = cSize - 1; i >= 0; --i)
					insertPoly.push_back(&insertPath.at(i));
			}
			else
			{
				for (int i = 0; i < cSize; ++i)
					insertPoly.push_back(&insertPath.at(i));
			}

			insertedPolys.pop_back();

			int innerSize = (int)insertPoly.size();
			float mx = -10000000.0f;

			ClipperLibXYZ::IntPoint* maxVertex = nullptr;
			int vertexIndex = -1;
			for (int i = 0; i < innerSize; ++i)
			{
				trimesh::dvec3 v = CInt2VD(*insertPoly.at(i));
				if (v.x > mx)
				{
					mx = v.x;
					vertexIndex = i;
					maxVertex = insertPoly.at(i);
				}
			}

			if (maxVertex)
			{
				trimesh::dvec3 tVertex = CInt2VD(*maxVertex);
				int nvert = (int)newPoly->size();
				int i, j = 0;
				double cmx = 1000000.0;
				int cOuterIndex = -1;
				int cOuterIndex0 = -1;
				for (i = 0, j = nvert - 1; i < nvert; j = i++)
				{
					trimesh::dvec3 verti = CInt2VD(*newPoly->at(i));
					trimesh::dvec3 vertj = CInt2VD(*newPoly->at(j));
					if (verti.y == tVertex.y && vertj.y == tVertex.y)
					{
						double mmx = verti.x > vertj.x ? vertj.x : verti.x;
						if (mmx > tVertex.x && mmx < cmx)
						{
							cOuterIndex = i;
							cOuterIndex0 = j;
							cmx = mmx;
						}
					}
					else if ((verti.y > tVertex.y) != (vertj.y > tVertex.y))
					{
						double cx = (vertj.x - verti.x) * (tVertex.y - verti.y) / (vertj.y - verti.y) + verti.x;
						if (cx > tVertex.x)  // must 
						{
							if (cx < cmx)
							{
								cOuterIndex = i;
								cOuterIndex0 = j;
								cmx = cx;
							}
							else if (cx == cmx)
							{  // collide two opposite edge
								trimesh::dvec3 xxn(1.0, 0.0, 0.0);
								trimesh::dvec3 nji = verti - vertj;
								if (crossValue(xxn, nji) >= 0.0)
								{
									cOuterIndex = i;
									cOuterIndex0 = j;
									cmx = cx;
								}
							}
						}
					}
				}

				int mutaulIndex = -1;
				if (cOuterIndex >= 0)
				{
					if (cmx == INT2MM(newPoly->at(cOuterIndex)->X)
						&& tVertex.y == INT2MM(newPoly->at(cOuterIndex)->Y))
						mutaulIndex = cOuterIndex;
					else if (cmx == INT2MM(newPoly->at(cOuterIndex0)->X)
						&& tVertex.y == INT2MM(newPoly->at(cOuterIndex0)->Y))
					{
						mutaulIndex = cOuterIndex0;
					}
					else
					{
						trimesh::dvec3 M = tVertex;
						if (newPoly->at(cOuterIndex)->X < newPoly->at(cOuterIndex0)->X)
						{
							cOuterIndex = cOuterIndex0;
						}
						trimesh::dvec3 P = CInt2VD(*newPoly->at(cOuterIndex));
						trimesh::dvec3 I = trimesh::dvec3(cmx, M.y, 0.0);
						if (P.y > I.y)
						{
							trimesh::dvec3 T = P;
							P = I;
							I = T;
						}

						std::vector<int> reflexVertex;
						for (i = 0; i < nvert; ++i)
						{
							trimesh::dvec3 tv = CInt2VD(*newPoly->at(i));
							if ((i != cOuterIndex) && (newPoly->at(i) != newPoly->at(cOuterIndex)) && insideTriangle(M, P, I, tv))
							{
								reflexVertex.push_back(i);
							}
						}

						if (reflexVertex.size() == 0)
						{
							mutaulIndex = cOuterIndex;
						}
						else
						{
							int reflexSize = reflexVertex.size();
							double minLen = 1000000.0;
							double maxDot = -10000.0;
							int minReflexIndex = 0;
							for (i = 0; i < reflexSize; ++i)
							{
								trimesh::dvec3 R = CInt2VD(*newPoly->at(reflexVertex.at(i)));
								trimesh::dvec3 MR = R - M;
								double len = trimesh::len(MR);
								trimesh::normalize(MR);
								double dot = abs(dotValue(MR, trimesh::dvec3(1.0, 0.0, 0.0)));
								if (dot > maxDot && len < minLen)
								{
									minReflexIndex = i;
									minLen = len;
									maxDot = dot;
								}
							}

							mutaulIndex = reflexVertex.at(minReflexIndex);
						}
					}
				}

				if (mutaulIndex >= 0)
				{// merge mutaulIndex in outer and vertexIndex in inner
					std::vector<ClipperLibXYZ::IntPoint*> mergedPolygon;
					for (i = 0; i < nvert; ++i)
					{
						mergedPolygon.push_back(newPoly->at(i));
						if (i == mutaulIndex)
						{// insert inner
							for (j = vertexIndex; j < innerSize; ++j)
								mergedPolygon.push_back(insertPoly.at(j));
							for (j = 0; j <= vertexIndex; ++j)
								mergedPolygon.push_back(insertPoly.at(j));
							mergedPolygon.push_back(newPoly->at(i));
						}
					}
					newPoly->swap(mergedPolygon);
				}
			}
		}
	}
}