#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

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

#if 0
#pragma mark --- rdsCullingSettings-Decl ---
#endif // 0
#if 1

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

#endif

}