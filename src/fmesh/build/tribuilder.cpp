#include "tribuilder.h"

void buildFromRefPolies(std::vector<RefPoly*>& ref1, std::vector<RefPoly*>& ref2, RefTriangle& refTri)
{
	size_t size = ref1.size();
	size_t tsize = ref2.size();
	if (size == tsize)
	{
		for (size_t i = 0; i < size; ++i)
			buildFromRefPoly(ref1.at(i), ref2.at(i), refTri);
	}
}

void buildFromRefPoly(RefPoly* ref1, RefPoly* ref2, RefTriangle& refTri)
{
	size_t size = ref1->size();
	if (size == ref2->size() && size > 2)
	{
		for (int i = 0; i < size; ++i)
		{
			int i1 = i;
			int i3 = i;
			int i2 = i1 + 1;
			int i4 = i3 + 1;

			if (i == size - 1)
			{
				i2 = 0;
				i4 = 0;
			}

			refTri.push_back(ref1->at(i1));
			refTri.push_back(ref1->at(i2));
			refTri.push_back(ref2->at(i4));
			refTri.push_back(ref1->at(i1));
			refTri.push_back(ref2->at(i4));
			refTri.push_back(ref2->at(i3));
		}
	}
}