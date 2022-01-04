#include "outline.h"

namespace fmesh
{
	Outline::Outline()
		:m_count(0)
	{
		m_pathes.reserve(10);
	}

	Outline::~Outline()
	{

	}

	void Outline::push(double x, double y)
	{
		ClipperLibXYZ::IntPoint point((int)1000.0 * x, (int)1000.0 * y, 0);
		m_path.push_back(point);
	}

	void Outline::end()
	{
		if (m_path.size() > 0)
		{
			ClipperLibXYZ::Path path;
			m_pathes.push_back(path);
			m_pathes.back().swap(m_path);
		}

		m_count = 0;
		for (ClipperLibXYZ::Path& path : m_pathes)
			m_count += (int)path.size();
	}

	const ClipperLibXYZ::Paths& Outline::pathes() const
	{
		return m_pathes;
	}

	ClipperLibXYZ::Paths& Outline::pathes()
	{
		return m_pathes;
	}

	int Outline::count()
	{
		return m_count;
	}
}