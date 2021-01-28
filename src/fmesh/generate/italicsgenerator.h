#ifndef FMESH_ITALICSGENERATOR_1604508837329_H
#define FMESH_ITALICSGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class ItalicsGenerator : public GeneratorImpl
	{
	public:
		ItalicsGenerator();
		virtual ~ItalicsGenerator();

		void build() override;

		void buildShell() override;
	};
}

#endif // FMESH_ITALICSGENERATOR_1604508837329_H