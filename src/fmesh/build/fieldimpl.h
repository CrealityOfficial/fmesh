#ifndef _FIELDIMPL_1604169038219_H
#define _FIELDIMPL_1604169038219_H
#include "fmesh/build/buildimpl.h"

class FieldImpl : public BuildImpl
{
public:
	FieldImpl();
	virtual ~FieldImpl();

	void setup(const F2MParam& param) override;
};

#endif // _FIELDIMPL_1604169038219_H