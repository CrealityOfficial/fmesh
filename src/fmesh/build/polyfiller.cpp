#include "polyfiller.h"
#include "earpolygon.h"

RefTriangle* fillSimplePoly(RefPoly* poly)
{
	EarPolygon ear;
	return ear.earClippingFromRefPoly(poly);
}

void fillComplexLayerNode(Layer* root, std::vector<RefTriangle*>& triangles)
{
	if (!root)
		return;

	typedef std::function<void(LayerNode*)> nodeFunc;
	nodeFunc func = [&func, &triangles](LayerNode* node) {
		if (node->depth % 2 == 1)
		{
			std::vector<RefPoly*> cpolys;
			for (LayerNode* n : node->children)
				cpolys.push_back(n->poly);
			RefTriangle* tri = fillOneLevel(node->poly, cpolys);
			if (tri) triangles.push_back(tri);
		}

		for (LayerNode* n : node->children)
			func(n);
	};
	
	func(root);
}

void fillLayerFirstLevel(Layer* root, std::vector<RefTriangle*>& triangles)
{
	if (!root)
		return;

	for (LayerNode* node : root->children)
	{
		RefTriangle* tri = fillSimplePoly(node->poly);
		if (tri) triangles.push_back(tri);
	}
}

void fillLayerAllInner(Layer* root, std::vector<RefTriangle*>& triangles)
{
	if (!root)
		return;

	typedef std::function<void(LayerNode*)> nodeFunc;
	nodeFunc func = [&func, &triangles](LayerNode* node) {
		if (node->depth > 0 && node->depth % 2 == 0)
		{
			RefPoly iPoly;
			invertRef(&iPoly, node->poly);
			size_t cSize = node->children.size();
			std::vector<RefPoly> iPolys;
			if (cSize > 0)
			{
				iPolys.resize(cSize);
				for (size_t i = 0; i < cSize; ++i)
					invertRef(&iPolys.at(i), node->children.at(i)->poly);
			}

			std::vector<RefPoly*> cpolys;
			for (RefPoly& poly : iPolys)
				cpolys.push_back(&poly);
			RefTriangle* tri = fillOneLevel(&iPoly, cpolys);
			if (tri) triangles.push_back(tri);
		}

		for (LayerNode* n : node->children)
			func(n);
	};

	func(root);
}

RefTriangle* fillOneLevel(RefPoly* outerPoly, std::vector<RefPoly*>& innerPolys)
{
	std::unique_ptr<RefPoly> newPoly(merge2SimpleRefPoly(outerPoly, innerPolys));
	return fillSimplePoly(newPoly.get());
}

RefPoly* merge2SimpleRefPoly(RefPoly* outerPoly, std::vector<RefPoly*>& innerPolys)
{
	RefPoly* newPoly = new RefPoly();
	*newPoly = *outerPoly;

	std::list<RefPoly*> insertedPolys;
	for (RefPoly* poly : innerPolys)
		insertedPolys.push_back(poly);

	while (insertedPolys.size() > 0)
	{
		RefPoly* insertPoly = insertedPolys.back();
		insertedPolys.pop_back();

		int innerSize = (int)insertPoly->size();
		float mx = -10000000.0f;

		RefVertex* maxVertex = nullptr;
		int vertexIndex = -1;
		for (int i = 0; i < innerSize; ++i)
		{
			trimesh::dvec3& v = insertPoly->at(i)->v;
			if (v.x > mx)
			{
				mx = v.x;
				vertexIndex = i;
				maxVertex = insertPoly->at(i);
			}
		}

		if (maxVertex)
		{
			trimesh::dvec3& tVertex = maxVertex->v;
			int nvert = (int)newPoly->size();
			int i, j = 0;
			double cmx = 1000000.0;
			int cOuterIndex = -1;
			int cOuterIndex0 = -1;
			for (i = 0, j = nvert - 1; i < nvert; j = i++)
			{
				trimesh::dvec3& verti = newPoly->at(i)->v;
				trimesh::dvec3& vertj = newPoly->at(j)->v;
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
				if (cmx == newPoly->at(cOuterIndex)->v.x)
					mutaulIndex = cOuterIndex;
				else if (cmx == newPoly->at(cOuterIndex0)->v.x)
				{
					mutaulIndex = cOuterIndex0;
				}
				else
				{
					trimesh::dvec3 M = tVertex;
					if (newPoly->at(cOuterIndex)->v.x < newPoly->at(cOuterIndex0)->v.x)
					{
						cOuterIndex = cOuterIndex0;
					}
					trimesh::dvec3 P = newPoly->at(cOuterIndex)->v;
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
						trimesh::dvec3& tv = newPoly->at(i)->v;
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
							trimesh::dvec3 R = newPoly->at(reflexVertex.at(i))->v;
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
				std::vector<RefVertex*> mergedPolygon;
				for (i = 0; i < nvert; ++i)
				{
					mergedPolygon.push_back(newPoly->at(i));
					if (i == mutaulIndex)
					{// insert inner
						for (j = vertexIndex; j < innerSize; ++j)
							mergedPolygon.push_back(insertPoly->at(j));
						for (j = 0; j <= vertexIndex; ++j)
							mergedPolygon.push_back(insertPoly->at(j));
						mergedPolygon.push_back(newPoly->at(i));
					}
				}
				newPoly->swap(mergedPolygon);
			}
		}
	}

	return newPoly;
}