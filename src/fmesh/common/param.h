#ifndef FMESH_PARAM_1604458181746_H
#define FMESH_PARAM_1604458181746_H
#include <string>
#include <vector>

#include "trimesh2/Vec.h"

namespace fmesh
{
	typedef std::vector<std::string> Args;

	struct F2MParam
	{
		float expectLen;
		trimesh::vec2 dmin;
		trimesh::vec2 dmax;
		std::vector<std::string> args;

		float initH;
		float totalH;
		float bottomH;
		float thickness;
	};

	enum class EBottomMode
	{
		STEP,
		WIDEN,
		CLOSED,
		NONE
	};

	class GenParam
	{
	public:
		GenParam();
		~GenParam();

		float expectLen;
		float initH;
		float totalH;
		float bottomH;
		float thickness;
		float topH;

		//steps
		float connectDepth;

		//bottom surface
		EBottomMode bottomMode;
		float bottomStepH;
		float bottomStepW;
		float bottomWidenW;

	};
}

#endif // FMESH_PARAM_1604458181746_H