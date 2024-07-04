#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfDeferredRendering_Result : public RenderPassFeature_Result
{
	
};

struct RpfDeferredRendering_Param : public RenderPassFeature_Param
{

};

#if 0
#pragma mark --- rdsRpfDeferredRendering-Decl ---
#endif // 0
#if 1

class RpfDeferredRendering : public RenderPassFeature
{
public:
	using Base		= RenderPassFeature;
	using Result	= RpfDeferredRendering_Result;

public:
	Result result;

public:
	RdgPass* addGeometryPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf);

protected:
	RpfDeferredRendering();
	virtual ~RpfDeferredRendering();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderDeferredRendering;
	SPtr<Material>	_mtlDeferredRendering;
};

#endif
}