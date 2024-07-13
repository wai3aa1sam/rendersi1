#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfScreenQuad_Result : public RenderPassFeature_Result
{

};

struct RpfScreenQuad_Param : public RenderPassFeature_Param
{

};

#if 0
#pragma mark --- rdsRpfScreenQuad-Decl ---
#endif // 0
#if 1

class RpfScreenQuad : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfScreenQuad);
public:
	//Result result;

public:
	RdgPass* addDrawScreenQuadPass(SPtr<Material>& mtl, RdgTextureHnd rtColor, RdgTextureHnd image);

protected:
	RpfScreenQuad();
	virtual ~RpfScreenQuad();

	void create();
	void destroy();

private:
	static SPtr<Shader>	_shaderScreenQuad;
	//SPtr<Material>	_mtlScreenQuad;
};

#endif
}