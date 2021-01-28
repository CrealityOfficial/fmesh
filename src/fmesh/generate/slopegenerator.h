#ifndef FMESH_SLOPEGENERATOR_1604508837329_H
#define FMESH_SLOPEGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SlopeGenerator : public GeneratorImpl
	{
	public:
		SlopeGenerator();
		virtual ~SlopeGenerator();

		void build() override;

		void buildShell() override;
	};
}

#endif // FMESH_SLOPEGENERATOR_1604508837329_H