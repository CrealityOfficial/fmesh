#include "param.h"

namespace fmesh
{
	GenParam::GenParam()
		: expectLen(50.0f)
		, initH(5.0f)
		, totalH(15.0f)
		, bottomH(5.0f)
		, topH(3.0f)
		, thickness(1.0f)
		, connectDepth(1.0f)
		, exteriorH(3.0f)
	{
		bottomMode = EBottomMode::NONE;
		bottomStepH =1.5;
		bottomStepW = 0.2;
		bottomWidenW = 3.0;
		drumH = 5.0;

		italicsAngle=10;
		italicsBottomH=3.0;

		slopeRightH = 5.0;
	}

	GenParam::~GenParam()
	{

	}

	ADParam::ADParam()
	{
		extend_width = 0.8f;
		total_height = 15.0f;

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

		shape_bottom_height=2.0;
		shape_top_height=2.0;
		shape_middle_width=2.0;
		shape_angle=20;
	}

	ADParam::~ADParam()
	{

	}
}