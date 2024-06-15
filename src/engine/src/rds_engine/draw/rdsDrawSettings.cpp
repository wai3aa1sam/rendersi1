#include "rds_engine-pch.h"
#include "rdsDrawSettings.h"

namespace rds
{

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

#if 0
#pragma mark --- rdsDrawSettings-Impl ---
#endif // 0
#if 1

DrawSettings::DrawSettings()
{

}


#endif

}