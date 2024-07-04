#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfShadowMapping_Result : public RenderPassFeature_Result
{
	RdgTextureHnd depthMap;
	RdgTextureHnd depthTex;
};

struct RpfShadowMapping_Param : public RenderPassFeature_Param
{
	u32 mapSize = 1024;
};

#if 0
#pragma mark --- rdsRpfShadowMapping-Decl ---
#endif // 0
#if 1

class RpfShadowMapping : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfShadowMapping);
public:
	Result	result;
	Param	param;

public:
	RdgPass* addShadowMappingPass(const Vec3f& lightDir);
	RdgPass* addShadowMappingPass(SPtr<Material>& mtl, Result* oResult, const Param& param, const Vec3f& lightPos);

protected:
	RpfShadowMapping();
	virtual ~RpfShadowMapping();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderShadowMap;
	SPtr<Material>	_mtlShadowMap;
};

#endif
}