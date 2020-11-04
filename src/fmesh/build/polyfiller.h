#ifndef _POLYFILLER_1604066553595_H
#define _POLYFILLER_1604066553595_H
#include "layer.h"

RefTriangle* fillSimplePoly(RefPoly* poly);
void fillComplexLayerNode(Layer* root, std::vector<RefTriangle*>& triangles);
void fillLayerFirstLevel(Layer* root, std::vector<RefTriangle*>& triangles);
void fillLayerAllInner(Layer* root, std::vector<RefTriangle*>& triangles);
RefTriangle* fillOneLevel(RefPoly* outerPoly, std::vector<RefPoly*>& innerPolys);

RefPoly* merge2SimpleRefPoly(RefPoly* outerPoly, std::vector<RefPoly*>& innerPolys);
#endif // _POLYFILLER_1604066553595_H