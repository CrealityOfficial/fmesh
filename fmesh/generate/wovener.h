#ifndef FMESH_WOVENER_1604559020205_H
#define FMESH_WOVENER_1604559020205_H
#include "clipperxyz/clipper.hpp"
#include "fmesh/generate/patch.h"

namespace fmesh
{
	// paths are closer
	class Wovener
	{
	public:
		Wovener();
		~Wovener();

		Patch* woven(ClipperLibXYZ::Path* pathLower, ClipperLibXYZ::Path* pathUp);
	protected:
		void processQuad(int& lowerIndex, int& upIndex, Patch* patch);  // lowerIndex upIndex in[0 , size - 2]
		void processLast(Patch* patch);
		int findNearest();

		bool inside(std::vector<trimesh::vec3>& polygon, trimesh::vec3& p);
	protected:
		ClipperLibXYZ::Path* m_pathLower;
		ClipperLibXYZ::Path* m_pathUp;
		int m_lowerStart;
		int m_upStart;
		int m_lowerSize;
		int m_upSize;
	};
}

#endif // FMESH_WOVENER_1604559020205_H