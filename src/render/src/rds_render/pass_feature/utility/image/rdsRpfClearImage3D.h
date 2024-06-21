#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfClearImage3D_Result : public RenderPassFeature_Result
{
	
};

#if 0
#pragma mark --- rdsRpfClearImage3D-Decl ---
#endif // 0
#if 1

class RpfClearImage3D : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfClearImage3D);
public:
	//Result result;

public:
	RdgPass* addClearImage3DPass(SPtr<Material>& mtl, RdgTextureHnd image);
	RdgPass* addClearImage3DPass(SPtr<Material>& mtl, RdgTextureHnd image, const Tuple3u& image_extent, const Tuple3u& image_offset, const Tuple4f& clear_value);

protected:
	RpfClearImage3D();
	virtual ~RpfClearImage3D();

	void create();
	void destroy();

private:
	static SPtr<Shader>	_shaderClearImage3D;
	//SPtr<Material>	_mtlClearImage3D;
};

#endif
}