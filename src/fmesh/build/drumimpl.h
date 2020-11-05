#ifndef _DRUMIMPL_1604169038219_H
#define _DRUMIMPL_1604169038219_H
#include "fmesh/build/buildimpl.h"

class DrumImpl : public BuildImpl
{
public:
	DrumImpl();
	virtual ~DrumImpl();

	void setup(const F2MParam& param) override;
};

#endif // _DRUMIMPL_1604169038219_H