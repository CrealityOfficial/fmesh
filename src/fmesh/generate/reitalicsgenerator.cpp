#include "reitalicsgenerator.h"

namespace fmesh
{
	ReItalicsGenerator::ReItalicsGenerator()
	{

	}

	ReItalicsGenerator::~ReItalicsGenerator()
	{

	}

	void ReItalicsGenerator::build()
	{
		//test data
// 		m_adParam.top_type = ADTopType::adtt_step;
// 		m_adParam.top_height = 1.0;
// 		m_adParam.shape_top_height = 2.0;
// 		m_adParam.bottom_type = ADBottomType::adbt_close;
// 		m_adParam.bottom_height = 1.0;
// 		m_adParam.shape_bottom_height = 3.0;
		//

		//init
		float btoomStepHeight = m_adParam.shape_bottom_height;
		float shape_top_height = m_adParam.shape_top_height;
		float thickness = m_adParam.extend_width / 2.0;
		float bottomHeight = m_adParam.bottom_height;
		float topStepWiden = m_adParam.top_extend_width / 2.0;
		float btoomStepWiden = m_adParam.bottom_extend_width / 2.0;					

		//modify
		if (m_adParam.top_type == ADTopType::adtt_step)
		{
			shape_top_height = m_adParam.shape_top_height > m_adParam.top_height ? m_adParam.shape_top_height : m_adParam.top_height;
		}

		float middleHeight = m_adParam.total_height - shape_top_height;
		size_t count = middleHeight / 0.5;
		float offset = 0.5 * tan(m_adParam.shape_angle) / 4;

		std::vector<ClipperLib::PolyTree> Steppolys(1);
		size_t num =count - bottomHeight / 0.5;

		//bottom
		std::vector<ClipperLib::PolyTree> middlePolys(count + 1);
		for (int i = 0; i <= count; ++i)
		{	
			float _offset = offset * i;
			float delta = middleHeight - (float)i * 0.5;
			fmesh::offsetAndExtendPolyTree(m_poly, offset*i/2, thickness, delta, middlePolys.at(i)); 
			if (!middlePolys.at(i).ChildCount())
			{
				count = i;
			}

			if (m_adParam.bottom_type == ADBottomType::adbt_step)
			{
				if (i == num)
				{
					fmesh::offsetAndExtendPolyTree(m_poly, offset * i / 2, thickness, delta, Steppolys.at(0));
					offsetExteriorInner(Steppolys.at(0), btoomStepWiden);
					_buildFromDiffPolyTree(&Steppolys.at(0),& middlePolys.at(i));
				}
			}
		}
		for (int i = 0; i < count; ++i)
		{
			ClipperLib::PolyTree out;
			//_buildFromDiffPolyTree_drum(&middlePolys.at(i), &middlePolys.at(i + 1), 0, out);

			if (m_adParam.bottom_type == ADBottomType::adbt_step
				&& num == i)
			{
				_buildFromDiffPolyTree_drum(&Steppolys.at(0), &middlePolys.at(i + 1), 0, out);
				if (out.ChildCount() > 0)
				{
					_fillPolyTreeReverseInner(&out);
				}
			}
			else if (m_adParam.bottom_type == ADBottomType::adbt_close
				&& num <= i)
			{
				_buildFromDiffPolyTree_drum(&middlePolys.at(i), &middlePolys.at(i + 1), 1, out);

				if (out.ChildCount() > 0)
				{
					_fillPolyTreeReverseInner(&out, true);
				}
			}
			else
			{
				_buildFromDiffPolyTree_drum(&middlePolys.at(i), &middlePolys.at(i + 1), 0, out);

				if (out.ChildCount() > 0)
				{
					_fillPolyTreeReverseInner(&out);
				}
			}
		}

		//bottom
		if (m_adParam.bottom_type == ADBottomType::adbt_step)
		{
// 			size_t num = count -bottomHeight / 0.5;
// 			std::vector<ClipperLib::PolyTree> bottomSteppolys(3);
// 
// 			fmesh::offsetAndExtendPolyTree(m_poly, offset * num / 2, thickness, middleHeight - ((float)num * 0.5 + 0.5), bottomSteppolys.at(0));
// 			fmesh::offsetAndExtendPolyTree(m_poly, offset * num / 2, thickness, middleHeight - ((float)num * 0.5), bottomSteppolys.at(1));
// 			fmesh::offsetAndExtendPolyTree(m_poly, offset * num / 2, thickness, middleHeight - ((float)num * 0.5), bottomSteppolys.at(2));
// 			offsetExteriorInner(bottomSteppolys.at(1), btoomStepWiden);
// 			_buildFromDiffPolyTree(&bottomSteppolys.at(0), &bottomSteppolys.at(1));
// 			_buildFromDiffPolyTree(&bottomSteppolys.at(1), &bottomSteppolys.at(2));
		}
		else if(m_adParam.bottom_type == ADBottomType::adbt_close)
		{
// 			size_t num1 = count;
// 			size_t num2 = count - bottomHeight / 0.5;
// 			std::vector<ClipperLib::PolyTree> bottomSteppolys(2);
// 			ClipperLib::PolyTree closeBottom;
// 			double hCloseBottom = m_adParam.bottom_offset;

// 			offsetAndExtendPolyTree(m_poly, offset * num1 / 2, thickness, middleHeight - ((float)num1 * 0.5), bottomSteppolys.at(0));
// 			offsetAndExtendPolyTree(m_poly, offset * num2 / 2, thickness, middleHeight - ((float)num2 * 0.5), bottomSteppolys.at(1));

			_fillPolyTreeDepth14(&middlePolys.back(), true);
			if (num< middlePolys.size())
			{
				_fillPolyTreeInner(&middlePolys.at(num));
			}
			//_buildFromDiffPolyTree(&bottomSteppolys.at(0), &bottomSteppolys.at(1), 3);
		}
		//bottom end 

		_fillPolyTree(&middlePolys.back(),true);

// 		ClipperLib::PolyTree treeTop;
// 		double hTop;
// 		_buildTop(treeTop, hTop);
// 		_buildFromDiffPolyTree(&middlePolys.front(), &treeTop);

		_buildTopBottom(nullptr, &middlePolys.front());
	}
}