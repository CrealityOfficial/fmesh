#ifndef CDRDXF_WRITESVG_1609317666015_H
#define CDRDXF_WRITESVG_1609317666015_H
#include <clipper/clipper.hpp>

namespace cdrdxf
{
	void writesvg(ClipperLib::PolyTree* tree, const std::string& file);
}

#endif // CDRDXF_WRITESVG_1609317666015_H