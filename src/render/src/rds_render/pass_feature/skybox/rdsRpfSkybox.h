#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfSkybox_Result : public RenderPassFeature_Result
{
	
};

#if 0
#pragma mark --- rdsRpfSkybox-Decl ---
#endif // 0
#if 1

class RpfSkybox : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfSkybox);
public:
	Result result;

public:
	RdgPass* addSkybox(RdgTextureHnd rtColor);

protected:
	RpfSkybox();
	virtual ~RpfSkybox();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderSkybox;
	SPtr<Material>	_mtlSkybox;
};

#endif
}