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
    Plane planes[4];

    /*
    Plane[0] topPlane;
    Plane[1] bottomPlane;
    Plane[2] leftPlane;
    Plane[3] rightPlane;
    */
};

struct DebugFrustrumPts
{
    float4 pts[4];
};

struct Cone
{
    float3  tip;
    float   hieght;
    float3  direction;      // center to tip
    float   radius;
};

#endif

#endif