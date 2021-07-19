#include "param.h"

namespace fmesh
{
	ADParam::ADParam()
	{
		extend_width = 1.0f;
		total_height = 20;
		bottom_offset = 0.0f;

		//top
		top_type = ADTopType::adtt_none;
		top_height = 2.0f;
		top_extend_width = 1.0f;

		//bottom
		bottom_type = ADBottomType::adbt_none;
		bottom_height = 2.0f;
		bottom_extend_width = 1.0f;

		//shape type
		shape_type = ADShapeType::adst_none;
		shape_bottom_height = 2.0f;
		shape_top_height = 5.0f;
		shape_middle_width = 2.0f;
		shape_angle = 15.0f;

		top_layers = 1;
		bottom_layers = 1;
		bluntSharpCorners = true;
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