#include "skeleton2polytree.h"

namespace fmesh
{
	void dealSkeleton(ClipperLib::PolyTree& roofLine, ClipperLib::Paths* paths)
	{
		std::vector<ClipperLib::IntPoint> listPointF;
		std::vector<ClipperLib::IntPoint> listPointE;
		for (int i = 0; i < roofLine.Contour.size(); ++i, ++i)
		{
			listPointF.push_back(roofLine.Contour[i]);
			listPointE.push_back(roofLine.Contour[i + 1]);
		}

		removeRePoint(listPointF, listPointE);

		int count = 0;
		int optimalPos = findOptimalPos(listPointF, listPointE);
		ClipperLib::IntPoint pointF = listPointF[optimalPos];
		ClipperLib::IntPoint pointE = listPointE[optimalPos];
		std::vector<ClipperLib::IntPoint> optimalPoint;
		ClipperLib::Path path;
		while (listPointF.size() > 0)
		{
			path.push_back(pointF);
			count = findCount(listPointF, pointE);
			if (count == 0)
			{
				count = reversePoint(listPointF, listPointE, pointF, pointE);
			}
			int optPos = -1;
			int pos = -1;
			switch (count)
			{
			case 0:
				path.push_back(pointE);
				if (path.size() > 2)
					paths->push_back(path);
				path.clear();

				deletePoints(listPointF, listPointE, pointF, pointE);

				optPos = findPoint(listPointF, optimalPoint);
				if (optPos >= 0)
				{
					pointF = optimalPoint[optPos];
					pointE = listPointE[findPos(listPointF, pointF)];
					deletePoint(optimalPoint, optimalPoint[0]);
				}
				else if (listPointF.size())
				{
					int optimalPos = findOptimalPos(listPointF, listPointE);
					pointF = listPointF[optimalPos];
					pointE = listPointE[optimalPos];
				}
				break;
			case -1:
				break;
			case 1:
				deletePoints(listPointF, listPointE, pointF, pointE);

				pos = findPos(listPointF, pointE);
				if (pos >= 0)
				{
					pointF = pointE;
					pointE = listPointE[pos];
				}
				break;
			default:
				if (pointF == pointE)
				{
					deletePoints(listPointF, listPointE, pointF, pointE);
					deletePoint(optimalPoint, pointF);
				}
				pos = findPos(listPointF, pointE);
				if (pos >= 0)
				{
					pointF = pointE;
					pointE = listPointE[pos];
				}
				findOptimalPoints(listPointF, optimalPoint, pointF);
				break;
			}
		}
	}
	void removeRePoint(std::vector<skelepoint>& listPointF, std::vector<skelepoint>& listPointE)
	{
		int i = 0;
		int j = 0;
		bool bdelete = false;
		for (std::vector<ClipperLib::IntPoint>::iterator iter = listPointF.begin(); iter != listPointF.end() - 1;)
		{
			for (std::vector<ClipperLib::IntPoint>::iterator iterB = iter + 1; iterB != listPointF.end(); iterB++)
			{
				i = iter - listPointF.begin();
				j = iterB - listPointF.begin();
				if ((listPointF[i] == listPointF[j] && listPointE[i] == listPointE[j])
					|| (listPointF[i] == listPointE[j] && listPointE[i] == listPointF[j]))
				{
					bdelete = true;
					break;
				}
			}
			if (bdelete)
			{
				bdelete = false;
				int no = iter - listPointF.begin();
				listPointE.erase(listPointE.begin() + no);
				iter = listPointF.erase(iter);
			}
			else
				++iter;
		}
	}

	int reversePoint(std::vector<ClipperLib::IntPoint>& listPointF, std::vector<ClipperLib::IntPoint>& listPointE, ClipperLib::IntPoint pointF, ClipperLib::IntPoint pointE)
	{
		int count = 0;
		for (size_t i = 0; i < listPointE.size(); i++)
		{
			if (pointE == listPointE[i] && pointF != listPointF[i])
			{
				ClipperLib::IntPoint p = listPointE[i];
				listPointE[i] = listPointF[i];
				listPointF[i] = p;
				++count;
				break;
			}
		}
		return count;
	}

	int findCount(std::vector<ClipperLib::IntPoint>& listPointF, ClipperLib::IntPoint point)
	{
		int count = 0;
		if (listPointF.size() == 0)
		{
			return -1;
		}
		for (size_t i = 0; i < listPointF.size(); i++)
		{
			if (point == listPointF[i])
			{
				++count;
			}
		}
		return count;
	}

	int findPos(std::vector<ClipperLib::IntPoint>& listPointF, ClipperLib::IntPoint point)
	{
		int pos = 0;
		for (size_t i = 0; i < listPointF.size(); i++)
		{
			if (point == listPointF[i])
			{
				return i;
			}
		}
		return pos;
	}

	int findPoint(std::vector<ClipperLib::IntPoint>& listPointF, std::vector<ClipperLib::IntPoint>& optimalPoint)
	{
		int pos = -1;
		for (size_t i = 0; i < optimalPoint.size(); i++)
		{
			for (size_t j = 0; j < listPointF.size(); j++)
			{
				if (optimalPoint[i] == listPointF[j])
				{
					return i;
				}
			}
		}

		return pos;
	}

	void findOptimalPoints(std::vector<ClipperLib::IntPoint>& listPointF, std::vector<ClipperLib::IntPoint>& optimalPoint, ClipperLib::IntPoint point)
	{
		bool bOptimal = false;
		for (size_t i = 0; i < listPointF.size(); i++)
		{
			if (bOptimal)
			{
				optimalPoint.push_back(listPointF[i]);
			}
			if (point == listPointF[i])
			{
				bOptimal = true;
			}
		}
	}

	int findOptimalPos(std::vector<ClipperLib::IntPoint>& listPointF, std::vector<ClipperLib::IntPoint>& listPointE)
	{
		int pos = 0;
		for (size_t i = 0; i < listPointF.size(); i++)
		{
			if (pos != i && listPointF[pos] == listPointF[i])
			{
				ClipperLib::IntPoint p = listPointE[pos];
				listPointE[pos] = listPointF[pos];
				listPointF[pos] = p;
				break;
			}
		}
		return pos;
	}

	bool deletePoints(std::vector<ClipperLib::IntPoint>& listPointF, std::vector<ClipperLib::IntPoint>& listPointE, ClipperLib::IntPoint pointF, ClipperLib::IntPoint pointE)
	{
		for (size_t i = 0; i < listPointF.size(); i++)
		{
			if (pointF == listPointF[i]
				&& pointE == listPointE[i])
			{
				listPointF.erase(listPointF.begin() + i);
				listPointE.erase(listPointE.begin() + i);
				return true;
			}
		}
		return false;
	}

	bool deletePoint(std::vector<ClipperLib::IntPoint>& listPoint, ClipperLib::IntPoint point)
	{
		for (size_t i = 0; i < listPoint.size(); i++)
		{
			if (point == listPoint[i])
			{
				listPoint.erase(listPoint.begin() + i);
				return true;
			}
		}
		return false;
	}

	double averageLen(ClipperLib::PolyTree& source, ClipperLib::Paths& paths)
	{
		double alen = 0.0f;
		if (paths.size() == 0)
			return 0.0f;
		//get middle
		int optIndex = 0;
		for (int i = 1; i < paths.size(); i++)
		{
			if (paths[i].size() > paths[optIndex].size())
				optIndex = i;
		}

		int no = paths[optIndex].size() / 2;
		alen = (getMinLen(source, paths[optIndex][no - 1]) + getMinLen(source, paths[optIndex][no])) / 2.0f;

		//delete  path
		auto iterD = paths.begin();
		while (iterD != paths.end())
		{
			if (needDelPath(source, *iterD, alen))
			{
				iterD = paths.erase(iterD);
			}
			else
				iterD++;
		}

		bool allDel = false;
		//remove
		for (int i = 0; i < paths.size(); i++)
		{
			allDel = false;
			auto iter = paths[i].begin();
			while (iter != paths[i].end()) {

				if (allDel)
				{
					iter = paths[i].erase(iter);
					continue;
				}
				if (getMinLen(source, *iter) < (2 * alen / 3.0)
					&& isEnds(source, paths[i], iter, alen, allDel))
				{
					if (isIntersect(paths, *iter))
					{
						++iter;
					}
					else
					{
						iter = paths[i].erase(iter);
					}
				}
				else {
					allDel = false;
					++iter;
				}
			}
		}
		auto iter = paths.begin();
		double value = 999999.9f;
		while (iter != paths.end()) {
			if (iter->size() <= 1) {
				iter = paths.erase(iter);
			}
			else {
				for (int i = 0; i < (*iter).size(); i++)
				{
					value = getMinLen(source, (*iter)[i]);
					alen = std::min(alen, value);
				}
				++iter;
			}
		}

		return alen;
	}

	double PointTOline(ClipperLib::IntPoint const& a, ClipperLib::IntPoint const& b, ClipperLib::IntPoint const& p) {
		double ap_ab = (b.X - a.X) * (p.X - a.X) + (b.Y - a.Y) * (p.Y - a.Y);//cross( a , p , b );
		if (ap_ab <= 0)
			return sqrt((p.X - a.X) * (p.X - a.X) + (p.Y - a.Y) * (p.Y - a.Y));

		double d2 = (b.X - a.X) * (b.X - a.X) + (b.Y - a.Y) * (b.Y - a.Y);
		if (ap_ab >= d2) return sqrt((p.X - b.X) * (p.X - b.X) + (p.Y - b.Y) * (p.Y - b.Y));

		double r = ap_ab / d2;
		double px = a.X + (b.X - a.X) * r;
		double py = a.Y + (b.Y - a.Y) * r;
		return sqrt((p.X - px) * (p.X - px) + (p.Y - py) * (p.Y - py));
	}

	double getMinLen(ClipperLib::PolyTree& source, ClipperLib::IntPoint point)
	{
		bool bInit = true;
		double min = -1.0f;
		double value = 0.0f;
		for (ClipperLib::PolyNode* n : source.Childs)
		{	//inside ploygon
			if (0 != ClipperLib::PointInPolygon(point, n->Contour))
			{
				for (int k = 0; k < n->Contour.size() - 1; k++)
				{
					value = PointTOline(n->Contour[k], n->Contour[k + 1], point);
					if (bInit)
					{
						bInit = false;
						min = value;
					}
					else
					{
						min = std::min(min, value);
					}		
				}
			}
		}
		return min;
	}

	bool isIntersect(ClipperLib::Paths& paths, ClipperLib::IntPoint point)
	{
		size_t count = 0;
		for (int i = 0; i < paths.size(); i++)
		{
			for (int j = 0; j < paths[i].size(); j++)
			{
				if (paths[i][j] == point)
				{
					if (++count > 1)
						return true;
				}
			}
		}
		return false;
	}

	bool isEnds(ClipperLib::PolyTree& source, std::vector<ClipperLib::IntPoint>& path, std::vector<ClipperLib::IntPoint>::iterator iter, double alen, bool& allDel)
	{
		if (iter == path.begin())
		{
			return true;
		}
		auto _iter = iter;
		while (_iter != path.end()) {
			if (getMinLen(source, *_iter) >= (5 * alen / 5.0))
			{
				allDel = true;
				return false;
			}

			_iter++;
		}
		return true;
	}

	bool needDelPath(ClipperLib::PolyTree& source, ClipperLib::Path& path, double alen)
	{
		for (int i = 0; i < path.size(); i++)
		{
			if (getMinLen(source, path[i]) >= (5 * alen / 5.0))
				return false;
		}
		return true;
	}

}