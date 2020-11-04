#ifndef _BOTTOMIMPL_1604238184119_H
#define _BOTTOMIMPL_1604238184119_H
#include "fmesh/build/buildimpl.h"

class BottomImpl : public BuildImpl
{
public:
	BottomImpl();
	virtual ~BottomImpl();

	void setup(const F2MParam& param) override;
};

#endif // _BOTTOMIMPL_1604238184119_H