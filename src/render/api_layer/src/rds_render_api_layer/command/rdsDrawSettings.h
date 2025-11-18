#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsCullingSettings.h"

namespace rds
{

class Material;
class Shader;

#if 0
#pragma mark --- rdsDrawSettings-Decl ---
#endif // 0
#if 1

struct DrawSettings
{
public:
	CullingSettings				cullingSetting;
	Function<void(Material*)>	setMaterialFn;
	Shader*						overrideShader = nullptr;

public:
	DrawSettings();

};

#endif

}