#include "earpolygon.h"
#include "fmesh/generate/triangularization.h"
#include "fmesh/common/dvecutil.h"
#include "mmesh/trimesh/polygonstack.h"
#include "mmesh/clipper/circurlar.h"

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
		source.clear();
		poly.clear();

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
					in.read((char*)&source.at(i), sizeof(ClipperLib::IntPoint));
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
				out.write((const char*)point, sizeof(ClipperLib::IntPoint));
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

	void EarPolygon::setup(ClipperLib::PolyNode* poly)
	{
		if (!poly)
			return;
		std::vector<std::vector<int>> earspolygons;
		std::vector<trimesh::dvec2> earspoints;
		std::vector<double> earspointsZ;
		int count = 0;
		polyNodeFunc func = [&func, &earspolygons, &earspoints,&earspointsZ,&count](ClipperLib::PolyNode* node) {

			std::vector<int> polygon;
			std::vector<trimesh::dvec2> points;
			for (ClipperLib::IntPoint& point : node->Contour)
			{
				polygon.push_back(earspoints.size());
				earspoints.push_back(trimesh::dvec2(point.X/1000.0, point.Y / 1000.0));
				earspointsZ.push_back(point.Z / 1000.0);
			}
			earspolygons.push_back(polygon);
		};
		mmesh::loopPolyTree(func, poly);
		m_earspolygons.swap(earspolygons);
		m_earspoints.swap(earspoints);
		m_earspointsZ.swap(earspointsZ);
		earspolygons.clear();
		earspoints.clear();
		earspointsZ.clear();
	}

	Patch* EarPolygon::earClipping()
	{
		if (!m_root)
			return nullptr;

		Patch* triangle = new Patch();
		triangle->reserve(m_totalSize * 3);
		while (earClipping(triangle));
		releaseNode();
	}

	Patch* EarPolygon::earClippingNewType()
	{
		Patch* triangle = new Patch();
		m_stack.clear();
		m_tris.clear();
		m_stack.prepare(m_earspolygons, m_earspoints);
		trimesh::TriMesh::Face f;
		while (m_stack.earClipping(f))
		{
			triangle->push_back(trimesh::vec3(m_earspoints.at(f.x).x, m_earspoints.at(f.x).y, m_earspointsZ.at(f.x)));
			triangle->push_back(trimesh::vec3(m_earspoints.at(f.y).x, m_earspoints.at(f.y).y, m_earspointsZ.at(f.y)));
			triangle->push_back(trimesh::vec3(m_earspoints.at(f.z).x, m_earspoints.at(f.z).y, m_earspointsZ.at(f.z)));
		}
		return triangle;
	}

	Patch* EarPolygon::earClippingFromRefPoly(SimplePoly* poly)
	{
		setup(poly);
		return earClipping();
	}

	Patch* EarPolygon::earClippingFromRefPoly(ClipperLib::PolyNode* poly)
	{
		setup(poly);
		return earClippingNewType();
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