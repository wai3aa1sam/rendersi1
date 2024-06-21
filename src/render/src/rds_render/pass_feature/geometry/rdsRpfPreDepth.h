#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfPreDepth_Result : public RenderPassFeature_Result
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
	RdgPass* addPreDepthPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf, RdgTextureHnd* oTexDepth, Color4f clearColor);

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