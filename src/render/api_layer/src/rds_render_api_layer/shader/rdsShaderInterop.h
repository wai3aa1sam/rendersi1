#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

namespace shaderInterop
{

using uint      = u32;
using float2    = Tuple2f;
using float3    = Tuple3f;
using float4    = Tuple4f;
//using float3x3  = Mat3f;
using float4x4  = Mat4f;

#include "../../../built-in/shader/interop/rdsShaderInterop.hlsl"

}

using shaderInterop::PerObjectParam;
using shaderInterop::ObjectTransform;
using shaderInterop::DrawParam;

}
