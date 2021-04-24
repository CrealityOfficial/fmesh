#include "path.h"

namespace fmesh
{
	void pathBox(const ClipperLib::Path& path, ClipperLib::IntPoint& bmin, ClipperLib::IntPoint& bmax)
	{
		bmin.X = 99999999999;
		bmin.Y = 99999999999;
		bmax.X = -99999999999;
		bmax.Y = -99999999999;
		for (const ClipperLib::IntPoint& point : path)
		{
			if (point.X < bmin.X)
				bmin.X = point.X;
			if (point.Y < bmin.Y)
				bmin.Y = point.Y;
			if (point.X > bmax.X)
				bmax.X = point.X;
			if (point.Y > bmax.Y)
				bmax.Y = point.Y;
		}
	}

	void offsetPaths(ClipperLib::Paths& paths, ClipperLib::IntPoint offset)
	{
		for (ClipperLib::Path& path : paths)
			offsetPath(path, offset);
	}

	void offsetPath(ClipperLib::Path& path, ClipperLib::IntPoint offset)
	{
		for (ClipperLib::IntPoint& p : path)
		{
			p.X += offset.X;
			p.Y += offset.Y;
		}
	}

	void scalePath2ExpectLen(ClipperLib::Paths* paths, double expectLen) //mm
	{
		if (expectLen <= 0.0)
			return;

		ClipperLib::IntPoint bmin;
		ClipperLib::IntPoint bmax;
		calculatePathBox(paths, bmin, bmax);
		//scale
		double xLen = (double)(bmax.X - bmin.X);
		double yLen = (double)(bmax.Y - bmin.Y);
		//double scale = 1000.0 * expectLen / std::max(xLen, yLen);
		double scaleX = 1000.0 * expectLen / xLen;
		double scaleY = 1000.0 * expectLen / yLen;
		for (ClipperLib::Path& path : *paths)
		{
			for (ClipperLib::IntPoint& p : path)
			{
				ClipperLib::IntPoint ep;
				ep.X = (int)(scaleX * ((double)p.X - (double)bmin.X) + (double)bmin.X);
				ep.Y = (int)(scaleY * ((double)p.Y - (double)bmin.Y) + (double)bmin.Y);

				p = ep;
			}
		}
	}

	void calculatePathBox(ClipperLib::Paths* paths, ClipperLib::IntPoint& outMin, ClipperLib::IntPoint& outMax)
	{
		outMin = ClipperLib::IntPoint(99999999, 99999999, 99999999);
		outMax = ClipperLib::IntPoint(-99999999, -99999999, -99999999);
		for (ClipperLib::Path& path : *paths)
		{
			for (ClipperLib::IntPoint& p : path)
			{
				outMin.X = std::min(outMin.X, p.X);
				outMin.Y = std::min(outMin.Y, p.Y);
				outMin.Z = std::min(outMin.Z, p.Z);
				outMax.X = std::max(outMax.X, p.X);
				outMax.Y = std::max(outMax.Y, p.Y);
				outMax.Z = std::max(outMax.Z, p.Z);
			}
		}
	}

	void fixOrientation(std::vector<ClipperLib::Paths*>& pathses)
	{
		for (ClipperLib::Paths* paths : pathses)
			fixOrientation(paths);
	}

	void fixOrientation(ClipperLib::Paths* paths)
	{
		if (!paths)
			return;

		size_t size = paths->size();
		if (size == 0)
			return;

		struct FixInfo
		{
			ClipperLib::Path* path;
			double area;
			ClipperLib::IntPoint pMin;
			ClipperLib::IntPoint pMax;
		};

		std::vector<FixInfo*> infos(size, nullptr);
		std::list<int> candidates;
		for (size_t i = 0; i < size; ++i)
		{
			FixInfo*& info = infos.at(i);
			info = new FixInfo();

			info->path = &paths->at(i);
			info->area = ClipperLib::Area(paths->at(i));
			pathBox(paths->at(i), info->pMin, info->pMax);

			candidates.push_back((int)i);
		}

		candidates.sort([&infos](int i1, int i2)->bool {
			return infos.at(i1)->pMin.X < infos.at(i2)->pMin.X;
		});

		struct TreeNode
		{
			int index;
			int depth;
			std::list<int> temp;
			std::vector<TreeNode> children;
		};
		TreeNode rootNode;
		rootNode.index = -1;
		rootNode.depth = -1;
		rootNode.temp.swap(candidates);

		std::vector<std::vector<int>> simplePolygons(size);

		std::function<void(TreeNode& node)> split;
		std::function<void(TreeNode& node)> fix;
		std::function<bool(int c, int t)> testin;
		testin = [&infos](int c, int t) ->bool {
			FixInfo* infoc = infos.at(c);
			FixInfo* infot = infos.at(t);
			if (std::abs(infot->area) > std::abs(infoc->area))
				return false;

			if((infoc->pMax.X < infot->pMin.X) || (infoc->pMax.Y < infot->pMin.Y)
				|| (infoc->pMin.X > infot->pMax.X) || (infoc->pMin.Y > infot->pMax.Y))
				return false;
		
			if (infot->path->size() < 2)
				return false;

			ClipperLib::IntPoint p1 = infot->path->at(0);
			return ClipperLib::PointInPolygon(p1, *infoc->path);
		};

		split = [&split, &testin, &infos](TreeNode& node) {
			std::list<int>& candidates = node.temp;
			while (candidates.size() > 0)
			{
				int index = candidates.front();
				candidates.pop_front();

				TreeNode rootNode;
				rootNode.index = index;
				rootNode.depth = node.depth + 1;

				std::vector<int> added;
				for (std::list<int>::iterator it = candidates.begin(); it != candidates.end();)
				{
					std::list<int>::iterator c = it;
					++it;
					if (testin(index, *c))
					{
						bool inAdded = false;
						for (int addedIndex : added)
						{
							if (testin(addedIndex, *c))
							{
								inAdded = true;
								break;
							}
						}
						if (!inAdded)  //test *c collide in index
						{
							added.push_back(*c);
						}

						rootNode.temp.push_back(*c);
						candidates.erase(c);
					}
				}
				node.children.push_back(rootNode);
			}

			for (TreeNode& cNode : node.children)
			{
				split(cNode);
			}
		};

		fix = [&fix, &infos](TreeNode& node) {
			if (node.depth >= 0)
			{
				FixInfo* info = infos.at(node.index);
				bool needReverse = false;
				if ((node.depth % 2 == 0 && info->area > 0.0)
					|| (node.depth % 2 == 1 && info->area < 0.0))
					needReverse = true;

				if (needReverse)
					std::reverse(info->path->begin(), info->path->end());
			}
			for (TreeNode& cNode : node.children)
			{
				fix(cNode);
			}
		};


		split(rootNode);
		fix(rootNode);

		for (size_t i = 0; i < size; ++i)
			delete infos.at(i);
		infos.clear();
	}
}