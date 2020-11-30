#ifndef FMESH_DRUMGENERATOR_1604649717545_H
#define FMESH_DRUMGENERATOR_1604649717545_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class DrumGenerator : public GeneratorImpl
	{
	public:
		DrumGenerator();
		virtual ~DrumGenerator();

		void build() override;
	};
}

#endif // FMESH_DRUMGENERATOR_1604649717545_H