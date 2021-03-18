#include "param.h"

namespace fmesh
{
	ADParam::ADParam()
	{
		extend_width = 1.0f;
		total_height = 26.0f;
		bottom_offset = 0.0f;
		width = 50.0f;
		height = 50.0f;

		//top
		top_type = ADTopType::adtt_none;
		top_height = 2.0f;
		top_extend_width = 2.0f;

		//bottom
		bottom_type = ADBottomType::adbt_none;
		bottom_height = 2.0f;
		bottom_extend_width = 0.5f;

		//shape type
		shape_type = ADShapeType::adst_none;
		shape_bottom_height = 2.0f;
		shape_top_height = 5.0f;
		shape_middle_width = 2.0f;
		shape_angle = 15.0f;
	}

	ADParam::~ADParam()
	{

	}

	ExportParam::ExportParam()
	{
		top_offset = 0.08;
		bottom_offset = 0.08;
	}

	ExportParam::~ExportParam()
	{

	}
}