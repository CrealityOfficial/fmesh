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
		float shape_bottom_height = m_adParam.shape_bottom_height;
		float shape_top_height = m_adParam.shape_top_height;
		float shape_middle_width = m_adParam.shape_middle_width;
		float thickness = m_adParam.extend_width / 4.0;

		float middleHeight = m_adParam.total_height - shape_bottom_height - shape_top_height;

		size_t drumHCount = 32;
		double drumDelta = middleHeight / (double)drumHCount;
		std::vector<ClipperLib::PolyTree> middlePolys(1 + drumHCount);

		float offset = 3.1415926 / drumHCount;

		size_t middle = drumHCount / 2;

		std::vector<float> offs(drumHCount + 1);
		for (size_t i = 0; i < drumHCount + 1; i++)
		{
			float delta = shape_bottom_height + i * drumDelta;
			float _offset = shape_middle_width * sin((offset * i) > 0 ? offset * i : 0);
			offsetAndExtendPolyTree(m_poly, _offset / 2, thickness, delta, middlePolys.at(i));
			offs.at(i) = _offset;
		}

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
}