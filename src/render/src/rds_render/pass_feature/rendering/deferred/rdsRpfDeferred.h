#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfDeferred_Result
{
	
};

#if 0
#pragma mark --- rdsRpfDeferred-Decl ---
#endif // 0
#if 1

class RpfDeferred : public RenderPassFeature
{
public:
	using Base		= RenderPassFeature;
	using Result	= RpfDeferred_Result;

public:
	Result result;

public:
	RdgPass* addGeometryPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf);

protected:
	RpfDeferred();
	virtual ~RpfDeferred();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderDeferred;
	SPtr<Material>	_mtlDeferred;
};

#endif
}