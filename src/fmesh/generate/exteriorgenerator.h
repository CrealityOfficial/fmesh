#ifndef FMESH_EXTERIORGENERATOR_1604649717545_H
#define FMESH_EXTERIORGENERATOR_1604649717545_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class ExteriorGenerator : public GeneratorImpl
	{
	public:
		ExteriorGenerator();
		virtual ~ExteriorGenerator();

		void build() override;
	};
}

#endif // FMESH_EXTERIORGENERATOR_1604649717545_H