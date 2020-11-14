#ifndef FMESH_ROOFGENERATOR_1605318431913_H
#define FMESH_ROOFGENERATOR_1605318431913_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class RoofGenerator : public GeneratorImpl
	{
	public:
		RoofGenerator();
		virtual ~RoofGenerator();

		void build() override;
	};
}

#endif // FMESH_ROOFGENERATOR_1605318431913_H