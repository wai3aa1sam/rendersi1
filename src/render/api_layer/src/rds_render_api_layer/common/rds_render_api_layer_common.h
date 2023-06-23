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

	bool	isDebug	: 1;

	struct Feature
	{
		void clear()
		{

		}

		bool	isDiscreteGPU		: 1;
		bool	hasGeometryShader	: 1;

		bool	multithread			: 1;
		bool	hasComputeShader	: 1;
		bool	shaderHasFloat64	: 1;

		int		minThreadGroupSize	= 1;
	};
	Feature feature;
};



#endif
}
