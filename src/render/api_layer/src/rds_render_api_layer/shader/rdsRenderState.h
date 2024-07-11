#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

/*
references:
- src/render/shader/RenderState.h in [https://github.com/SimpleTalkCpp/SimpleGameEngine](https://github.com/SimpleTalkCpp/SimpleGameEngine)
*/

namespace rds
{

#define RenderState_Cull_ENUM_LIST(E) \
	E(None,) \
	E(Back,) \
	E(Front,) \
	E(_kCount,) \
//----
RDS_ENUM_CLASS(RenderState_Cull, u8)

#define RenderState_DepthTestOp_ENUM_LIST(E) \
	E(Less,) \
	E(Equal,) \
	E(LessEqual,) \
	E(Greater,) \
	E(GreaterEqual,) \
	E(NotEqual,) \
	E(Always,) \
	E(Never,) \
	E(_kCount,) \
//-----
RDS_ENUM_CLASS(RenderState_DepthTestOp, u8)

#define RenderState_BlendOp_ENUM_LIST(E) \
	E(Disable,) \
	E(Add,) \
	E(Sub,) \
	E(RevSub,) \
	E(Min,) \
	E(Max,) \
	E(_kCount,) \
//-----
RDS_ENUM_CLASS(RenderState_BlendOp, u8)

#define RenderState_BlendFactor_ENUM_LIST(E) \
	E(Zero,) \
	E(One,) \
	E(SrcColor,) \
	E(DstColor,) \
	E(SrcAlpha,) \
	E(DstAlpha,) \
	E(ConstColor,) \
	E(ConstAlpha,) \
	E(OneMinusSrcColor,) \
	E(OneMinusDstColor,) \
	E(OneMinusSrcAlpha,) \
	E(OneMinusDstAlpha,) \
	E(OneMinusConstColor,) \
	E(OneMinusConstAlpha,) \
	E(SrcAlphaSaturate,) \
	E(_kCount,) \
//-----
RDS_ENUM_CLASS(RenderState_BlendFactor, u8)

#if 0
#pragma mark --- rdsRenderState-Decl ---
#endif // 0
#if 1

struct RenderState 
{
	using Cull			= RenderState_Cull;
	using DepthTestOp	= RenderState_DepthTestOp;
	using BlendOp		= RenderState_BlendOp;
	using BlendFactor	= RenderState_BlendFactor;

	//RenderPrimitiveType primitiveType	= RenderPrimitiveType::Triangle;
	bool				wireframe		= false;
	Cull				cull			= Cull::Back;

	struct DepthTest 
	{
		DepthTestOp	op			= DepthTestOp::LessEqual;
		bool		writeMask	= true;

		RDS_INLINE bool operator==(const DepthTest& r) const 
		{
			return op == r.op && writeMask == r.writeMask;
		}

		bool isEnable() const { return op != DepthTestOp::Always; }

		template<class SE>
		void onJsonIo(SE& se) 
		{
			RDS_NAMED_FIXED_IO(se, op);
			RDS_NAMED_FIXED_IO(se, writeMask);
		}
	};
	DepthTest	depthTest;

	struct BlendFunc 
	{
		BlendOp		op			= BlendOp::Disable;
		BlendFactor	srcFactor	= BlendFactor::SrcAlpha;
		BlendFactor	dstFactor	= BlendFactor::OneMinusSrcAlpha;

		void set(BlendOp op_, BlendFactor srcFactor_, BlendFactor dstFactor_) 
		{
			op			= op_;
			srcFactor	= srcFactor_;
			dstFactor	= dstFactor_;
		}

		template<class SE>
		void onJsonIo(SE& se) 
		{
			RDS_NAMED_FIXED_IO(se, op);
			RDS_NAMED_FIXED_IO(se, srcFactor);
			RDS_NAMED_FIXED_IO(se, dstFactor);
		}
	};

	struct Blend 
	{
		BlendFunc	rgb;
		BlendFunc	alpha;
		Color4f		constColor;

		Blend() 
			: constColor(1,1,1,1) 
		{}

		bool isEnable() const { return rgb.op != BlendOp::Disable || alpha.op != BlendOp::Disable; }

		template<class SE>
		void onJsonIo(SE& se) 
		{
			RDS_NAMED_FIXED_IO(se, rgb);
			RDS_NAMED_FIXED_IO(se, alpha);
			RDS_NAMED_FIXED_IO(se, constColor);
		}
	};
	Blend blend;

	template<class SE>
	void onJsonIo(SE& se) 
	{
		//RDS_NAMED_FIXED_IO(se, primitiveType);
		RDS_NAMED_FIXED_IO(se, wireframe);
		RDS_NAMED_FIXED_IO(se, cull);
		RDS_NAMED_FIXED_IO(se, depthTest);
		RDS_NAMED_FIXED_IO(se, blend);
	}
};


}


#endif