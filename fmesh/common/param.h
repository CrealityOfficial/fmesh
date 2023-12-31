#ifndef FMESH_PARAM_1604458181746_H
#define FMESH_PARAM_1604458181746_H
#include <string>
#include <vector>
#include "fmesh/common/export.h"
#include "trimesh2/Vec.h"

namespace fmesh
{
	typedef std::vector<std::string> Args;

	enum class ADTopType
	{
		adtt_none,
		adtt_step,
		adtt_close,
		adtt_round
	};

	enum class ADBottomType
	{
		adbt_none,
		adbt_close,
		adbt_extend_outter,
		adbt_extend_inner,
		adbt_step
	};

	enum class ADShapeType
	{
		adst_none,
		adst_xiebian_top,
		adst_xiebian_bottom,
		adst_gubian,
		adst_cemianjianjiao,
		adst_yuanding,
		adst_jianjiao,
		adst_xiemian_front,
		adst_xiemian_back,
		adst_dingmianjieti,
		adst_shuangc,
		adst_shuangjianjiao,
		adst_baobianzi_inner,
		adst_baobianzi_outer,
		adst_nihongdeng,
		adst_chongkong,
		//adst_loukong,
		adst_qiantao,
		adst_dengpao,
		adst_num
	};

	enum class StepParamType
	{
		eHeight,
		eWide
	};

	class FMESH_API ExportParam
	{
	public:
		ExportParam();
		~ExportParam();

		float top_offset;
		float bottom_offset;
		float top_offset_other;
		float bottom_offset_other;
	};

	class FMESH_API ADParam
	{
	public:
		ADParam();
		~ADParam();

		//global
		float extend_width;
		float total_height;
		float bottom_offset;

		//top
		ADTopType top_type;
		float top_height;
		float top_extend_width;

		//bottom
		ADBottomType bottom_type;
		float bottom_height;
		float bottom_extend_width;

		//shape type
		ADShapeType shape_type;
		float shape_bottom_height;
		float shape_top_height;
		float shape_middle_width;
		float shape_angle;

		//top/bottom layer count
		int top_layers;
		int bottom_layers;
		float close_bottom_height;
		float close_up_height;

		float roundRadius;
		float roundGap;

		bool bluntSharpCorners;

		ExportParam exoprtParam;
	};
}

#endif // FMESH_PARAM_1604458181746_H