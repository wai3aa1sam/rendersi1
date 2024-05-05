#ifndef __rdsGeometry_HLSL__
#define __rdsGeometry_HLSL__

/*
    references:
    ~ https://www.3dgep.com/forward-plus/#Forward
*/

#include "rdsGeometryPrimitive.hlsl"

#if 0
#pragma mark --- Geometry-Impl ---
#endif
#if 1

Plane 
Geometry_makePlane(float3 pt0, float3 pt1, float3 pt2)
{
    Plane plane;

    float3 vec10 = pt1 - pt0;
    float3 vec20 = pt2 - pt0;

    plane.normal    = normalize(cross(vec10, vec20));
    plane.distance  = dot(plane.normal, pt0);

    return plane;
}

// check if sphere is fully behide a plane (forward is -z)
bool 
Geometry_isInside_spherePlane(Sphere sphere, Plane plane)
{
    return dot(plane.normal, sphere.center) - plane.distance < -sphere.radius; 
}

bool 
Geometry_isInside_sphereFrustum(Sphere sphere, Frustum frustum, float zNear, float zFar)
{
    bool isInside = true;
    if (sphere.center.z - sphere.radius > zNear || sphere.center.z + sphere.radius < zFar)
    {
        isInside = false;
    }

    for(uint i = 0; i < 4 && isInside; ++i)
    {
        if (Geometry_isInside_spherePlane(sphere, frustum.planes[i]))
        {
            isInside = false;
        }
    }

    return isInside;
}

bool 
Geometry_isInside_pointPlane(float3 pt, Plane plane)
{
    return dot(plane.normal, pt) - plane.distance < 0; 
}

bool 
Geometry_isInside_conePlane(Cone cone, Plane plane)
{
    // Compute the farthest point on the end of the cone to the positive space of the plane.
    float3 m = cross(cross(plane.normal, cone.direction), cone.direction);
    float3 q = cone.tip + cone.direction * cone.hieght - m * cone.radius;

    /* The cone is in the negative halfspace of the plane if both
    // the tip of the cone and the farthest point on the end of the cone to the 
    // positive halfspace of the plane are both inside the negative halfspace 
    // of the plane. 
    */
    
    return Geometry_isInside_pointPlane(cone.tip, plane) && Geometry_isInside_pointPlane(q, plane);
}

bool 
Geometry_isInside_coneFrustum(Cone cone, Frustum frustum, float zNear, float zFar)
{
    bool isInside = true;
    Plane planeNear = { float3(0, 0, -1.0), -zNear };
    Plane planeFar  = { float3(0, 0,  1.0), zFar };

    if (Geometry_isInside_conePlane(cone, planeNear) || Geometry_isInside_conePlane(cone, planeFar))
    {
        isInside = false;
    }

    for(uint i = 0; i < 4 && isInside; ++i)
    {
        if (Geometry_isInside_conePlane(cone, frustum.planes[i]))
        {
            isInside = false;
        }
    }

    return isInside;
}

#endif

#endif