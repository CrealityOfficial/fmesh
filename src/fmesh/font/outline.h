#ifndef FMESH_OUTLINE_1603206058654_H
#define FMESH_OUTLINE_1603206058654_H
#include <clipper/clipper.hpp>

namespace fmesh
{
	class Outline
	{
	public:
		Outline();
		~Outline();

		void push(double x, double y);
		void end();
		const ClipperLib::Paths& pathes() const;
		ClipperLib::Paths& pathes();

		int count();
	protected:
		ClipperLib::Paths m_pathes;
		ClipperLib::Path m_path;

		int m_count;
	};
}

#endif // FMESH_OUTLINE_1603206058654_H