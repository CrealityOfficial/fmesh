#ifndef CDR_LOAD_1605232819375_H
#define CDR_LOAD_1605232819375_H
#include <clipper/clipper.hpp>
#include "CDR2externalInterface.h"

namespace cdrdxf
{
	ClipperLib::Paths* loadCDRFile(const char* fileName);
}

#endif // CDR_LOAD_1605232819375_H