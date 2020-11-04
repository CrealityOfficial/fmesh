#ifndef _SLOPEIMPL_1603982393365_H
#define _SLOPEIMPL_1603982393365_H
#include "fmesh/build/buildimpl.h"

class SlopeImpl : public BuildImpl
{
public:
	SlopeImpl();
	virtual ~SlopeImpl();

	void setup(const F2MParam& param) override;
};
#endif // _SLOPEIMPL_1603982393365_H