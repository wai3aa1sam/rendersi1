#pragma once

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfPbrIbl_Result
{
	SPtr<TextureCube>	cubeEnvMap;
	SPtr<TextureCube>	cubeIrradinceMap;
	SPtr<TextureCube>	cubePrefilteredMap;
	SPtr<Texture2D>		brdfLut;
};

#if 0
#pragma mark --- rdsRpfPbrIbl-Decl ---
#endif // 0
#if 1

class RpfPbrIbl : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfPbrIbl);
	using CubeMaterial = Vector<SPtr<Material>, TextureCube::s_kFaceCount >;
public:
	RdgPass* addPreparePass(Result* oResult, Texture2D* texHdrEnvMap, const RenderMesh& box, u32 cubeSize = 512, u32 irradianceCubeSize = 32);
	RdgPass* addLightingPass(const DrawSettings& drawSettings, RdgTextureHnd rtColor, RdgTextureHnd dsBuf);

protected:
	RdgPass* addBrdfLutPass(Texture2D* outBrdfLut, RdgPass* dependency);
	RdgPass* addRenderToCubePass(StrView name, TextureCube* outCube, Shader* shader, const RenderMesh& box, RdgPass* dependency, CubeMaterial& mtls, Function<void(Material*, u32, u32)> setMtlFunc);

protected:
	RpfPbrIbl();
	virtual ~RpfPbrIbl();

	void create();
	void destroy();

	bool createTexture(Result& result, StrView name, u32 cubeSize = 512, u32 irradianceCubeSize = 32);

private:
	SPtr<Shader> _shaderHdrToCube;
	SPtr<Shader> _shaderIrradianceEnvCube;
	SPtr<Shader> _shaderPrefilteredEnvCube;
	SPtr<Shader> _shaderBrdfLut;

	CubeMaterial _mtlsHdrToCube;
	CubeMaterial _mtlsIrradianceEnvCube;
	CubeMaterial _mtlsPrefilteredEnvCube;

	SPtr<Material> _mtlBrdfLut;
};

#endif
}