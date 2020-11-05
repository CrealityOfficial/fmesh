#ifndef FMESH_BOTTOMGENERATOR_1604501056244_H
#define FMESH_BOTTOMGENERATOR_1604501056244_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class BottomGenerator : public GeneratorImpl
	{
	public:
		BottomGenerator();
		virtual ~BottomGenerator();

		void build() override;
	};
}

#endif // FMESH_BOTTOMGENERATOR_1604501056244_H