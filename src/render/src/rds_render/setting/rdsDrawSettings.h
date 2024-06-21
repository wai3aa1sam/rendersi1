#pragma once

//#include "rds_engine/common/rds_engine_common.h"
#include "rds_render/common/rds_render_common.h"
#include "rdsCullingSettings.h"

namespace rds
{

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