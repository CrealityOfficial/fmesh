#ifndef _STEPSIMPL_1604169038219_H
#define _STEPSIMPL_1604169038219_H
#include "fmesh/build/buildimpl.h"

class StepsImpl : public BuildImpl
{
public:
	StepsImpl();
	virtual ~StepsImpl();

	void setup(const F2MParam& param) override;
};

#endif // _STEPSIMPL_1604169038219_H