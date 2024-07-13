#pragma once

/*
* reference:
* ~ https://github.com/SaschaWillems/Vulkan/blob/master/examples/shadowmappingcascade/shadowmappingcascade.cpp
* ~ cascadeSplits calculation - https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
*/

#include "rds_render/common/rds_render_common.h"
#include "rds_render/pass/rdsRenderPassFeature.h"

namespace rds
{

struct RpfCascadedShadowMapping_Result : public RenderPassFeature_Result
{
public:
	static constexpr u32 s_kCascadeLevelCount = 4;

public:
	//using DepthMaps			= Vector<RdgTextureHnd,	 s_kCascadeLevelCount>;
	using LightMatrices		= Vector<Mat4f>;
	using CascaedPlaneDists	= Vector<float,			 s_kCascadeLevelCount>;

public:
	RdgTextureHnd		shadowMap;
	LightMatrices		lightMatrices;
	CascaedPlaneDists	cascadePlaneDists;
};

struct RpfCascadedShadowMapping_Param : public RenderPassFeature_Param
{
	u32		mapSize				= 1024 * 2;
	u32		cascadedSplitCount	= 3;
	float	cascadeSplitLambda	= 0.9f;
};

#if 0
#pragma mark --- rdsRpfCascadedShadowMapping-Decl ---
#endif // 0
#if 1

class RpfCascadedShadowMapping : public RenderPassFeature
{
	RDS_RPF_COMMON_BODY(RpfCascadedShadowMapping);
public:
	static constexpr u32 s_kCascadeLevelCount = Result::s_kCascadeLevelCount;

public:
	//using DepthMaps			= Result::DepthMaps;
	using LightMatrices		= Result::LightMatrices;
	using CascaedPlaneDists	= Result::CascaedPlaneDists;
	using Materials			= Vector<SPtr<Material>, s_kCascadeLevelCount>;

public:
	static void  computeCasadedSplits(		float* dst, size_t cascadedLevelCount, float cascadeSplitLambda, const math::Camera3f camera);
	static void	 computeCascadedFrustum(	Frustum3f& o, const Mat4f& camVpInv, float splitDist, float lastSplitDist);
	static Mat4f computeLightSpaceMatrix(	CascaedPlaneDists& oDists, const math::Camera3f camera, const Mat4f& camVpInv, float splitDist, float lastSplitDist, const Vec3f& lightDir);
	static void  computeLightSpaceMatrices(	Result& oResult, const Param& param, const math::Camera3f camera, const Vec3f& lightDir, float zMultiplier = 10.0f);
	static u32	 getCascadedLevelCount(		size_t cascadedSplitCount);

public:
	Result	result;
	Param	param;

public:
	RdgPass* addCascadedShadowMappingPass(const math::Camera3f& camera, const Vec3f& lightDir);
	RdgPass* addCascadedShadowMappingPass(Materials& materials, Result* oResult, Param& param, const math::Camera3f& camera, const Vec3f& lightDir);

	RdgPass* addDebugFrustumsPass(Material* mtl_, RdgTextureHnd rtColor, const math::Camera3f& camera, const Vec3f& lightDir, bool isUpdateCamera);

protected:
	RpfCascadedShadowMapping();
	virtual ~RpfCascadedShadowMapping();

	void create();
	void destroy();

private:
	SPtr<Shader>	_shaderCsmShadowMap;
	Materials		_mtlsCsmShadowMap;
};

#endif

}