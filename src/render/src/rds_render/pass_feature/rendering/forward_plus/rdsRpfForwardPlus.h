#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfForwardPlus_Result
{

};

#if 0
#pragma mark --- rdsRpfForwardPlus-Decl ---
#endif // 0
#if 1


class RpfForwardPlus : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfForwardPlus);
public:
	Result result;

public:
	RdgPass* addGeometryPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf);

protected:
	RpfForwardPlus();
	virtual ~RpfForwardPlus();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderFwdp;
	SPtr<Material>	_mtlFwdp;
};


#endif

}