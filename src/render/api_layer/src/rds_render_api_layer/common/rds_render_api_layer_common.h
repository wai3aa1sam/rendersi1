#pragma once

#include <rds_core.h>

#include "rds_render_api_layer-config.h"
#include "rds_render_api_layer_traits.h"

#include "rdsRenderApi_Common.h"

#include "rdsRenderDataType.h"
#include "rdsRenderResource.h"
#include "rdsRenderResource_T.h"

namespace rds
{
#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1

struct RenderAdapterInfo_Base
{
	RenderApiType apiType;

	bool	isDebug					: 1;
	bool	isMultithread			: 1;
	bool	isPresent				: 1;
	bool	isCompileShaderMode		: 1;
};

struct RenderAdapterInfo : public RenderAdapterInfo_Base
{
	String	adapterName;
	i64		memorySize = 0;

public:
	RenderAdapterInfo()
	{
	}

	void create(const RenderAdapterInfo_Base& info)
	{
		sCast<RenderAdapterInfo_Base&>(*this) = info;
	}

public:
	struct Feature
	{
		void clear()
		{

		}

		bool	isDiscreteGPU			: 1;
		bool	hasGeometryShader		: 1;
		bool	hasWireframe			: 1;
		bool	hasAccelerationStruct	: 1;
		bool	bindless				: 1;

		bool	multithread				: 1;
		bool	hasComputeShader		: 1;
		bool	shaderHasFloat64		: 1;

		bool	hasSamplerAnisotropy	: 1;

		int		minThreadGroupSize	= 1;
	};

	struct Limit
	{
		float maxSamplerAnisotropy;
	};

public:
	Feature feature;
	Limit	limit;
};

struct RenderDevice_CreateDesc
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	RenderDevice_CreateDesc();

public:
	RenderAdapterInfo_Base info;

public:
	bool isShaderCompileMode() const;
};

struct Renderer_CreateDesc : public RenderDevice_CreateDesc
{
	SizeType		gpuCount				= 1;
};

//#define RenderAttachmentLoadOp_ENUM_LIST(E) \
//	E(None, = 0) \
//	E(Load,) \
//	E(Clear,) \
//	E(DontCare,) \
//	E(_kCount,) \
////---
//RDS_ENUM_CLASS(RenderAttachmentLoadOp, u8);
//
//#define RenderAttachmentStoreOp_ENUM_LIST(E) \
//	E(None, = 0) \
//	E(Store,) \
//	E(DontCare,) \
//	E(_kCount,) \
////---
//RDS_ENUM_CLASS(RenderAttachmentStoreOp, u8);


void checkRenderThreadExclusive(RDS_DEBUG_SRCLOC_PARAM);

#endif
}
