#include "specialpoly.h"
#include <algorithm>
#include <unordered_set>

namespace fmesh
{
	void sortPath(ClipperLib::Path* path, ClipperLib::Paths* paths)
	{
		//get start point
		int index = 0;
		ClipperLib::IntPoint point;

		//find first Intersection 
		std::unordered_set<ClipperLib::IntPoint> intersections;
		for (size_t i = 0; i < path->size(); i++)
		{
			if (pathCount(path, path->at(i)) > 2)
			{
				intersections.insert(path->at(i));
			}
		}

		while (path->size() > 1)
		{
			for (size_t i = 0; i < path->size(); i++)
			{
				if (std::find(intersections.begin(), intersections.end(), path->at(i)) != intersections.end())
				{
					if (i % 2 == 1)
					{
						ClipperLib::IntPoint point = path->at(i);
						path->at(i) = path->at(i - 1);
						path->at(i - 1) = point;
						index = i - 1;
					}
					else
						index = i;
					break;
				}
			}

			ClipperLib::Path _path;
			while (index >= 0)
			{
				_path.push_back(path->at(index));
				deletePoint(path, index);
				point = path->at(index);
				deletePoint(path, index);

				index = findNext(path, point);
			}
			_path.push_back(point);
			paths->push_back(_path);
			index = 0;
		}
	}

	int findNext(ClipperLib::Path* path, ClipperLib::IntPoint point)
	{
		if (path->size() < 2)
			return -1;
		for (size_t i = 0; i < path->size(); i++)
		{
			if (point == path->at(i))
			{
				if (i % 2 == 1)
				{
					ClipperLib::IntPoint point = path->at(i);
					path->at(i) = path->at(i - 1);
					path->at(i - 1) = point;
					return i - 1;
				}
				return i;
			}
		}
		return -1;
	}

	int pathCount(ClipperLib::Path* path, const ClipperLib::IntPoint& p)
	{
		int count = 0;
		for (size_t i = 0; i < path->size(); i++)
		{
			if (path->at(i) == p)
			{
				count++;
			}
		}
		return count;
	}

	bool deletePoint(ClipperLib::Path* path, int index)
	{
		if (index<0 || index > path->size())
			return false;

		path->erase(path->begin() + index);
		return true;
	}

	bool isCollision(const ClipperLib::Paths& paths, const ClipperLib::IntPoint& point, const double& smallest_dist)
	{
		for (const ClipperLib::Path& path : paths)
		{
			for (const ClipperLib::IntPoint& p : path)
			{
				double a = vSize2f(p, point);
				if (vSize2f(p, point) < smallest_dist - 0.2)
				{
					return true;
				}
			}
		}
		return false;
	}

	void generateLines(ClipperLib::Paths& originPloy, ClipperLib::Paths& newPaths, const double& laceGap, const double& laceRadius, bool isskeleton)
	{
		double smallest_dist = laceGap + laceRadius * 2;
		for (ClipperLib::Path& path : originPloy)
		{
			if (!path.size())
				return;
			float dist = 0.0f;

			ClipperLib::IntPoint p0 = path.back();
			if (isskeleton)
			{
				p0 = path.at(0);
			}

			ClipperLib::Path::iterator p1 = path.begin();

			newPaths.push_back(ClipperLib::Path());

			if (!isskeleton)
			{
				if (!isCollision(newPaths, p0, smallest_dist))
					newPaths.back().push_back(p0);
			}
			else
			{
				dist = smallest_dist / 2.0f;
			}
			while (p1 != path.end())
			{
				dist += vSize2f(p0, *p1);
				if (dist < smallest_dist)
				{
					p0 = *p1;
					p1++;
				}
				else
				{
					double p0p1dist = vSize2f(p0, *p1);
					double newDist = smallest_dist - (dist - p0p1dist);
					double ratio = newDist * 1.0 / p0p1dist;

					ClipperLib::IntPoint p;
					ClipperLib::IntPoint p0p1((*p1).X - p0.X, (*p1).Y - p0.Y);
					p0p1.X *= ratio;
					p0p1.Y *= ratio;
					p.X = p0p1.X + p0.X;
					p.Y = p0p1.Y + p0.Y;

					//if (isCollision(newPaths, p, smallest_dist))
					if (isCollision(newPaths, p, laceRadius*2.0+ laceRadius/2.0))
					{
						dist = smallest_dist - 0.2;
					}
					else
					{
						newPaths.back().push_back(p);
						dist = 0;
					}

					p0 = p;
				}
			}
		}
	}

	void generateRounds(const ClipperLib::Paths& originPath, ClipperLib::Paths& newPloy, const double& laceGap, const double& laceRadius)
	{
		for (const ClipperLib::Path& path : originPath)
		{
			ClipperLib::Paths cpaths;
			for (int n = 0; n < path.size(); n++)
			{
				//float Rx = 3.0;
				float Rx = laceRadius;
				ClipperLib::Path newPath;
				for (unsigned int i = 0; i < 100; i++)//Ë³Ê±ÕëÈ¡µã
				{
					float Angle = 2 * M_PIf * (i / 100.0);
					size_t nSize = newPath.size();
					newPath.emplace_back();
					newPath[nSize].X = (path.at(n).X + Rx * cosf(Angle) * 1000);
					newPath[nSize].Y = (path.at(n).Y + Rx * sinf(Angle) * 1000);
				}
				newPloy.push_back(newPath);
			}
		}
	}
}