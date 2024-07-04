#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfDisplayNormal_Result : public RenderPassFeature_Result
{
	
};

struct RpfDisplayNormal_Param : public RenderPassFeature_Param
{

};

#if 0
#pragma mark --- rdsRpfDisplayNormal-Decl ---
#endif // 0
#if 1

class RpfDisplayNormal : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfDisplayNormal);
public:
	//Result result;

public:
	RdgPass* addDisplayNormalPass(const DrawSettings& drawSettings, RdgTextureHnd rtColor);

protected:
	RpfDisplayNormal();
	virtual ~RpfDisplayNormal();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderDisplayNormal;
	SPtr<Material>	_mtlDisplayNormal;
};

#endif
}