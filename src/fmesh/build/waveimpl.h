#ifndef _WAVEIMPL_1604233924104_H
#define _WAVEIMPL_1604233924104_H
#include "fmesh/build/buildimpl.h"

class WaveImpl : public BuildImpl
{
public:
	WaveImpl();
	virtual ~WaveImpl();

	void setup(const F2MParam& param) override;
};

#endif // _WAVEIMPL_1604233924104_H