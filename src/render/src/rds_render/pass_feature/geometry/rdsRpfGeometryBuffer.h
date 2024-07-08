#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfGeometryBuffer_Result : public RenderPassFeature_Result
{
	RdgTextureHnd normal;
	RdgTextureHnd baseColor;
	RdgTextureHnd roughnessMetalness;
	RdgTextureHnd emission;

	RdgTextureHnd debugPosition;
};

struct RpfGeometryBuffer_Param : public RenderPassFeature_Param
{

};

#if 0
#pragma mark --- rdsRpfGeometryBuffer-Decl ---
#endif // 0
#if 1

class RpfGeometryBuffer : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfGeometryBuffer);
public:
	Result result;

public:
	RdgPass* addGeometryPass(Result& oResult, const DrawSettings& drawSettings, RdgTextureHnd dsBuf);
	RdgPass* addGeometryPass(const DrawSettings& drawSettings, RdgTextureHnd dsBuf);

protected:
	RpfGeometryBuffer();
	virtual ~RpfGeometryBuffer();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderGeometryBuffer;
	SPtr<Material>	_mtlGeometryBuffer;
};

#endif
}