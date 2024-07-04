#include "rds_render-pch.h"
#include "rdsRpfShadowMapping.h"

namespace rds
{


#if 0
#pragma mark --- rdsRpfShadowMapping-Impl ---
#endif // 0
#if 1

RpfShadowMapping::RpfShadowMapping()
{

}

RpfShadowMapping::~RpfShadowMapping()
{
	destroy();
}

void 
RpfShadowMapping::create()
{
	RenderUtil::createMaterial(&_shaderShadowMap, &_mtlShadowMap, "asset/shader/pass_feature/shadow/shadow_mapping/rdsShadowMapping_ShadowMap.shader");
}

void 
RpfShadowMapping::destroy()
{
	RenderUtil::destroyShaderMaterial(_shaderShadowMap, _mtlShadowMap);
}

RdgPass* 
RpfShadowMapping::addShadowMappingPass(SPtr<Material>& mtl_, Result* oResult, const Param& param_, const Vec3f& lightDir)
{
	auto*		rdGraph				= renderGraph();
	auto*		drawData			= drawDataBase();
	RdgPass*	passShadowMapping	= nullptr;

	Vec2u			mapSize		= Vec2u::s_one() * param_.mapSize;
	RdgTextureHnd	depthMap	= rdGraph->createTexture("shadow_mapping_depthMap", Texture2D_CreateDesc{ mapSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource });
	//result.depthTex = rdGraph->createTexture("shadow_mapping_depthTex", Texture2D_CreateDesc{ mapSize, ColorType::Rf, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource });

	auto viewport = Rect2f{ Tuple2f::s_zero(), Tuple2f{param.mapSize, param.mapSize} };

	RenderUtil::createMaterial(_shaderShadowMap, &mtl_);
	Material* mtl = mtl_;
	{
		auto& pass = rdGraph->addPass("shadow_mapping", RdgPassTypeFlags::Graphics);
		//pass.setRenderTarget(result.depthTex, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(depthMap, RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
		pass.setExecuteFunc(
			[=, param = param_](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData);

				rdReq.setViewport(viewport);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearDepth();
				//clearValue->setClearColor();

				float nearPlane = 1.0f;
				float farPlane	= 7.5f;
				float planeSize	= 10.0f;

				Mat4f lightProj	= Mat4f::s_ortho(-planeSize, planeSize, -planeSize, planeSize, nearPlane, farPlane);
				Mat4f lightView	= Mat4f::s_lookAt(lightDir + Vec3f::s_one() * math::epsilon<float>(), Vec3f::s_zero(), Vec3f::s_up());	// use light model matrix inv is better

				Mat4f lightVp	= lightProj * lightView;

				mtl->setParam("light_vp", lightVp);

				DrawSettings drawSettings;
				Frustum3f frustum; 
				frustum.setByViewProjMatrix(lightVp); 
				drawSettings.cullingSetting.setCameraFrustum(frustum);

				drawData->drawScene(rdReq, mtl, drawSettings);
			}
		);
		passShadowMapping = &pass;
	}

	oResult->depthMap = depthMap;

	return passShadowMapping;

}

RdgPass* 
RpfShadowMapping::addShadowMappingPass(const Vec3f& lightDir)
{
	RdgPass* passShadowMapping = addShadowMappingPass(_mtlShadowMap, &result, param, lightDir);
	return passShadowMapping;
}


#endif

}