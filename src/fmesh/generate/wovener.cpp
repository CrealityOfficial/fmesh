#include "wovener.h"
#include "fmesh/generate/triangularization.h"

namespace fmesh
{
	Wovener::Wovener()
		:m_pathLower(nullptr)
		, m_pathUp(nullptr)
		, m_lowerStart(0)
		, m_upStart(0)
		, m_lowerSize(0)
		, m_upSize(0)
	{

	}

	Wovener::~Wovener()
	{

	}

	Patch* Wovener::woven(ClipperLib::Path* pathLower, ClipperLib::Path* pathUp)
	{
//#ifdef 1
		////save path
		//static int i = 0;
		//char a[128];
		//sprintf(a, "%d", i++);
		//std::string file = "f:/0/";
		//file += a;
		//ClipperLib::Paths pathlowers;
		//pathlowers.push_back(*pathLower);
		//ClipperLib::Paths pathUps;
		//pathUps.push_back(*pathUp);
		//if (pathLower->size())
		//	ClipperLib::save(pathlowers, file + ".path");
		//if (pathUp->size())
		//	ClipperLib::save(pathUps, file + "_1.path");
		//save end
//#endif // 1

		if (!pathLower || !pathUp)
			return nullptr;

		m_pathLower = pathLower;
		m_pathUp = pathUp;

		m_lowerSize = (int)pathLower->size();
		m_upSize = (int)pathUp->size();
		if (m_lowerSize < 1 || m_upSize < 1)
			return nullptr;

		m_lowerStart = 0;
		m_upStart = findNearest();
		int lowerProcessedSize = 0;
		int upProcessedSize = 0;

		Patch* patch = new Patch();
		patch->reserve(3 * (m_lowerSize + m_upSize) + 4);
		while (lowerProcessedSize < (m_lowerSize - 1) && upProcessedSize < (m_upSize - 1))
			processQuad(lowerProcessedSize, upProcessedSize, patch);

		if (lowerProcessedSize == m_lowerSize - 1)
		{
			int last = m_lowerSize - 1;
			while (upProcessedSize != m_upSize - 1)
			{
				patch->push_back(CInt2V(m_pathLower->at(last)));
				patch->push_back(CInt2V(m_pathUp->at((upProcessedSize + 1 + m_upStart) % m_upSize)));
				patch->push_back(CInt2V(m_pathUp->at((upProcessedSize + m_upStart) % m_upSize)));
		
				upProcessedSize++;
			}
		}
		else if(upProcessedSize == m_upSize - 1)
		{
			int last = (upProcessedSize + m_upStart) % m_upSize;
			while (lowerProcessedSize != m_lowerSize - 1)
			{
				patch->push_back(CInt2V(m_pathUp->at(last)));
				patch->push_back(CInt2V(m_pathLower->at(lowerProcessedSize)));
				patch->push_back(CInt2V(m_pathLower->at(lowerProcessedSize + 1)));
				lowerProcessedSize++;
			}
		}

		processLast(patch);
		return patch;
	}

	int Wovener::findNearest()
	{
		int upIndex = 0;
		float dmin = 10000.0f;
		trimesh::vec3 lowerP = CInt2V(m_pathLower->at(0));
		for (int i = 0; i < m_upSize; ++i)
		{
			trimesh::vec3 P = CInt2V(m_pathUp->at(i));
			float d = trimesh::len(P - lowerP);
			if (d < dmin)
			{
				upIndex = i;
				dmin = d;
			}
		}

		return upIndex;
	}

	void Wovener::processQuad(int& lowerIndex, int& upIndex, Patch* patch)
	{
		trimesh::vec3 up0 = CInt2V(m_pathUp->at((upIndex + m_upStart) % m_upSize));
		trimesh::vec3 up1 = CInt2V(m_pathUp->at((upIndex + 1 + m_upStart) % m_upSize));
		trimesh::vec3 lower0 = CInt2V(m_pathLower->at(lowerIndex));
		trimesh::vec3 lower1 = CInt2V(m_pathLower->at(lowerIndex + 1));
		std::vector<trimesh::vec3> polygon;
		//polygon.clear();
		polygon.push_back(up0);
		polygon.push_back(lower0);

		float u0l1 = trimesh::len2(up0 - lower1);
		float u1l0 = trimesh::len2(up1 - lower0);

		if (u0l1 < u1l0 )
		{
			polygon.push_back(lower1);
			//if (inside(polygon, up1))
			{
				//patch->push_back(up0);
				//patch->push_back(lower0);
				//patch->push_back(up1);
				//upIndex++;
			}
			//else
			{
				patch->push_back(up0);
				patch->push_back(lower0);
				patch->push_back(lower1);
				lowerIndex++;
			}
		}
		else
		{
			polygon.push_back(up1);
			//if (inside(polygon, lower1))
			{
				//patch->push_back(up0);
				//patch->push_back(lower0);
				//patch->push_back(lower1);
				//lowerIndex++;
			}
			//else
			{
				patch->push_back(up0);
				patch->push_back(lower0);
				patch->push_back(up1);
				upIndex++;
			}
		}
	}

	bool Wovener::inside(std::vector<trimesh::vec3>& polygon, trimesh::vec3& p)
	{
		if (polygon.size() < 1)
			return false;
		int crossings = 0;
		trimesh::vec3 p0 = polygon[polygon.size() - 1];
		for (unsigned int n = 0; n < polygon.size(); n++)
		{
			trimesh::vec3 p1 = polygon[n];
			if ((p0.at(1) >= p.at(1) && p1.at(1) < p.at(1)) || (p1.at(1) > p.at(1) && p0.at(1) <= p.at(1)))
			{
				int64_t x = p0.at(0) + (p1.at(0) - p0.at(0)) * (p.at(1) - p0.at(1)) / (p1.at(1) - p0.at(1));
				if (x >= p.at(0))
					crossings++;
			}
			p0 = p1;
		}
		return (crossings % 2) == 1;
	}

	void Wovener::processLast(Patch* patch)
	{
		trimesh::vec3 up0 = CInt2V(m_pathUp->at((m_upStart + m_upSize - 1) % m_upSize));
		trimesh::vec3 up1 = CInt2V(m_pathUp->at(m_upStart));
		trimesh::vec3 lower0 = CInt2V(m_pathLower->at(m_lowerSize - 1));
		trimesh::vec3 lower1 = CInt2V(m_pathLower->at(0));

		float u0l1 = trimesh::len2(up0 - lower1);
		float u1l0 = trimesh::len2(up1 - lower0);
		if (u0l1 < u1l0)
		{
			patch->push_back(up0);
			patch->push_back(lower0);
			patch->push_back(lower1);
			patch->push_back(up0);
			patch->push_back(lower1);
			patch->push_back(up1);
		}
		else
		{
			patch->push_back(up0);
			patch->push_back(lower0);
			patch->push_back(up1);
			patch->push_back(lower0);
			patch->push_back(lower1);
			patch->push_back(up1);
		}
	}
}