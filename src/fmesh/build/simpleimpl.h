#ifndef _SIMPLEIMPL_1603805693585_H
#define _SIMPLEIMPL_1603805693585_H
#include "fmesh/build/buildimpl.h"

class SimpleImpl : public BuildImpl
{
public:
	SimpleImpl();
	virtual ~SimpleImpl();

	void setup(const F2MParam& param) override;
};
#endif // _SIMPLEIMPL_1603805693585_H