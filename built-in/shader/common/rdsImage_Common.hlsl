#ifndef __rdsImage_Common_HLSL__
#define __rdsImage_Common_HLSL__

/*
reference:
~ OpenGL Insights Chapter 22 "Octree-Based Sparse Voxelization Using the GPU Hardware Rasterizer" [Cyril Crassin, Simon Green]
*/

float4 makeFloat4ByRBGA8(uint v)
{
    float4 o = float4(float((v & 0x000000FF))
                    , float((v & 0x0000FF00) >> 8U)
                    , float((v & 0x00FF0000) >> 16U)
                    , float((v & 0xFF000000) >> 24U));
    return clamp(o, float4(0.0, 0.0, 0.0, 0.0), float4(255.0, 255.0, 255.0, 255.0));   // float4(1.0, 1.0, 1.0, 1.0) float4(255.0, 255.0, 255.0, 255.0)
}

uint makeRBGA8ByFloat4(float4 v)
{
    return (uint(v.w) & 0x000000FF) << 24U 
         | (uint(v.z) & 0x000000FF) << 16U 
         | (uint(v.y) & 0x000000FF) << 8U 
         | (uint(v.x) & 0x000000FF);
}

/*
reference:
~ OpenGL Insights Chapter 22 "Octree-Based Sparse Voxelization Using the GPU Hardware Rasterizer" [Cyril Crassin, Simon Green]
*/
void AtomicOp_avgRGBA8(RWTexture3D<uint> dst, uint3 coords, float4 value)
{
    value.rgb *= 255.0;
    value.w   += rds_epsilon;
    uint newVal         = makeRBGA8ByFloat4(value);
    uint prevStoredVal  = 0;
    uint curStoredVal   = 0;
	int  i              = 0;

    //InterlockedExchange(dst[coords], newVal, curStoredVal);
    //return;

	const int maxIterations = 255;
    // loop as long as destination value gets changed by other threads
    //[allow_uav_condition] 
    do 
    {
        InterlockedCompareExchange(dst[coords], prevStoredVal, newVal, curStoredVal);
        if (curStoredVal == prevStoredVal)
            break;

        prevStoredVal   = curStoredVal;
        float4 rval     = makeFloat4ByRBGA8(curStoredVal);

        // denormalize
        rval.xyz = rval.xyz * rval.w;

        // add new value
        float4 curValF = rval + value;

        // renormalize
        curValF.xyz /= (curValF.w);

        newVal = makeRBGA8ByFloat4(curValF);
        ++i;
    } while (true && i < maxIterations);
}

#if 0
#pragma mark --- Image-Impl ---
#endif
#if 1

bool Image_isInBoundary(uint3 dispatchThreadId, uint3 imageExtent)
{
	return all(dispatchThreadId < imageExtent);
}

bool Image_isInBoundary(uint3 dispatchThreadId, uint3 imageExtent, uint3 imageExtentOffset)
{
	return all(imageExtentOffset + dispatchThreadId < imageExtent);
	// bool isInBoundary = imageExtentOffset.x 	+ dispatchThreadId.x < imageExtent.x
    //     				&& imageExtentOffset.y 	+ dispatchThreadId.y < imageExtent.y
    //     				&& imageExtentOffset.z 	+ dispatchThreadId.z < imageExtent.z;
	// return isInBoundary;
}

#endif



























#endif

