#include "rds_render-pch.h"
#include "rdsCullingSettings.h"

namespace rds
{

#if 0
#pragma mark --- rdsCullingSettings-Impl ---
#endif // 0
#if 1

void 
CullingSettings::setCameraFrustum(const Frustum3f& v)
{
	mode			= CullingMode::CameraFustrum;
	cameraFustrum	= v;
}

void 
CullingSettings::setAABBox(const AABBox3f& v)
{
	mode			= CullingMode::AABBox;
	aabbox			= v;
}

CullingSettings::CullingSettings()
{

}

#endif

}