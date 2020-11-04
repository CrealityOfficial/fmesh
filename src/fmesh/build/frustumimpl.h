#ifndef _FRUSTUMIMPL_1604320199809_H
#define _FRUSTUMIMPL_1604320199809_H
#include "fmesh/build/buildimpl.h"

class FrustumImpl : public BuildImpl
{
public:
	FrustumImpl();
	virtual ~FrustumImpl();

	void setup(const F2MParam& param) override;
};

#endif // _FRUSTUMIMPL_1604320199809_H