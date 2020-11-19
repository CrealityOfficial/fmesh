#include "drumgenerator.h"
#include "fmesh/contour/contour.h"
#include "fmesh/roof/roof.h"

namespace fmesh
{
	DrumGenerator::DrumGenerator()
	{

	}

	DrumGenerator::~DrumGenerator()
	{

	}

	void DrumGenerator::build()
	{
		double thickness = m_param.thickness / 2.0f;
		double offset = thickness;
		std::vector<float> heights(2);
		heights.at(0) = 0.0f;
		heights.at(1) = m_param.totalH-m_param.drumH;

		size_t drumHCount = m_param.drumH/0.5;
		std::vector<ClipperLib::PolyTree> polys(2+ drumHCount);

		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(0), polys.at(0));
		offsetAndExtendPolyTree(m_poly, 0.0, thickness, heights.at(1), polys.at(1));
		for (size_t i = 2; i < drumHCount+1; i++)
		{
			double delta = heights.at(1) + (i - 1) * 0.5;
			double offset = m_param.drumH - sqrt(m_param.drumH * m_param.drumH - 0.25*i*i);
			offsetAndExtendPolyTree(m_poly, -offset, thickness, delta, polys.at(i));
		}

		std::vector<Patch*> patches;
		skeletonPolyTree(polys.at(drumHCount), heights.at(1) + (drumHCount - 1) * 0.5, patches);
		//sort(patches);
		addPatches(patches);

		_fillPolyTree(&polys.at(0), true);
		//_fillPolyTree(&polys.at( drumHCount+1));

		_buildFromSamePolyTree(&polys.at(0), &polys.at(1));
		for (size_t i=1;i < polys.size()-2;i++)
		{
			_buildFromDiffPolyTree(&polys.at(i), &polys.at(i+1));
		}
	}

	void DrumGenerator::sort(std::vector<Patch*>& patches)
	{
		for (size_t num=0;num< patches.size();num++)
		{
			for (int i = 0; i < patches.at(num)->size() - 1; i++) {
				//第i趟比较
				for (int j = 0; j < patches.at(num)->size() - i - 1; j++) {
					//开始进行比较，如果arr[j]比arr[j+1]的值大，那就交换位置
					if (!cmp2(patches.at(num)->at(j), patches.at(num)->at(j+1))){
						trimesh::vec3 temp = patches.at(num)->at(j);
						patches.at(num)->at(j) = patches.at(num)->at(j+1);
						patches.at(num)->at(j+1) = temp;
					}
				}
			}
		}
	}

	float DrumGenerator::cross(float x1, float y1, float x2, float y2)//计算叉积
	{
		return (x1 * y2 - x2 * y1);
	}

	float DrumGenerator::compare(trimesh::vec3 a, trimesh::vec3 b, trimesh::vec3 c)
	{
		float f = cross((b.x - a.x), (b.y - a.y), (c.x - a.x), (c.y - a.y));
		return f;
	}

	bool DrumGenerator::cmp2(trimesh::vec3 a, trimesh::vec3 b)
	{
		trimesh::vec3 c;//原点
		c.x = 0;
		c.y = 0;
		if (compare(c, a, b) == 0)//计算叉积，函数在上面有介绍，如果叉积相等，按照X从小到大排序
			return a.x < b.x;
		else 
			return compare(c, a, b) > 0;
	}
}