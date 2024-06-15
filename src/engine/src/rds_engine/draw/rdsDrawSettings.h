#pragma once

#include "rds_engine/common/rds_engine_common.h"

namespace rds
{


#define CullingMode_ENUM_LIST(E) \
	E(None, = 0) \
	\
	E(CameraFustrum,) \
	E(AABBox,) \
	\
	E(_kCount,) \
//---
RDS_ENUM_CLASS(CullingMode, u8);

struct CullingSettings
{
public:
	CullingMode mode = CullingMode::None;
	union
	{
		Frustum3f	cameraFustrum;
		AABBox3f	aabbox;
	};

public:
	CullingSettings();

	void setCameraFrustum(	const Frustum3f& v);
	void setAABBox(			const AABBox3f& v);
};

#if 0
#pragma mark --- rdsDrawSettings-Decl ---
#endif // 0
#if 1

struct DrawSettings
{
public:
	CullingSettings				cullingSetting;
	Function<void(Material*)>	setMaterialFn;

public:
	DrawSettings();

};

#endif

}