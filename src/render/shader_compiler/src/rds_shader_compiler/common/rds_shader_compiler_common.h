#pragma once

#include <rds_render.h>
#include "rds_shader_compiler-config.h"

#include "rdsShaderCompileUtil.h"

namespace rds
{


#if 0
#pragma mark --- rdsXXX-Decl ---
#endif // 0
#if 1


#endif


struct ShaderCompileOption
{
public:
	ShaderCompileOption()
		: isDebug(false), isToSpirv(true), enableLog(false), isNoOffset(true), isCompileBinary(true), isReflect(true)
	{

	}

	bool isDebug	: 1;
	bool enableLog	: 1;
	bool isToSpirv	: 1;

	bool isNoOffset : 1;

	bool isCompileBinary	: 1;
	bool isReflect			: 1;

	RenderApiType apiType = RenderApiType::Vulkan;
};


}