#include "rds_render-pch.h"
#include "rdsRpfCascadedShadowMaps.h"

namespace rds
{

#if 0
#pragma mark --- rdsRpfCascadedShadowMaps-Impl ---
#endif // 0
#if 1

void 
RpfCascadedShadowMaps::computeCasadedSplits(float* dst, size_t cascadedLevelCount, float cascadeSplitLambda, const math::Camera3f camera)
{
	float nearClip	= camera.nearClip();
	float farClip	= camera.farClip();
	float clipRange = farClip - nearClip;

	float minZ = nearClip;
	float maxZ = nearClip + clipRange;

	float range = maxZ - minZ;
	float ratio = maxZ / minZ;

	// Calculate split depths based on view camera frustum
	for (uint32_t i = 0; i < cascadedLevelCount; i++) 
	{
		float p = (i + 1) / sCast<float>(cascadedLevelCount);
		float log = minZ * math::pow(ratio, p);
		float uniform = minZ + range * p;
		float d = cascadeSplitLambda * (log - uniform) + uniform;
		*dst = (d - nearClip) / clipRange;
		dst++;
	}
}

void 
RpfCascadedShadowMaps::computeCascadedFrustum(Frustum3f& o, const Mat4f& camVpInv, float splitDist, float lastSplitDist)
{
	Frustum3f& frustum = o;
	frustum.setByInvViewProjMatrix(camVpInv);

	for (size_t i = 0; i < Frustum3f::s_kVertexCount / 2; i++)
	{
		auto& points = frustum.points;
		Vec3f dist	= points[i + 4] - points[i] ;
		points[i]		= points[i] + (dist * lastSplitDist);
		points[i + 4]	= points[i] + (dist * splitDist);
	}
}

Mat4f
RpfCascadedShadowMaps::computeLightSpaceMatrix(CascaedPlaneDists& oDists, const math::Camera3f camera, const Mat4f& camVpInv, float splitDist, float lastSplitDist, const Vec3f& lightDir)
{
	Frustum3f frustum;
	computeCascadedFrustum(frustum, camVpInv, splitDist, lastSplitDist);

	Vec3f center = frustum.center();

	float radius = 0.0f;
	for (auto& e : frustum.points) 
	{
		float distance = e.distance(center);
		radius = math::max(radius, distance);
	}
	radius = std::ceil(radius * 16.0f) / 16.0f;

	AABBox3f aabbox;
	aabbox.reset(Vec3f::s_one() * -radius, Vec3f::s_one() * radius);

	auto lightView	= Mat4f::s_lookAt(center - lightDir * -aabbox.min.z, center, Vec3f::s_up());

	Mat4f lightProj = glm::orthoRH_ZO(aabbox.min.x, aabbox.max.x, aabbox.min.y, aabbox.max.y, 0.0f, aabbox.max.z - aabbox.min.z);		// z is -ve
	oDists.emplace_back((camera.nearClip() + splitDist * (camera.farClip() - camera.nearClip())) * -1.0f);
	
	auto lightVp = (lightProj * lightView);
	return lightVp;
}

void  
RpfCascadedShadowMaps::computeLightSpaceMatrices(Result& oResult, const Param& param, const math::Camera3f camera, const Vec3f& lightDir, float zMultiplier)
{
	auto  cascadedLevelCount	= getCascadedLevelCount(param.cascadedSplitCount);
	Mat4f camVpInv				= camera.viewProjMatrix().inverse();

	auto& oMatrices				= oResult.lightMatrices;
	auto& oCascaedPlaneDists	= oResult.cascadePlaneDists;

	oMatrices.clear();
	oMatrices.resize(cascadedLevelCount);

	oCascaedPlaneDists.clear();
	oCascaedPlaneDists.reserve(cascadedLevelCount);

	Vector<float, 16> cascadeSplits;
	cascadeSplits.resize(cascadedLevelCount);
	computeCasadedSplits(cascadeSplits.data(), cascadedLevelCount, param.cascadeSplitLambda, camera);

	float lastSplitDist = 0.0;
	for (u32 i = 0; i < cascadedLevelCount; ++i)
	{
		oMatrices[i] = computeLightSpaceMatrix(oCascaedPlaneDists, camera, camVpInv, cascadeSplits[i], lastSplitDist, lightDir);
		lastSplitDist = cascadeSplits[i];
	}
}

u32 RpfCascadedShadowMaps::getCascadedLevelCount(size_t cascadedSplitCount) { return sCast<u32>(cascadedSplitCount + 1); }

RpfCascadedShadowMaps::RpfCascadedShadowMaps()
{

}

RpfCascadedShadowMaps::~RpfCascadedShadowMaps()
{
	destroy();
}

void 
RpfCascadedShadowMaps::create()
{
	//RenderUtil::createMaterials(&_shaderCsmShadowMap, _mtlsCsmDepth, "asset/shader/pass_feature/shadow/cascaded_shadow_maps/rdsCsm_Depth.shader");
}

void 
RpfCascadedShadowMaps::destroy()
{
	RenderUtil::destroyShaderMaterials(_shaderCsmShadowMap, _mtlsCsmShadowMap);
}

RdgPass* 
RpfCascadedShadowMaps::addCascadedShadowMappingPass(Materials& materials, Result* oResult, Param& param_, const math::Camera3f& camera, const Vec3f& lightDir)
{
	auto*		rdGraph						= renderGraph();
	auto*		drawData					= drawDataBase();
	RdgPass*	passCascadedShadowMapping	= nullptr;

	Vec2u			mapSize				= Vec2u::s_one() * param_.mapSize;
	auto			cascadedLevelCount	= getCascadedLevelCount(param_.cascadedSplitCount);
	RDS_CORE_ASSERT(cascadedLevelCount <= s_kCascadeLevelCount, "only support 4 cascaded level");

	auto viewport = Rect2f{ Tuple2f::s_zero(), Tuple2f{param.mapSize, param.mapSize} };

	//oResult->depthMaps.resize(	cascadedLevelCount);
	materials.resize(			cascadedLevelCount);
	RenderUtil::createMaterials(&_shaderCsmShadowMap, materials, "asset/shader/pass_feature/shadow/cascaded_shadow_maps/rdsCsm_ShadowMap.shader");

	computeLightSpaceMatrices(*oResult, param_, camera, lightDir);

	TempString shadowMapName	= "csm_shadow_map";
	TempString passName			= "csm_depth";

	RdgTextureHnd shadowMap = rdGraph->createTexture(shadowMapName
		, Texture2DArray_CreateDesc{ mapSize, cascadedLevelCount, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	for (u32 i = 0; i < cascadedLevelCount; i++)
	{
		fmtToNew(shadowMapName, "csm_shadow_map-l{}", i);

		Material* mtl = materials[i];
		{
			fmtToNew(passName, "{}-l{}", "csm_shadow_map_", i);
			auto& pass = rdGraph->addPass(passName, RdgPassTypeFlags::Graphics);
			//pass.setRenderTarget(result.depthTex, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
			pass.setDepthStencil(shadowMap, i, RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
			pass.setExecuteFunc(
				[=, light_vp = oResult->lightMatrices[i]] (RenderRequest& rdReq)
				{
					rdReq.reset(rdGraph->renderContext(), drawData);

					rdReq.setViewport(viewport);
					rdReq.setScissorRect(viewport);

					auto* clearValue = rdReq.clearFramebuffers();
					clearValue->setClearDepth();
					//clearValue->setClearColor();

					//mtl->setParam("map_level",	i);
					mtl->setParam("light_vp",	light_vp);

					DrawSettings drawSettings;
					Frustum3f frustum; 
					frustum.setByViewProjMatrix(light_vp); 
					drawSettings.cullingSetting.setCameraFrustum(frustum);

					drawData->drawScene(rdReq, mtl, drawSettings);
				}
			);
			passCascadedShadowMapping = &pass;
		}
		oResult->shadowMap = shadowMap;
		//oResult->depthMaps[i] = depthMap;
	}

	return passCascadedShadowMapping;
}

RdgPass* 
RpfCascadedShadowMaps::addCascadedShadowMappingPass(const math::Camera3f& camera, const Vec3f& lightDir)
{
	RdgPass* passCascadedShadowMapping = addCascadedShadowMappingPass(_mtlsCsmShadowMap, &result, param, camera, lightDir);
	return passCascadedShadowMapping;
}

RdgPass* 
RpfCascadedShadowMaps::addDebugFrustumsPass(Material* mtl_, RdgTextureHnd rtColor, const math::Camera3f& camera_, const Vec3f& lightDir, bool isUpdateCamera)
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passDebugFrustums	= nullptr;

	static auto camera = camera_;
	if (isUpdateCamera)
	{
		camera = camera_;
	}

	Material* mtl = mtl_;
	{
		auto& pass = rdGraph->addPass("csm_debug_frustums", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		pass.setExecuteFunc(
			[=, param = param](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);
				rdReq.lineMaterial = mtl;

				//auto* clearValue = rdReq.clearFramebuffers();
				//clearValue->setClearColor();

				{
					Result result;

					auto  cascadedLevelCount	= getCascadedLevelCount(param.cascadedSplitCount);

					Vector<float, 16> cascadeSplits;
					cascadeSplits.resize(cascadedLevelCount);
					computeCasadedSplits(cascadeSplits.data(), cascadedLevelCount, param.cascadeSplitLambda, camera);

					result.lightMatrices.reserve(cascadedLevelCount);

					auto camVpInv = camera.viewProjMatrix().inverse();

					float lastSplitDist = 0.0;
					for (u32 i = 0; i < cascadedLevelCount; ++i)
					{
						auto lightMatrix = computeLightSpaceMatrix(result.cascadePlaneDists, camera, camVpInv, cascadeSplits[i], lastSplitDist, lightDir);

						Frustum3f frustum;
						Frustum3f lightFrustum;

						computeCascadedFrustum(frustum, camVpInv, cascadeSplits[i], lastSplitDist);
						lightFrustum.setByViewProjMatrix(lightMatrix);

						rdReq.drawFrustum(frustum,		Color4f{1.0, 0.0f, 0.0f, 1.0f});
						rdReq.drawFrustum(lightFrustum,	Color4f{0.0, 1.0f, 0.0f, 1.0f});

						lastSplitDist = cascadeSplits[i];
					}
				}

				drawData->setupMaterial(mtl);
			});
		passDebugFrustums = &pass;
	}

	return passDebugFrustums;
}

#endif
}