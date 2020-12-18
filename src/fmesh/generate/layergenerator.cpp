#include "layergenerator.h"

namespace fmesh
{
	LayerGenerator::LayerGenerator()
		:m_tracer(nullptr)
	{

	}

	LayerGenerator::~LayerGenerator()
	{

	}

	void LayerGenerator::setLayerGeneratorTracer(LayerGeneratorTracer* tracer)
	{
		m_tracer = tracer;
	}

	void LayerGenerator::build()
	{
		size_t drumHCount = 5;
		std::vector<ClipperLib::PolyTree> middlePolys;
#if 0
		//float shape_bottom_height = m_adParam.shape_bottom_height;
		//float shape_top_height = m_adParam.shape_top_height;
		//float shape_middle_width = m_adParam.shape_middle_width;
		//float thickness = m_adParam.extend_width / 2.0;
		//
		//float middleHeight = m_adParam.total_height - shape_bottom_height - shape_top_height;
		//
		//size_t drumHCount = 32;
		//double drumDelta = middleHeight / (double)drumHCount;
		//std::vector<ClipperLib::PolyTree> middlePolys(1 + drumHCount);
		//
		//float offset = 3.1415926 / drumHCount;
		//
		//size_t middle = drumHCount / 2;
		//
		//std::vector<float> offs(drumHCount + 1);
		//for (size_t i = 0; i < drumHCount + 1; i++)
		//{
		//	float delta = shape_bottom_height + i * drumDelta;
		//	float _offset = shape_middle_width * sin((offset * i) > 0 ? offset * i : 0);
		//	offsetAndExtendPolyTree(m_poly, _offset / 2, thickness, delta, middlePolys.at(i));
		//	offs.at(i) = _offset;
		//}
#else
		double thickness = m_adParam.extend_width / 2.0;
		double bottomHeight = m_adParam.total_height - m_adParam.shape_top_height;
		drumHCount = (m_adParam.shape_top_height - 0.5) / 0.8;
		middlePolys.resize(1 + drumHCount);
		float delta = 0.0f;
		for (size_t i = 0; i < drumHCount + 1; i++)
		{
			delta = bottomHeight + i * 0.8;
			double offset = m_adParam.shape_top_height - sqrt(m_adParam.shape_top_height * m_adParam.shape_top_height - thickness * thickness * i * i);
			offsetAndExtendPolyTree(m_poly, -offset, thickness, delta, middlePolys.at(i));
		}
#endif

		int index = m_tracer ? m_tracer->index() : 0;
		if(index >= 0 && index < drumHCount - 1)
		{
			ClipperLib::PolyTree out;
			_buildFromDiffPolyTree_drum(&middlePolys.at(index), &middlePolys.at(index + 1), 0, out);
			if (out.ChildCount() > 0)
			{
				_fillPolyTreeReverseInner(&out, false);
			}

			if (m_tracer)
			{
				//m_tracer->impl(middlePolys.at(index), middlePolys.at(index + 1));
				
				ClipperLib::PolyTree tree;
				polyNodeFunc func = [&func](ClipperLib::PolyNode* node) {
					for (ClipperLib::IntPoint& point : node->Contour)
						point.Z = 0;
					for (ClipperLib::PolyNode* n : node->Childs)
						func(n);
				};

				func(&out);

				m_tracer->impl(out, tree);
			}
		}
	}

	void LayerGenerator::saveXOR(ClipperLib::PolyTree& tree, const ADParam& param)
	{
		size_t drumHCount = 32;
		float shape_middle_width = param.shape_middle_width;
		float thickness = param.extend_width / 2.0;
		std::vector<ClipperLib::PolyTree> middlePolys(1 + drumHCount);

		float offset = 3.1415926 / drumHCount;
		for (size_t i = 0; i < drumHCount + 1; i++)
		{
			float _offset = shape_middle_width * sin((offset * i) > 0 ? offset * i : 0);
			offsetAndExtendPolyTree(tree, _offset / 2, thickness, 0.0, middlePolys.at(i));
		}

		int index = 0;
		auto time_string = [&index]()->std::string {
			++index;
			char szTime[100] = { '\0' };

			time_t time2 = time(nullptr);
			tm pTm;
			localtime_s(&pTm, &time2);
			pTm.tm_year += 1900;
			pTm.tm_mon += 1;

			sprintf_s(szTime, "%04d-%02d-%02d-%02d-%02d-%02d-%d",
				pTm.tm_year,
				pTm.tm_mon,
				pTm.tm_mday,
				pTm.tm_hour,
				pTm.tm_min,
				pTm.tm_sec,
				index);

			std::string strTime = szTime;

			return strTime;
		};

		for (size_t i = 0; i < drumHCount - 1; i++)
		{
			ClipperLib::PolyTree out;
			buildXORFrom2PolyTree(&middlePolys.at(i), &middlePolys.at(i + 1), out);

			std::vector<ClipperLib::PolyNode*> source;
			std::vector<ClipperLib::PolyNode*> tmp;

			for (ClipperLib::PolyNode* node : out.Childs)
				if (!node->IsHole())
					source.push_back(node);

			int parentChilds = 0;
			while (source.size() > 0)
			{
				for (ClipperLib::PolyNode* node : source)
				{
					SimplePoly poly;
					merge2SimplePoly(node, &poly, false);
					saveSimplePoly(poly, time_string());

					for (ClipperLib::PolyNode* n : node->Childs)
					{
						for (ClipperLib::PolyNode* cn : n->Childs)
						{
							if (!cn->IsHole())
								tmp.push_back(cn);
						}
					}
				}

				source.swap(tmp);
				tmp.clear();
				++parentChilds;
			}
		}
	}
}