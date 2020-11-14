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
		bottomStepH = 3.0;
		bottomStepW = 0.6;
		bottomWidenW = 3.0;
		drumH = 5.0;
	}

	GenParam::~GenParam()
	{

	}
}