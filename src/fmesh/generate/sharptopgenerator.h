#ifndef FMESH_SHARPTOPGENERATOR_1604649717545_H
#define FMESH_SHARPTOPGENERATOR_1604649717545_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SharptopGenerator : public GeneratorImpl
	{
	public:
		SharptopGenerator();
		virtual ~SharptopGenerator();

		void build() override;

		void buildShell() override;
	};
}

#endif // FMESH_SHARPTOPGENERATOR_1604649717545_H