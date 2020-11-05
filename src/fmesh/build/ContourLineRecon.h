#ifndef CONTOURLINERECON_H
#define CONTOURLINERECON_H
#include <algorithm>
#include "Plygon.h"

class ContourStitcher
{
private:
	struct QuadUnit
	{
	public:
		int UpIndex0;
		int UpIndex1;
		int DownIndex0;
		int DownIndex1;
		double DiaU0D1Len;
		double DiaU1D0Len;
		std::vector<FloatDouble>* lineUp;
		std::vector<FloatDouble>* lineDown;
		void Init(int upIndex, int downIndex)
		{
			UpIndex0 = upIndex;
			DownIndex0 = downIndex;
			UpIndex1 = (upIndex + 1); //%lineUp.Count;
			DownIndex1 = (downIndex + 1); //%lineDown.Count;
			DiaU0D1Len = GetDLen(UpIndex0, DownIndex1);
			DiaU1D0Len = GetDLen(UpIndex1, DownIndex0);
		}
		void InitLast()
		{
			UpIndex0 = lineUp->size() - 1;
			DownIndex0 = lineDown->size() - 1;
			UpIndex1 = 0; //%lineUp.Count;
			DownIndex1 = 0; //%lineDown.Count;
			DiaU0D1Len = GetDLen(UpIndex0, DownIndex1);
			DiaU1D0Len = GetDLen(UpIndex1, DownIndex0);
		}
		double GetDLen(int index1, int index2)
		{
			float x0 = (*lineUp)[index1].X;
			float y0 = (*lineUp)[index1].Y;
			float x1 = (*lineDown)[index2].X;
			float y1 = (*lineDown)[index2].Y;
			return sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
		}
	};
	void Transform(std::vector<FloatDouble>& list, float dx, float dy)
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			list[i].X += dx;
			list[i].Y += dy;
		}
	}
	Point3d GetCenter(Box3Float& box3)
	{
		return Point3d((box3.Max3[0] + box3.Min3[0]) / 2.0f, (box3.Max3[1] + box3.Min3[1]) / 2.0f, (box3.Max3[2] + box3.Min3[2]) / 2.0f);
	}
public:
	ContourLine* lineUp;
	ContourLine* lineDown;
	std::vector<FloatDouble> lineUpProcessed;
	std::vector<FloatDouble> lineDownProcessed;
	Box3Float boxUp;
	Box3Float boxDown;
	ContourStitcher(ContourLine* line1, ContourLine* line2)
	{
		if (line1->GetZ() > line2->GetZ())
		{
			this->lineUp = line1;
			this->lineDown = line2;
		}
		else
		{
			this->lineUp = line2;
			this->lineDown = line1;
		}
		lineUpProcessed.reserve(lineUp->GetLinePointCount());
		lineDownProcessed.reserve(lineDown->GetLinePointCount());
		CopyArray(lineUp->GetPointList(), lineUpProcessed);
		CopyArray(lineDown->GetPointList(), lineDownProcessed);
		boxUp = lineUp->GetBox();
		boxDown = lineDown->GetBox();
		Point3d cU = GetCenter(boxUp);
		Point3d cD = GetCenter(boxDown);
		Transform(lineDownProcessed, -cD.X, -cD.Y);
		Transform(lineUpProcessed, -cU.X, -cU.Y);
		int indexDown = GetNearIndex();
		AdjustDownArray(indexDown);
	}
	~ContourStitcher()
	{
		lineUp=NULL;
		lineDown=NULL;
	}
private:
	void CopyArray(std::vector<FloatDouble> &olist, std::vector<FloatDouble> &tarlist)
	{
		for (size_t i = 0; i < olist.size(); i++)
		{
			tarlist.push_back(FloatDouble(olist[i].X, olist[i].Y));
		}
	}
	int ContourStitcher::GetNearIndex()
	{
		int index = -1;
		double distense = DBL_MAX;
		FloatDouble& p = lineUpProcessed[0];
		float x0 = p.X;
		float y0 = p.Y;
		for (size_t i = 0; i < lineDownProcessed.size(); i++)
		{
			float x1 = lineDownProcessed[i].X;
			float y1 = lineDownProcessed[i].Y;
			double dis = sqrt((x0 - x1) * (x0 - x1) + (y0 - y1) * (y0 - y1));
			if (dis < distense)
			{
				distense = dis;
				index = i;
			}
		}
		return index;
	}
	void AdjustDownArray(int indexDown)
	{
		std::vector<FloatDouble> list;
		list.reserve(lineDownProcessed.size());
		for (size_t i = 0; i < lineDownProcessed.size(); i++)
		{
			list.push_back(lineDownProcessed[(indexDown + i) % lineDownProcessed.size()]);
		}
		lineDownProcessed.swap(list);
	}
public:
	Mesh* DoStitching()
	{
		Mesh *m = new Mesh();
		int* upMap=new int[lineUpProcessed.size()];
		float dx1 = GetCenter(boxUp).X;
		float dy1 = GetCenter(boxUp).Y;
		float dx2 = GetCenter(boxDown).X;
		float dy2 =GetCenter( boxDown).Y;
		int* downMap=new int[lineDownProcessed.size()];
		for (size_t i = 0; i < lineDownProcessed.size(); i++)
		{
			Point3d p(lineDownProcessed[i].X + dx2, lineDownProcessed[i].Y + dy2, lineDown->GetZ());
			downMap[i] = m->AddVertex(p);
		}
		for (size_t i = 0; i < lineUpProcessed.size(); i++)
		{
			Point3d p(lineUpProcessed[i].X + dx1, lineUpProcessed[i].Y + dy1, lineUp->GetZ());
			upMap[i] = m->AddVertex(p);
		}
		size_t upIndex = 0;
		size_t downIndex = 0;
		QuadUnit quad;
		quad.lineDown = &lineDownProcessed;
		quad.lineUp = &lineUpProcessed;
		while (true)
		{
			if (upIndex == lineUpProcessed.size() - 1 || downIndex == lineDownProcessed.size() - 1)
			{
				break;
			}
			quad.Init(upIndex, downIndex);
			if (quad.DiaU0D1Len < quad.DiaU1D0Len)
			{
				Triangle t(upMap[quad.UpIndex0], downMap[quad.DownIndex0], downMap[quad.DownIndex1]);
				m->AddFace(t);
				downIndex++;
			}
			else
			{
				Triangle t(upMap[quad.UpIndex0], downMap[quad.DownIndex0], upMap[quad.UpIndex1]);
				m->AddFace(t);
				upIndex++;
			}
		}
		if (upIndex == lineUpProcessed.size() - 1 || downIndex == lineDownProcessed.size() - 1)
		{
			if (downIndex == lineDownProcessed.size() - 1)
			{
				int last = lineDownProcessed.size() - 1;
				while (upIndex != lineUpProcessed.size() - 1)
				{
					Triangle t(downMap[last], upMap[upIndex + 1], upMap[upIndex]);
					m->AddFace(t);
					upIndex++;
				}
			}
			else
			{
				int last = lineUpProcessed.size() - 1;
				while (downIndex != lineDownProcessed.size() - 1)
				{
					Triangle t(upMap[last], downMap[downIndex], downMap[downIndex + 1]);
					m->AddFace(t);
					downIndex++;
				}
			}
		}
		quad.InitLast();
		if (quad.DiaU0D1Len < quad.DiaU1D0Len)
		{
			Triangle t(upMap[quad.UpIndex0], downMap[quad.DownIndex0], downMap[quad.DownIndex1]);
			Triangle t2(upMap[quad.UpIndex0], downMap[quad.DownIndex1], upMap[quad.UpIndex1]);
			m->AddFace(t);
			m->AddFace(t2);
		}
		else
		{
			Triangle t(upMap[quad.UpIndex0], downMap[quad.DownIndex0], upMap[quad.UpIndex1]);
			Triangle t2(upMap[quad.UpIndex1], downMap[quad.DownIndex0], downMap[quad.DownIndex1]);
			m->AddFace(t);
			m->AddFace(t2);
		}
		delete[] upMap;
		delete[] downMap;
		return m;
	}
};

class ContourLineSurfaceGenerator
{
private:
	static bool CompareContourline(ContourLine* r1,ContourLine* r2)
	{
		return r1->GetZ() < r2->GetZ();
	}
	void Transform(std::vector<FloatDouble>& list, float dx, float dy)
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			list[i].X += dx;
			list[i].Y += dy;
		}
	}
	Point3d GetCenter(Box3Float& box3)
	{
		return Point3d((box3.Max3[0] + box3.Min3[0]) / 2.0f, (box3.Max3[1] + box3.Min3[1]) / 2.0f, (box3.Max3[2] + box3.Min3[2]) / 2.0f);
	}
private:
	std::vector<ContourLine*> lines;
public:
	ContourLineSurfaceGenerator(std::vector<ContourLine*> lines)
	{
		this->lines = lines;
	}
	~ContourLineSurfaceGenerator()
	{

	}
private:
	void ReverseNormal(std::vector<Triangle> &list)
	{
		for (size_t i = 0; i < list.size(); i++)
		{
			int temp = list[i].P0Index;
			list[i].P0Index = list[i].P2Index;
			list[i].P2Index = temp;
		}
	}
	bool IsNormalZ2(std::vector<FloatDouble> &vertices, std::vector<Triangle> &faces)
	{
		if (faces.empty())
		{
			return true;
		}
		else
		{
			Triangle &t = faces[0];
			Point3d p0(vertices[t.P0Index].X, vertices[t.P0Index].Y, 0.0f);
			Point3d p1(vertices[t.P1Index].X, vertices[t.P1Index].Y, 0.0f);
			Point3d p2(vertices[t.P2Index].X, vertices[t.P2Index].Y, 0.0f);

			Vector v = Triangle::CaculateNormal(p0,p1,p2);
			if (v.Z > 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	void FillDownHole(Mesh *m, std::vector<int>* maps, std::vector<FloatDouble> &downContourPrcessed, Box3Float& boxD)
	{
		Transform(downContourPrcessed, GetCenter(boxD).X, GetCenter(boxD).Y);
		std::vector<Triangle> down;
		PolyTriangulator::Process(downContourPrcessed, down);
		if (IsNormalZ2(downContourPrcessed, down))
		{
			ReverseNormal(down);
		}
		for (size_t i = 0; i < down.size(); i++)
		{
			Triangle t = down[i];
			t.P0Index = maps[0][t.P0Index];
			t.P1Index = maps[0][t.P1Index];
			t.P2Index = maps[0][t.P2Index];
			m->AddFace(t);
		}
	}
	void FillUpHole(Mesh *m, std::vector<int>* maps, std::vector<FloatDouble> &upContourPrcessed, Box3Float& boxU)
	{
		Transform(upContourPrcessed,GetCenter(boxU).X,GetCenter(boxU).Y);
		std::vector<Triangle> up;
		PolyTriangulator::Process(upContourPrcessed, up);
		if (!IsNormalZ2(upContourPrcessed, up))
		{
			ReverseNormal(up);
		}
		for (size_t i = 0; i < up.size(); i++)
		{
			Triangle t = up[i];
			t.P0Index = maps[lines.size() - 1][t.P0Index];
			t.P1Index = maps[lines.size() - 1][t.P1Index];
			t.P2Index = maps[lines.size() - 1][t.P2Index];
			m->AddFace(t);
		}
	}
	void ReverseArray(std::vector<FloatDouble> &list)
	{
		size_t count = list.size();
		for (size_t i = 0; i < count / 2; ++i)
		{
			FloatDouble temp = list[count - i - 1];
			list[count - i - 1] = list[i];
			list[i] = temp;
		}
	}
public:
	Mesh *GenerateSurface()
	{
		if (lines.size() <= 1)
		{
			return NULL;
		}
		std::sort(lines.begin(), lines.end(), CompareContourline);
		for (size_t i = 0; i < lines.size(); i++)
		{
			std::vector<FloatDouble>& linepoints = lines[i]->GetPointList();
			if (0.0f > PolyTriangulator::Area(linepoints))
			{
				ReverseArray(linepoints);
			}
		}
		Mesh *m = new Mesh();
		std::vector<int>* maps=new std::vector<int>[lines.size()];
		for (size_t i = 0; i < lines.size(); i++)
		{
			maps[i].resize(lines[i]->GetLinePointCount(),-1);
		}
		std::vector<FloatDouble> upContourPrcessed;
		Box3Float boxU = lines[lines.size() - 1]->GetBox();
		std::vector<FloatDouble> downContourPrcessed;
		Box3Float boxD = lines[0]->GetBox();
		for (size_t i = 0; i < lines.size() - 1; i++)
		{
			ContourStitcher cs(lines[i],lines[i + 1]);
			if (i == 0)
			{
				downContourPrcessed.insert(downContourPrcessed.end(),cs.lineDownProcessed.begin(),cs.lineDownProcessed.end());
			}
			if (i == lines.size() - 2)
			{
				upContourPrcessed.insert(upContourPrcessed.end(),cs.lineUpProcessed.begin(),cs.lineUpProcessed.end());
			}
			Mesh *submesh = cs.DoStitching();
			size_t Z0Count = lines[i]->GetLinePointCount();
			size_t Z2Count = lines[i + 1]->GetLinePointCount();
			if (submesh->Vertices.size() != Z0Count + Z2Count)
				throw std::exception();
			for (size_t j = 0; j < Z0Count; j++)
			{
				if (maps[i][j] == -1)
				{
					maps[i][j] = m->AddVertex(submesh->Vertices[j]);
				}
			}
			for (size_t j = 0; j < Z2Count; j++)
			{
				maps[i + 1][j] = m->AddVertex(submesh->Vertices[j + Z0Count]);
			}
			for (size_t j = 0; j < submesh->Faces.size(); j++)
			{
				Triangle t = submesh->Faces[j];
				if (t.P0Index < (int)Z0Count)
				{
					t.P0Index = maps[i][t.P0Index];
				}
				else
				{
					t.P0Index = maps[i + 1][t.P0Index - Z0Count];
				}

				if (t.P1Index < (int)Z0Count)
				{
					t.P1Index = maps[i][t.P1Index];
				}
				else
				{
					t.P1Index = maps[i + 1][t.P1Index - Z0Count];
				}

				if (t.P2Index <(int)Z0Count)
				{
					t.P2Index = maps[i][t.P2Index];
				}
				else
				{
					t.P2Index = maps[i + 1][t.P2Index - Z0Count];
				}
				m->AddFace(t);
			}
			delete submesh;
		}
		//FillUpHole(m,maps,upContourPrcessed, boxU);
		//FillDownHole(m, maps, downContourPrcessed, boxD);
		delete[] maps;
		return m;
	}

};

#endif