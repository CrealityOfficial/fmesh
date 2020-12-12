#include "earpolygon.h"
#include "fmesh/generate/triangularization.h"
#include "fmesh/common/dvecutil.h"

#include <fstream>
namespace fmesh
{
	SimplePolyWrapper::SimplePolyWrapper()
	{

	}

	SimplePolyWrapper::~SimplePolyWrapper()
	{

	}

	void SimplePolyWrapper::load(const std::string& file)
	{
		std::fstream in(file, std::ios::binary | std::ios::in);
		if (in.is_open())
		{
			int size = 0;
			in.read((char*)&size, sizeof(int));
			if (size > 0)
			{
				source.resize(size);
				poly.resize(size);
				for (int i = 0; i < size; ++i)
				{
					in.read((char*)&source, sizeof(ClipperLib::IntPoint));
					poly.at(i) = &source.at(i);
				}
			}
		}
		in.close();
	}

	void saveSimplePoly(const SimplePoly& poly, const std::string& file)
	{
		std::fstream out(file, std::ios::binary | std::ios::out);
		if (out.is_open())
		{
			int size = (int)poly.size();
			out.write((const char*)&size, sizeof(int));
			for (ClipperLib::IntPoint* point : poly)
			{
				out.write((const char*)&point, sizeof(ClipperLib::IntPoint));
			}
		}
		out.close();
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

	void EarPolygon::setup(SimplePoly* poly)
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

	void EarPolygon::setup(ClipperLib::Path* path)
	{
		if (!path || path->size() < 3)
			return;

		int size = (int)path->size();
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

			cur->vertex = &path->at(i);

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

	Patch* EarPolygon::earClipping()
	{
		if (!m_root)
			return nullptr;

		Patch* triangle = new Patch();
		triangle->reserve(m_totalSize * 3);
		while (earClipping(triangle));
		releaseNode();
		return triangle;
	}

	Patch* EarPolygon::earClippingFromRefPoly(SimplePoly* poly)
	{
		setup(poly);
		return earClipping();
	}

	Patch* EarPolygon::earClippingFromPath(ClipperLib::Path* path)
	{
		setup(path);
		return earClipping();
	}

	void EarPolygon::nodeTriangle(Patch* triangle, ENode* node)
	{
		triangle->push_back(CInt2V(*node->vertex));
		triangle->push_back(CInt2V(*node->next->vertex));
		triangle->push_back(CInt2V(*node->prev->vertex));
	}

	bool EarPolygon::earClipping(Patch* triangle)
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
		trimesh::dvec3 o = CInt2VD(*node->vertex);
		trimesh::dvec3 next = CInt2VD(*node->next->vertex);
		trimesh::dvec3 prev = CInt2VD(*node->prev->vertex);
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
							if (testContainEdge && insideTriangle(o, next, prev, CInt2VD(*cur->vertex)))
							{
								ear = false;
								break;
							}
							else if (!testContainEdge && insideTriangleEx(o, next, prev, CInt2VD(*cur->vertex)))
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
}