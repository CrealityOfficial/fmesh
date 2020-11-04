#include "earpolygon.h"

bool insideTriangle(const trimesh::dvec3& va, const trimesh::dvec3& vb, const trimesh::dvec3& vc,
	const trimesh::dvec3& vp)
{
	double ax = vc.x - vb.x; double ay = vc.y - vb.y;
	double bx = va.x - vc.x; double by = va.y - vc.y;
	double cx = vb.x - va.x; double cy = vb.y - va.y;
	double apx = vp.x - va.x; double apy = vp.y - va.y;
	double bpx = vp.x - vb.x; double bpy = vp.y - vb.y;
	double cpx = vp.x - vc.x; double cpy = vp.y - vc.y;

	double aCbp = ax * bpy - ay * bpx;
	double cCap = cx * apy - cy * apx;
	double bCcp = bx * cpy - by * cpx;
	return ((aCbp > 0.0) && (bCcp > 0.0) && (cCap > 0.0));
}

bool insideTriangleEx(const trimesh::dvec3& va, const trimesh::dvec3& vb, const trimesh::dvec3& vc,
	const trimesh::dvec3& vp)
{
	double ax = vc.x - vb.x; double ay = vc.y - vb.y;
	double bx = va.x - vc.x; double by = va.y - vc.y;
	double cx = vb.x - va.x; double cy = vb.y - va.y;
	double apx = vp.x - va.x; double apy = vp.y - va.y;
	double bpx = vp.x - vb.x; double bpy = vp.y - vb.y;
	double cpx = vp.x - vc.x; double cpy = vp.y - vc.y;

	double aCbp = ax * bpy - ay * bpx;
	double cCap = cx * apy - cy * apx;
	double bCcp = bx * cpy - by * cpx;
	return ((aCbp > 0.0) && (bCcp > 0.0) && (cCap > 0.0));
}

EarPolygon::EarPolygon()
	: m_root(nullptr)
	, m_circleSize(0)
	, m_totalSize(0)
{

}

EarPolygon::~EarPolygon()
{

}

void EarPolygon::setup(RefPoly* poly)
{
	if (!poly || poly->size() < 3)
		return;

	int size = (int)poly->size();
	releaseNode();
	m_nodes.reserve(size);

	m_totalSize = size;
	m_circleSize = size;
	ENode* root = new ENode();
	m_nodes.push_back(root);

	root->prev = nullptr;
	root->next = nullptr;
	ENode* cur = root;
	ENode* prv = nullptr;
	for (int i = 0; i < size; ++i)
	{
		if (!cur)
		{
			cur = new ENode();
			m_nodes.push_back(cur);
			cur->prev = nullptr;
			cur->next = nullptr;
		}

		cur->vertex = poly->at(i);

		if (prv)
		{
			prv->next = cur;
			cur->prev = prv;
		}
		prv = cur;
		cur = cur->next;
	}

	if (prv != root)
	{
		root->prev = prv;
		prv->next = root;
	}

	m_root = root;
	m_ears.clear();
	cur = m_root;
	do
	{
		calType(cur);

		if (cur->type == 3) m_ears.push_back(cur);
		cur = cur->next;
	} while (cur != m_root);
}

RefTriangle* EarPolygon::earClipping()
{
	if (!m_root)
		return nullptr;

	RefTriangle* triangle = new RefTriangle();
	triangle->reserve(m_totalSize);
	while (earClipping(triangle));
	releaseNode();
	return triangle;
}

RefTriangle* EarPolygon::earClippingFromRefPoly(RefPoly* poly)
{
	setup(poly);
	return earClipping();
}

void EarPolygon::nodeTriangle(RefTriangle* triangle, ENode* node)
{
	triangle->push_back(node->vertex);
	triangle->push_back(node->next->vertex);
	triangle->push_back(node->prev->vertex);
}

bool EarPolygon::earClipping(RefTriangle* triangle)
{
	if (!m_root)
		return false;

	if (m_circleSize >= 3)
	{
		if (m_circleSize == 3)
		{
			nodeTriangle(triangle, m_root);
			return false;
		}
		else
		{
			{ // ear clipping
				//assert(m_ears.size() > 0);
				if (m_ears.size() == 0)
					return false;

				ENode* used = m_ears.front();
				for (ENode* vn : m_ears)
				{// find sharpest vertex
					if (vn->dot > used->dot)
					{
						used = vn;
					}
				}

				nodeTriangle(triangle, used);

				ENode* np = used->prev;
				ENode* nn = used->next;

				np->next = nn;
				nn->prev = np;

				m_ears.remove(used);

				if (used == m_root)
				{
					m_root = nn;
				}
				--m_circleSize;

				auto check = [this](ENode* node) {
					bool in = node->type == 3;
					calType(node);
					if (in && node->type != 3)
						m_ears.remove(node);
					if (!in && node->type == 3)
						m_ears.push_back(node);
				};

				check(np);
				check(nn);

				if (m_ears.size() == 0 && m_circleSize >= 3)
				{// recal  // self intersection
					m_ears.clear();
					ENode* cur = m_root;
					do
					{
						calType(cur, false);

						if (cur->type == 3) m_ears.push_back(cur);
						cur = cur->next;
					} while (cur != m_root);
				}
				return true;
			}
		}
	}

	return false;
}

void EarPolygon::calType(ENode* node, bool testContainEdge)
{
	trimesh::dvec3 o = node->vertex->v;
	trimesh::dvec3 next = node->next->vertex->v;
	trimesh::dvec3 prev = node->prev->vertex->v;
	trimesh::dvec3 onext = next - o;
	trimesh::dvec3 oprev = prev - o;

	double cvalue = crossValue(onext, oprev);
	const double EPSILON = 1e-12;
	/*		if (cvalue > -EPSILON && cvalue < EPSILON) {
				node->type = 0;
			}
			else */if (cvalue < 0.0) {
				node->type = 1;
			}
			else
			{
				node->type = 2;
			}

			if (node->type == 2)
			{// check
				//node->type = 3;
				double dvalue = dotValue(onext, oprev);
				node->dot = dvalue;

				ENode* cur = m_root;
				bool ear = true;
				do
				{
					if ((cur->vertex != node->vertex) 
						&& (cur->vertex != node->next->vertex) 
						&& (cur->vertex != node->prev->vertex))
					{
						if (testContainEdge && insideTriangle(o, next, prev, cur->vertex->v))
						{
							ear = false;
							break;
						}
						else if (!testContainEdge && insideTriangleEx(o, next, prev, cur->vertex->v))
						{
							ear = false;
							break;
						}
					}
					cur = cur->next;
				} while (cur != m_root);

				if (ear) node->type = 3;
			}
}

void EarPolygon::releaseNode()
{
	if (!m_root) return;

	for (ENode* n : m_nodes)
		delete n;
	m_nodes.clear();
	m_root = nullptr;
	m_circleSize = 0;
	m_ears.clear();
}