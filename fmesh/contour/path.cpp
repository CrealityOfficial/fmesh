#include "path.h"

namespace fmesh
{
	void pathBox(const ClipperLibXYZ::Path& path, ClipperLibXYZ::IntPoint& bmin, ClipperLibXYZ::IntPoint& bmax)
	{
		bmin.X = 99999999999;
		bmin.Y = 99999999999;
		bmax.X = -99999999999;
		bmax.Y = -99999999999;
		for (const ClipperLibXYZ::IntPoint& point : path)
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

	void offsetPaths(ClipperLibXYZ::Paths& paths, ClipperLibXYZ::IntPoint offset)
	{
		for (ClipperLibXYZ::Path& path : paths)
			offsetPath(path, offset);
	}

	void offsetPath(ClipperLibXYZ::Path& path, ClipperLibXYZ::IntPoint offset)
	{
		for (ClipperLibXYZ::IntPoint& p : path)
		{
			p.X += offset.X;
			p.Y += offset.Y;
		}
	}

	void scalePath2ExpectLen(ClipperLibXYZ::Paths* paths, double expectLen) //mm
	{
		if (expectLen <= 0.0)
			return;

		ClipperLibXYZ::IntPoint bmin;
		ClipperLibXYZ::IntPoint bmax;
		calculatePathBox(paths, bmin, bmax);
		//scale
		ClipperLibXYZ::cInt val = std::max((bmax.X - bmin.X), (bmax.Y - bmin.Y));
		val = (val == 0 ? 1 : val);
		double scale = 1000.0 * expectLen / val;
		//double scaleX = 1000.0 * expectLen / xLen;
		//double scaleY = 1000.0 * expectLen / yLen;
		for (ClipperLibXYZ::Path& path : *paths)
		{
			for (ClipperLibXYZ::IntPoint& p : path)
			{
				ClipperLibXYZ::IntPoint ep;
				ep.X = (int)(scale * ((double)p.X - (double)bmin.X) + (double)bmin.X);
				ep.Y = (int)(scale * ((double)p.Y - (double)bmin.Y) + (double)bmin.Y);

				p = ep;
			}
		}
	}

	void calculatePathBox(ClipperLibXYZ::Paths* paths, ClipperLibXYZ::IntPoint& outMin, ClipperLibXYZ::IntPoint& outMax)
	{
		outMin = ClipperLibXYZ::IntPoint(99999999, 99999999, 99999999);
		outMax = ClipperLibXYZ::IntPoint(-99999999, -99999999, -99999999);
		for (ClipperLibXYZ::Path& path : *paths)
		{
			for (ClipperLibXYZ::IntPoint& p : path)
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

	void fixOrientation(std::vector<ClipperLibXYZ::Paths*>& pathses)
	{
		for (ClipperLibXYZ::Paths* paths : pathses)
			fixOrientation(paths);
	}

	void fixOrientation(ClipperLibXYZ::Paths* paths)
	{
		if (!paths)
			return;

		size_t size = paths->size();
		if (size == 0)
			return;

		struct FixInfo
		{
			ClipperLibXYZ::Path* path;
			double area;
			ClipperLibXYZ::IntPoint pMin;
			ClipperLibXYZ::IntPoint pMax;
		};

		std::vector<FixInfo*> infos(size, nullptr);
		std::list<int> candidates;
		for (size_t i = 0; i < size; ++i)
		{
			FixInfo*& info = infos.at(i);
			info = new FixInfo();

			info->path = &paths->at(i);
			info->area = ClipperLibXYZ::Area(paths->at(i));
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

			ClipperLibXYZ::IntPoint p1 = infot->path->at(0);
			return ClipperLibXYZ::PointInPolygon(p1, *infoc->path);
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