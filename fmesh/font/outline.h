#ifndef FMESH_OUTLINE_1603206058654_H
#define FMESH_OUTLINE_1603206058654_H
#include "clipperxyz/clipper.hpp"

namespace fmesh
{
	class Outline
	{
	public:
		Outline();
		~Outline();

		void push(double x, double y);
		void end();
		const ClipperLibXYZ::Paths& pathes() const;
		ClipperLibXYZ::Paths& pathes();

		int count();
	protected:
		ClipperLibXYZ::Paths m_pathes;
		ClipperLibXYZ::Path m_path;

		int m_count;
	};
}

#endif // FMESH_OUTLINE_1603206058654_H