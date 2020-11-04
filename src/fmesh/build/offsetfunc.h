#ifndef _OFFSETFUNC_1604158403866_H
#define _OFFSETFUNC_1604158403866_H
#include "trimesh2/Vec.h"
#include <vector>

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

#endif // _OFFSETFUNC_1604158403866_H