#ifndef FMESH_DRUNEDGEGENERATOR_1604508837329_H
#define FMESH_DRUNEDGEGENERATOR_1604508837329_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class DrumedgeGenerator : public GeneratorImpl
	{
	public:
		DrumedgeGenerator();
		virtual ~DrumedgeGenerator();

		void build() override;

		void buildShell() override;
	};
}

#endif // FMESH_DRUNEDGEGENERATOR_1604508837329_H