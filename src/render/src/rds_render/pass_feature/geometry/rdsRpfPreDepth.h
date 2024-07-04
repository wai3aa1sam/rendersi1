#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfPreDepth_Result : public RenderPassFeature_Result
{

};

struct RpfPreDepth_Param : public RenderPassFeature_Param
{

};

#if 0
#pragma mark --- rdsRpfPreDepth-Decl ---
#endif // 0
#if 1

class RpfPreDepth : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfPreDepth);
public:
	//Result result;

public:
	RdgPass* addPreDepthPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf, RdgTextureHnd* oTexDepth, Color4f clearColor = Color4f{0.0f, 0.0f, 0.0f, 0.0f});

protected:
	RpfPreDepth();
	virtual ~RpfPreDepth();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderPreDepth;
	SPtr<Material>	_mtlPreDepth;
};

#endif
}