#ifndef _TRIBUILDER_1603904850799_H
#define _TRIBUILDER_1603904850799_H
#include "fmesh/build/layer.h"

void buildFromRefPolies(std::vector<RefPoly*>& ref1, std::vector<RefPoly*>& ref2, RefTriangle& refTri);
void buildFromRefPoly(RefPoly* ref1, RefPoly* ref2, RefTriangle& refTri);
void buildFromRefPoly_alg(RefPoly* ref1, RefPoly* ref2, RefTriangle& refTri);
#endif // _TRIBUILDER_1603904850799_H