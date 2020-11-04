#ifndef _VERTEXPOOL_1603894789079_H
#define _VERTEXPOOL_1603894789079_H
#include "fmesh/build/layer.h"

void buildAllocate();
RefVertex* allocate(int size);
void buildDeallocate();

void buildRefPoly(ClipperLib::Path& path, RefPoly* poly, const trimesh::dvec3& offset);
void offsetRefPoly(RefPoly* poly, RefPoly* ref, offsetFunc func);
#endif // _VERTEXPOOL_1603894789079_H