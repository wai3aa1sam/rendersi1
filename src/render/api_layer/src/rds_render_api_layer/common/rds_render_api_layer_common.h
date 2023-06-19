#pragma once

#include <rds_core.h>

#include "rds_render_api_layer-config.h"
#include "rds_render_api_layer_traits.h"

#include "rdsRenderApi_Common.h"

namespace rds
{
#if 0
#pragma mark --- XXXX-Decl/Impl ---
#endif // 0
#if 1

struct RenderAdapterInfo 
{
	String	adapterName;
	i64		memorySize = 0;

	bool	isDebug				: 1;
	bool	isDiscreteGPU		: 1;
	bool	hasGeometryShader	: 1;

	bool	multithread			= false;
	bool	hasComputeShader	= false;
	bool	shaderHasFloat64	= false;
	int		minThreadGroupSize	= 1;
};



#endif
}
