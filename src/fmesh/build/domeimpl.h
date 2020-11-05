#ifndef _DOMEIMPL_1604169038219_H
#define _DOMEIMPL_1604169038219_H
#include "fmesh/build/buildimpl.h"

class DomeImpl : public BuildImpl
{
public:
	DomeImpl();
	virtual ~DomeImpl();

	void setup(const F2MParam& param) override;
};

#endif // _DOMEIMPL_1604169038219_H