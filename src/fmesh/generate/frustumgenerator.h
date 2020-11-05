#ifndef FMESH_FRUSTUMGENERATOR_1604508837329_H
#define FMESH_FRUSTUMGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class FrustumGenerator : public GeneratorImpl
	{
	public:
		FrustumGenerator();
		virtual ~FrustumGenerator();

		void build() override;
	};
}

#endif // FMESH_FRUSTUMGENERATOR_1604508837329_H