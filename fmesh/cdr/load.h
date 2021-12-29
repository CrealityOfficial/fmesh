#ifndef CDR_LOAD_1605232819375_H
#define CDR_LOAD_1605232819375_H
#include <clipper/clipper.hpp>
#include "fmesh/common/export.h"

namespace cdrdxf
{
	FMESH_API ClipperLib::Paths* loadCDRFile(const char* fileName);
}

#endif // CDR_LOAD_1605232819375_H