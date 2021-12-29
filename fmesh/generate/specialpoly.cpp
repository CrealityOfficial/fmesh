#include "specialpoly.h"
#include <algorithm>
#include <unordered_set>

namespace fmesh
{
	void sortPath(ClipperLib::Path* path, ClipperLib::Paths* paths, bool getPerLine)
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

				if (getPerLine == true)
				{
					if (std::find(intersections.begin(), intersections.end(), point) != intersections.end())
						break;
				}

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

	float PointTOline(ClipperLib::IntPoint const& a, ClipperLib::IntPoint const& b, ClipperLib::IntPoint const& p) {
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

	float getMinLen(const ClipperLib::Paths& paths, const ClipperLib::IntPoint point)
	{
		if (paths.size() < 1 && paths[0].size() < 2)
			return 0.0;

		float ans1 = PointTOline(paths[0][0], paths[0][1], point);
		for (ClipperLib::Path path : paths)
		{
			for (int i = 0; i < path.size() - 1; i++)
			{
				ans1 = std::min(ans1, PointTOline(path[i], path[i + 1], point));
			}
		}
		return ans1;
	}

	float optimizePaths(ClipperLib::Paths& paths, ClipperLib::Paths& pathOrigin)
	{
		std::vector<std::vector<float>> pathsValue;
		pathsValue.resize(paths.size());
		for (size_t i = 0; i < paths.size(); i++)
		{
			pathsValue[i].resize(paths[i].size());
		}

		float average = getMinLen(pathOrigin, paths[0][0]);
		for (size_t i = 0; i < paths.size(); i++)
		{
			for (size_t j = 0; j < paths[i].size(); j++)
			{
				pathsValue[i][j] = getMinLen(pathOrigin, paths[i][j]);
				average = (average + pathsValue[i][j]) / 2.0;
			}
		}

		bool needDel = false;
		for (size_t i = 0; i < paths.size(); )
		{
			if (paths[i].size() == 1)
				continue;
			needDel = false;
			for (size_t j = 0; j < paths[i].size(); j++)
			{
				if (pathsValue[i][j] < average * 1 / 2)
				{
					needDel = true;
					break;
				}
			}

			if (needDel || paths[i].size() == 1)
			{
				paths.erase(paths.begin() + i);
				pathsValue.erase(pathsValue.begin() + i);
			}
			else
				i++;
		}

		float len = 999999.99;
		for (size_t i = 0; i < pathsValue.size(); i++)
		{
			for (size_t j = 0; j < pathsValue[i].size(); j++)
			{
				len = pathsValue[i][j] > len ? len : pathsValue[i][j];
			}
		}
		return len;
	}

	aabb getAABB(ClipperLib::Path* path)
	{
		aabb _aabb;
		for (const ClipperLib::IntPoint& p : *path)
		{
			if (_aabb.pMax.X < p.X)
				_aabb.pMax.X = p.X;
			
			if (_aabb.pMmin.X > p.X)
				_aabb.pMmin.X = p.X;

			if (_aabb.pMax.Y < p.Y)
				_aabb.pMax.Y = p.Y;
			
			if (_aabb.pMmin.Y > p.Y)
				_aabb.pMmin.Y = p.Y;
		}
		return _aabb;
	}

	ClipperLib::IntPoint getAABBvalue(ClipperLib::PolyTree* poly, int flag = 0)
	{
		aabb _aabb;
		polyNodeFunc func = [&func, &_aabb, &flag](ClipperLib::PolyNode* node) {

			for (ClipperLib::PolyNode* n : node->Childs)
			{
				if (n->Contour.size() > 0)
				{
					aabb box = getAABB(&n->Contour);
					if (box.pMax.X - box.pMmin.X > _aabb.pMax.X- _aabb.pMmin.X
						|| box.pMax.Y - box.pMmin.Y > _aabb.pMax.Y - _aabb.pMmin.Y)
					{
						_aabb = box;
						return ClipperLib::IntPoint(_aabb.pMax.X - _aabb.pMmin.X, _aabb.pMax.Y - _aabb.pMmin.Y);
					}
				}
				func(n);
			}
		};

		func(poly);
		return ClipperLib::IntPoint(_aabb.pMax.X - _aabb.pMmin.X, _aabb.pMax.Y - _aabb.pMmin.Y);
	}


}

