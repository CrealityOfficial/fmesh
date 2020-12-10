#ifndef FMESH_SLOPEBACKGENERATOR_1604508837329_H
#define FMESH_SLOPEBACKGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SlopebackGenerator : public GeneratorImpl
	{
	public:
		SlopebackGenerator();
		virtual ~SlopebackGenerator();

		void build() override;
	};
}

#endif // FMESH_SLOPEBACKGENERATOR_1604508837329_H