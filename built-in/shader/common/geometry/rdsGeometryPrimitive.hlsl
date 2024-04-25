#ifndef __rdsGeometryPrimitive_HLSL__
#define __rdsGeometryPrimitive_HLSL__

#if 0
#pragma mark --- GeometryPrimitive-Impl ---
#endif
#if 1

struct Plane
{
    float3 normal;
    float  distance;
};

struct Sphere
{
    float3 center;
    float  radius;
};

// near and far is computed by depth value
struct Frustum
{
    Plane topPlane;
    Plane bottomPlane;
    Plane leftPlane;
    Plane rightPlane;
};

struct DebugFrustrumPts
{
    float4 pts[4];
};

#endif

#endif