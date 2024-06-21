#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfClearImage2D_Result : public RenderPassFeature_Result
{

};

#if 0
#pragma mark --- rdsRpfClearImage2D-Decl ---
#endif // 0
#if 1

class RpfClearImage2D : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfClearImage2D);
public:
	//Result result;

public:
	RdgPass* addClearImage2DPass(SPtr<Material>& mtl, RdgTextureHnd image, const Tuple2u& image_extent, const Tuple2u& image_offset, const Tuple4f& clear_value);
	RdgPass* addClearImage2DPass(SPtr<Material>& mtl, RdgTextureHnd image);

protected:
	RpfClearImage2D();
	virtual ~RpfClearImage2D();

	void create();
	void destroy();

private:
	static SPtr<Shader>	_shaderClearImage2D;
	//SPtr<Material>	_mtlClearImage2D;
};

#endif
}