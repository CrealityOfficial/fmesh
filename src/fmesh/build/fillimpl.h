#ifndef _FILLIMPL_1604153791360_H
#define _FILLIMPL_1604153791360_H

#include "fmesh/build/buildimpl.h"

class FillImpl : public BuildImpl
{
public:
	FillImpl();
	virtual ~FillImpl();

	void setup(const F2MParam& param) override;
};

#endif // _FILLIMPL_1604153791360_H