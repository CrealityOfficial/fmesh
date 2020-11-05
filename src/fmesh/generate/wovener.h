#ifndef FMESH_WOVENER_1604559020205_H
#define FMESH_WOVENER_1604559020205_H
#include <clipper/clipper.hpp>
#include "fmesh/generate/patch.h"

namespace fmesh
{
	// paths are closer
	class Wovener
	{
	public:
		Wovener();
		~Wovener();

		Patch* woven(ClipperLib::Path* pathLower, ClipperLib::Path* pathUp);
	protected:
		void processQuad(int& lowerIndex, int& upIndex, Patch* patch);  // lowerIndex upIndex in[0 , size - 2]
		void processLast(Patch* patch);
		int findNearest();
	protected:
		ClipperLib::Path* m_pathLower;
		ClipperLib::Path* m_pathUp;
		int m_lowerStart;
		int m_upStart;
		int m_lowerSize;
		int m_upSize;
	};
}

#endif // FMESH_WOVENER_1604559020205_H