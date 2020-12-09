#ifndef FMESH_SHARPSIDEGENERATOR_1604508837329_H
#define FMESH_SHARPSIDEGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SharpsideGenerator : public GeneratorImpl
	{
	public:
		SharpsideGenerator();
		virtual ~SharpsideGenerator();

		void build() override;
	};
}

#endif // FMESH_SHARPSIDEGENERATOR_1604508837329_H