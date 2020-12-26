#ifndef CDRDXF_LOAD_1605251140503_H
#define CDRDXF_LOAD_1605251140503_H
#include <clipper/clipper.hpp>

namespace cdrdxf
{
	ClipperLib::Paths* loadDXFFile(const char* fileName);
	ClipperLib::Paths* loadSplineFromDXFFile(const char* fileName);
}

#endif // CDRDXF_LOAD_1605251140503_H