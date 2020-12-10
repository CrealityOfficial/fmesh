#ifndef FMESH_SLOTGENERATOR_1607603270255_H
#define FMESH_SLOTGENERATOR_1607603270255_H
#include "fmesh/generate/generatorimpl.h"

namespace fmesh
{
	class SlotGenerator : public GeneratorImpl
	{
	public:
		SlotGenerator();
		virtual ~SlotGenerator();

		void build() override;
	};
}

#endif // FMESH_SLOTGENERATOR_1607603270255_H