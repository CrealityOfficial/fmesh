#ifndef FMESH_STEPSGENERATOR_1604549763432_H
#define FMESH_STEPSGENERATOR_1604549763432_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class StepsGenerator : public GeneratorImpl
	{
	public:
		StepsGenerator();
		virtual ~StepsGenerator();

		void build() override;
	};
}

#endif // FMESH_STEPSGENERATOR_1604549763432_H