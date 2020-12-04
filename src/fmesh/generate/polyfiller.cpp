#include "polyfiller.h"
#include "fmesh/generate/earpolygon.h"
#include "fmesh/generate/triangularization.h"
#include "fmesh/common/dvecutil.h"
#include "mmesh/clipper/circurlar.h"
#include "fmesh/contour/contour.h"

namespace fmesh
{
	Patch* fillSimplePolyTree(SimplePoly* poly)
	{
		EarPolygon ear;
		return ear.earClippingFromRefPoly(poly);
	}

	Patch* fillSimplePolyTree(ClipperLib::Path* path)
	{
		EarPolygon ear;
		return ear.earClippingFromPath(path);
	}

	void fillComplexPolyTree(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;

		polyNodeFunc func = [&patches](ClipperLib::PolyNode* node) {
			if (!node->IsHole())
			{
				Patch* patch = fillOneLevelPolyNode(node);
				if (patch) 
					patches.push_back(patch);
			}
		};

		mmesh::loopPolyTree(func, polyTree);
	}

	void fillComplexPolyTreeReverseInner(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;

		int parentChilds = 0;
		polyNodeFunc func = [&patches,&parentChilds](ClipperLib::PolyNode* node) {
			if (!node->IsHole())
			{
				Patch* patch = fillOneLevelPolyNode(node);
				if (parentChilds)
				{
					Patch* _path = new Patch;
					for (int i = patch->size() - 1; i >= 0; i--)
					{
						_path->push_back(patch->at(i));
					}
					--parentChilds;
					patches.push_back(_path);					
				}
				else
				{
					patches.push_back(patch);
					parentChilds = node->ChildCount();
				}					 				
			}
		};

		mmesh::loopPolyTree(func, polyTree);
	}

	Patch* fillOneLevelPolyNode(ClipperLib::PolyNode* polyNode, bool invert)
	{
		SimplePoly poly;
		merge2SimplePoly(polyNode, &poly, invert);
		return fillSimplePolyTree(&poly);
	}

	void fillFirstLevelPolyNode(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;

		for (ClipperLib::PolyNode* node : polyTree->Childs)
		{
			Patch* patch = fillSimplePolyTree(&node->Contour);
			if (patch) patches.push_back(patch);
		}
	}

	void fillPolyNodeInner(ClipperLib::PolyTree* polyTree, std::vector<Patch*>& patches)
	{
		if (!polyTree)
			return;

		polyNodeFunc func = [&patches](ClipperLib::PolyNode* node) {
			if (node->IsHole() && node->Parent)
			{
				Patch* patch = fillOneLevelPolyNode(node, true);
				if (patch) patches.push_back(patch);
			}
		};

		mmesh::loopPolyTree(func, polyTree);
	}

	void fillPolyNodeOutline(ClipperLib::PolyTree* polyTree1, std::vector<Patch*>& patches)
	{
		if (!polyTree1 )
			return;

		SimplePoly* newploy =new SimplePoly();
		merge2SimplePoly(polyTree1->Childs.at(0), newploy, 0);
		Patch* patch = fillSimplePolyTree(newploy);
		if (patch) patches.push_back(patch);

	}

	void dealPolyTreeAxisZ(ClipperLib::PolyTree* polyTree, double slope, double dminY)
	{
		if (!polyTree)
			return;
		polyNodeFunc func = [&slope,&dminY](ClipperLib::PolyNode* node) {
			for (size_t i = 0; i < node->Contour.size(); i++)
			{
				node->Contour.at(i).Z += slope * (node->Contour.at(i).Y/* - dminY*/);
			}
		};

		mmesh::loopPolyTree(func, polyTree);
	}

	ClipperLib::cInt pathMaxX(ClipperLib::Path& path)
	{
		ClipperLib::cInt x = -999999;
		for (ClipperLib::IntPoint& point : path)
		{
			if (point.X > x)
				x = point.X;
		}
		return x;
	}

	void merge2SimplePoly(ClipperLib::PolyNode* polyNode, SimplePoly* newPoly, bool invert)
	{
		if (!polyNode || !newPoly)
			return;

		size_t size = polyNode->Contour.size();
		size_t childCount = polyNode->Childs.size();
		std::list<ClipperLib::PolyNode*> insertedPolys;
		if (childCount > 0)
		{
			std::vector<ClipperLib::cInt> maxX(childCount);
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
			for (ClipperLib::IntPoint& p : polyNode->Contour)
				newPoly->push_back(&p);
		}

		while (insertedPolys.size() > 0)
		{
			ClipperLib::Path reversePath;
			ClipperLib::Path* insertPoly = nullptr;
			insertPoly = &insertedPolys.back()->Contour;
			if (invert)
			{
				int cSize = insertPoly->size();
				reversePath.reserve(cSize);
				if (cSize > 0)
				{
					for (int i = cSize - 1; i >= 0; --i)
						newPoly->push_back(&polyNode->Contour.at(i));
				}
				insertPoly = &reversePath;
			}

			insertedPolys.pop_back();

			int innerSize = (int)insertPoly->size();
			float mx = -10000000.0f;

			ClipperLib::IntPoint* maxVertex = nullptr;
			int vertexIndex = -1;
			for (int i = 0; i < innerSize; ++i)
			{
				trimesh::dvec3& v = CInt2VD(insertPoly->at(i));
				if (v.x > mx)
				{
					mx = v.x;
					vertexIndex = i;
					maxVertex = &insertPoly->at(i);
				}
			}

			if (maxVertex)
			{
				trimesh::dvec3& tVertex = CInt2VD(*maxVertex);
				int nvert = (int)newPoly->size();
				int i, j = 0;
				double cmx = 1000000.0;
				int cOuterIndex = -1;
				int cOuterIndex0 = -1;
				for (i = 0, j = nvert - 1; i < nvert; j = i++)
				{
					trimesh::dvec3& verti = CInt2VD(*newPoly->at(i));
					trimesh::dvec3& vertj = CInt2VD(*newPoly->at(j));
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
							trimesh::dvec3& tv = CInt2VD(*newPoly->at(i));
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
					std::vector<ClipperLib::IntPoint*> mergedPolygon;
					for (i = 0; i < nvert; ++i)
					{
						mergedPolygon.push_back(newPoly->at(i));
						if (i == mutaulIndex)
						{// insert inner
							for (j = vertexIndex; j < innerSize; ++j)
								mergedPolygon.push_back(&insertPoly->at(j));
							for (j = 0; j <= vertexIndex; ++j)
								mergedPolygon.push_back(&insertPoly->at(j));
							mergedPolygon.push_back(newPoly->at(i));
						}
					}
					newPoly->swap(mergedPolygon);
				}
			}
		}
	}
}