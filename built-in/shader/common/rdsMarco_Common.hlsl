#ifndef __rdsMarco_Common_HLSL__
#define __rdsMarco_Common_HLSL__

#if 0
#pragma mark --- Marco-Impl ---
#endif
#if 1

#define RDS_CONCAT_IMPL(A, B) A ## B
#define RDS_CONCAT(A, B) RDS_CONCAT_IMPL(A, B)

#define RDS_TYPE_ITER_LIST(E) \
E(float,) \
E(float2,) \
E(float3,) \
E(float4,) \
E(uint,) \
E(uint3,) \
E(uint2,) \
E(uint4,) \
// ---

#endif

#endif