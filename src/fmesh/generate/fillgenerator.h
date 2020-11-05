#ifndef FMESH_FILLGENERATOR_1604488558994_H
#define FMESH_FILLGENERATOR_1604488558994_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class FillGenerator : public GeneratorImpl
	{
	public:
		FillGenerator();
		virtual ~FillGenerator();

		void build() override;
	};
}

#endif // FMESH_FILLGENERATOR_1604488558994_H