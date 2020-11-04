#include "fillimpl.h"
#include "polyfiller.h"

FillImpl::FillImpl()
{

}

FillImpl::~FillImpl()
{

}

void FillImpl::setup(const F2MParam& param)
{
	std::string arg = "simple";
	if (param.args.size() > 0)
		arg = param.args.at(0);

	if (arg == "simple")
	{
		Layer* layer = createLayer(param.thickness / 2.0f);

		fillLayer(layer);
	}
}