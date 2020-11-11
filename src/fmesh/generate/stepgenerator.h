#ifndef FMESH_STEPRGENERATOR_1604649717545_H
#define FMESH_STEPRGENERATOR_1604649717545_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class StepGenerator : public GeneratorImpl
	{
	public:
		StepGenerator();
		virtual ~StepGenerator();

		void build() override;
	};
}

#endif // FMESH_STEPRGENERATOR_1604649717545_H