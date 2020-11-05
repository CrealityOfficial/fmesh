#include "tribuilder.h"
#include "Plygon.h"
#include "ContourLineRecon.h"

void buildFromRefPolies(std::vector<RefPoly*>& ref1, std::vector<RefPoly*>& ref2, RefTriangle& refTri)
{
	size_t size = ref1.size();
	size_t tsize = ref2.size();
	if (size == tsize)
	{
		for (size_t i = 0; i < size; ++i)
		{
			//buildFromRefPoly(ref1.at(i), ref2.at(i), refTri);
			buildFromRefPoly_alg(ref1.at(i), ref2.at(i), refTri);
			//buildFromRefPoly_alg(ref1.at(i), ref2.at(i), refTri);
		}
			
	}
}

void buildFromRefPoly(RefPoly* ref1, RefPoly* ref2, RefTriangle& refTri)
{
	size_t size = ref1->size();
	if (size == ref2->size() && size > 2)
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

			refTri.push_back(ref1->at(i1));
			refTri.push_back(ref1->at(i2));
			refTri.push_back(ref2->at(i4));
			refTri.push_back(ref1->at(i1));
			refTri.push_back(ref2->at(i4));
			refTri.push_back(ref2->at(i3));
		}
	}
}

void buildFromRefPoly_alg(RefPoly* ref1, RefPoly* ref2, RefTriangle& refTri)
{
	if (ref1->size() == 0 || ref2->size() == 0)
	{
		return;
	}

	ContourLine line1;
	size_t size = ref1->size();
	for (int i = 0; i < size; ++i)
	{
		line1.AddPoint2d(ref1->at(i)->v.x, ref1->at(i)->v.y);
	}
	line1.SetZ(ref1->at(0)->v.z);
	//line1.SetZ(100);

	ContourLine line2;
	size = ref2->size();
	for (int i = 0; i < size; ++i)
	{
		line2.AddPoint2d(ref2->at(i)->v.x, ref2->at(i)->v.y);
	}
	line2.SetZ(ref2->at(0)->v.z);
	//line1.SetZ(200);

	std::vector<ContourLine*> list;
	list.push_back(&line1);
	list.push_back(&line2);

	ContourLineSurfaceGenerator cls(list);
	Mesh* m = cls.GenerateSurface();

//	PlyManager::Output(*m, "Test_ContourLine.ply");

// 	trimesh::TriMesh* mesh = new trimesh::TriMesh();
// 	mesh->vertices.resize(m->Vertices.size());
// 	mesh->faces.resize(m->Faces.size());
// 
// 	for (size_t i = 0; i < m->Vertices.size(); i++)
// 	{
// 		mesh->vertices[i].x = m->Vertices[i].X;
// 		mesh->vertices[i].y = m->Vertices[i].Y;
// 		mesh->vertices[i].z = m->Vertices[i].Z;
// 	}
// 	for (size_t i = 0; i < m->Faces.size(); i++)
// 	{
// 		mesh->faces[i].x = m->Faces[i].P0Index;
// 		mesh->faces[i].y = m->Faces[i].P1Index;
// 		mesh->faces[i].z = m->Faces[i].P2Index;
// 	}
// 
// 	mesh->write("algtest.stl");

	bool bDone = false;
	std::vector<int> vecIndex;
	for (size_t i = 0; i < m->Faces.size(); i++)
	{	
		vecIndex.clear();	
		vecIndex.push_back(m->Faces[i].P0Index);
		vecIndex.push_back(m->Faces[i].P1Index);
		vecIndex.push_back(m->Faces[i].P2Index);
		for (int index=0; index <3; index++)
		{
			bDone = false;
			if (!bDone)
			{
				for (size_t j = 0; j < ref1->size(); j++)
				{
					if (ref1->at(j)->v.x == m->Vertices.at(vecIndex[index]).X
						&& ref1->at(j)->v.y == m->Vertices.at(vecIndex[index]).Y
						&& ref1->at(j)->v.z == m->Vertices.at(vecIndex[index]).Z)
					{
						refTri.push_back(ref1->at(j));
						bDone = true;
						break;
					}
				}
			}

			if (!bDone)
			{
				for (size_t k = 0; k < ref2->size(); k++)
				{
					if (ref2->at(k)->v.x == m->Vertices.at(vecIndex[index]).X
						&& ref2->at(k)->v.y == m->Vertices.at(vecIndex[index]).Y
						&& ref2->at(k)->v.z == m->Vertices.at(vecIndex[index]).Z)
					{
						refTri.push_back(ref2->at(k));
						bDone = true;
						break;
					}
				}
			}
		}
	}
}
