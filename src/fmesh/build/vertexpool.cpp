#include "vertexpool.h"
#include <assert.h>

std::vector<RefVertex> vertexPool;
int startIndex;
void buildAllocate()
{
	vertexPool.resize(5000000);
	startIndex = 0;
}

RefVertex* allocate(int size)
{
	if (size <= 0)
		return nullptr;

	if (size + startIndex > (int)vertexPool.size())
	{
		assert("overflow." && false);
		return nullptr;
	}

	RefVertex* ref = &vertexPool.at(startIndex);
	startIndex += size;
	return ref;
}

void buildDeallocate()
{
	vertexPool.clear();
	startIndex = 0;
}

void buildRefPoly(ClipperLib::Path& path, RefPoly* poly, const trimesh::dvec3& offset)
{
	int size = path.size();
	if (size > 0)
	{
		poly->clear();
		RefVertex* vertex = allocate(size);
		for (int i = 0; i < size; ++i)
		{
			ClipperLib::IntPoint& p = path.at(i);
			vertex->v = trimesh::dvec3(INT2MM(p.X), INT2MM(p.Y), 0.0) + offset;
			poly->push_back(vertex);
			++vertex;
		}
	}
}

void offsetRefPoly(RefPoly* poly, RefPoly* ref, offsetFunc func)
{
	size_t size = ref->size();

	if (poly->size() < size)
	{
		poly->clear();
		RefVertex* vertex = allocate(size);
		for (int i = 0; i < size; ++i)
		{
			poly->push_back(vertex);
			++vertex;
		}
	}

	for (size_t i = 0; i < size; ++i)
	{
		trimesh::dvec3& v = ref->at(i)->v;
		poly->at(i)->v = func(v);
	}
}
