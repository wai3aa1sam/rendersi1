#include "rds_voxel_cone_tracing-pch.h"
#include "rds_voxel_cone_tracing.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

#include "rds_render/pass_feature/geometry/rdsRpfGeometryBuffer.h"
#include "rds_render/pass_feature/shadow/cascaded_shadow_mapping/rdsRpfCascadedShadowMapping.h"
#include "rds_render/pass_feature/lighting/gi/vxgi/rdsRpfVoxelConeTracing.h"

#define RDS_LOAD_SPONZA 1
#define RDS_TEST_SHADOW 0
#define RDS_USE_SHADOW 0

namespace rds
{

/*
* TODO: remove
*/
static u64				s_frame = 0;

#if 0

struct Foo
{
	Foo()
	{
		RDS_LOG("ctor");
}

	~Foo()
	{
		RDS_LOG("dtor");
	}

	Foo(const Foo& f)
	{
		RDS_LOG("copy ctor");
	}

	void operator=(const Foo& f)
	{
		RDS_LOG("copy op");
	}

	Foo(Foo&& f)
	{
		RDS_LOG("move ctor");
	}

	void operator=(Foo&& f)
	{
		RDS_LOG("move op");
	}

	int a = 0;
};

struct Test
{
	void test(Foo& f)
	{
		std::function<void(void)> fn;


		fn = [=]()
			{
				RDS_LOG("{}", f.a);
			};
		fn();

		abort();
	}

	Foo _f;
};
#endif // 0

#if 0
#pragma mark --- rdsVoxelConeTracing-Impl ---
#endif // 0
#if 1

void 
VoxelConeTracing::onCreate()
{
	Base::onCreate();

	// temporary
	auto& rdPassPipeline = *_rdPassPipelines[0];

	_rpfGeomBuf = rdPassPipeline.addRenderPassFeature<RpfGeometryBuffer>();
	_rpfVct		= rdPassPipeline.addRenderPassFeature<RpfVoxelConeTracing>();
	_rpfCsm		= rdPassPipeline.addRenderPassFeature<RpfCascadedShadowMapping>();
}

void 
VoxelConeTracing::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);
	
	addEntity(_rpfVct->getVoxelVisualizationMaterial())->id();
	addEntity(_rpfVct->getVoxelConeTracingMaterial());

	auto shader = RDS_TEST_SHADOW ? Renderer::rdDev()->createShader("asset/shader/lighting/rdsDefaultLighting.shader") : _rpfVct->getVoxelizationShader();

	{
		#if RDS_LOAD_SPONZA
		meshAssets().loadSponza(shader);
		#endif // 0

		#if RDS_TEST_SHADOW
		auto& meshes		= meshAssets();
		auto& defaultMesh	= meshes.suzanne;

		u32 objCount = 6;
		createDefaultScene(oScene, shader, meshes.plane, Vec3u{1, 1, 1}, Vec3f{0.0f, -0.5f, 5.0f}, Vec3f{10.0f, 0.0f, 10.0f}, Vec3f{64.0, 1.0, 64.0});
		createDefaultScene(oScene, shader, defaultMesh, Vec3u{objCount, 1, objCount}, Vec3f{0.0f, 8.0f, 5.0f}, Vec3f{10.0f, 0.0f, 10.0f}, Vec3f::s_one() * 5.0f);
		#endif // 0
	}

	{
		auto& meshes		= meshAssets();
		

		createLights(oScene, Vec3u{ 5, 5, 1 });

		if (meshes.sponza)
		{
			float scale = 0.25f / 2.0f;
			//scale = 1.0f;
			meshes.sponza->addToScene(oScene, Mat4f::s_scale(Vec3f::s_one() * scale));
			//meshes.sponza->addToScene(oScene);
		}

		{
			auto& camera = app().mainWindow().camera();
			camera.setPos(-38.749f, 48.477f, -10.180f);
			camera.setFov(45.0f);
			camera.setAim(2.953f, 20.460f, 4.716f);
			camera.setNearClip(	0.1f	* 10.0f);
			camera.setFarClip(	1000.0f * 10.0f);

			camera.setPos(-36.930f, 41.249f, -24.413f);
			camera.setAim(-34.879f, 38.978f, -21.249f);

			camera.setPos(-123.558f, 74.239f, -7.161f);
			camera.setAim(-111.791f, 69.775f, -7.801f);
		}
	}
}

void 
VoxelConeTracing::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);

	//auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();
}

void 
VoxelConeTracing::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	auto* rpfGeomBuf	= _rpfGeomBuf;
	auto* rpfVct		= _rpfVct;
	auto* rpfCsm		= _rpfCsm;
	
	RdgTextureHnd rtColor		= rdGraph->createTexture("vct_rtColor",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf			= rdGraph->createTexture("vct_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferSrc});
	RdgTextureHnd texDepth		= rdGraph->createTexture("vct_texDepth",	Texture2D_CreateDesc{ screenSize, ColorType::Rf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource /*| TextureUsageFlags::TransferDst*/});

	DrawSettings drawSettings;
	auto camera = *drawData->camera;
	//camera.setFov(camera.fov() / 2.0f);
	drawSettings.cullingSetting.setCameraFrustum(camera.frustum());

	auto& csmResult = rpfCsm->result;

	#if RDS_USE_SHADOW
	rpfCsm->addCascadedShadowMappingPass(*drawData->camera, directionalLightTransform()->forward());
	addTestCascadedShadowMappingPass(rdGraph, drawData, drawSettings, rtColor, dsBuf, csmResult);
	#endif

	#if !RDS_TEST_SHADOW
	rpfGeomBuf->addGeometryPass(drawSettings, dsBuf);
	
	rpfVct->addVoxelizationPass(drawSettings, csmResult);
	rpfVct->addOpacityAlphaPass(&rpfVct->result);
	//rpfVct->addCheckAlphaPass();

	#if !VCT_USE_6_FACES_CLIPMAP
	rpfVct->addAnisotropicMipmappingPass();
	#endif // VCT_USE_6_FACES_CLIPMAP

	rpfVct->addVoxelConeTracingPass(dsBuf, rpfGeomBuf->result);
	rpfVct->addLightingPass(rtColor, dsBuf, rpfGeomBuf->result);

	if (isShowDebugVoxelClipmap)
	{
		rpfVct->addVoxelizationDebugPass(rtColor, dsBuf);
	}
	if (isShowVoxelClipmap)
	{
		rpfVct->addVoxelVisualizationPass(rtColor, dsBuf);
	}
	
	#endif // 0

	{
		addSkyboxPass(rdGraph, drawData, skyboxDefault(), rtColor, dsBuf);
		addPostProcessingPass(rdGraph, drawData, &rtColor);

		addDrawLightOutlinePass(rdGraph, drawData, rtColor, nullptr);
		//addDisplayNormalPass(rdGraph, drawData, rtColor);
		//addDisplayAABBoxPass(rdGraph, drawData, rtColor, drawSettings);
	}

	//rpfCsm->addDebugFrustumsPass(drawData->mtlLine(), rtColor, *drawData->camera, directionalLightTransf->forward(), app().mainWindow().uiInput().isKeyDown(UiKeyboardEventButton::Space));

	{
		if (voxelTextureIdx == 1)
		{
			drawData->oTexPresent = rpfVct->result.texDiffuse;
		}
		else if (voxelTextureIdx == 2)
		{
			drawData->oTexPresent = rpfVct->result.texSpecular;
		}
		else if (voxelTextureIdx == 3)
		{
			drawData->oTexPresent = rpfGeomBuf->result.normal;
		}
		else
		{
			drawData->oTexPresent = rtColor;
		}

		if (drawData->oTexPresent)
		{
			voxelTextureName = drawData->oTexPresent.name();
		}
	}

	RDS_TODO("remove");
	s_frame++;
	#if VCT_USE_6_FACES_CLIPMAP
	rpfVct->curLevel = s_frame % rpfVct->clipmapMaxLevel;
	#endif // VCT_USE_6_FACES_CLIPMAP
}

void 
VoxelConeTracing::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	Base::onDrawGui(uiDrawReq);

	auto* rpfVct = _rpfVct;

	auto wnd = uiDrawReq.makeWindow("Debug");
	uiDrawReq.dragFloat("clipmapRegionWorldExtentL0",	&_rpfVct->clipmapRegionWorldExtentL0);
	rpfVct->clipmapRegionWorldExtentL0 = math::max(sCast<float>(rpfVct->voxelResolution / 2.0f), rpfVct->clipmapRegionWorldExtentL0);

	int curLevel = sCast<int>(rpfVct->curLevel);
	uiDrawReq.dragInt("curLevel", &curLevel);
	rpfVct->curLevel = math::clamp<u32>(curLevel, 0, rpfVct->clipmapMaxLevel);

	int visualizeLevel = sCast<int>(rpfVct->visualizeLevel);
	uiDrawReq.dragInt("visualizeLevel",	&visualizeLevel);
	rpfVct->visualizeLevel = math::clamp<u32>(visualizeLevel, 0, rpfVct->clipmapMaxLevel);

	uiDrawReq.showText("voxelTexture: {}", voxelTextureName);
	uiDrawReq.dragInt("voxelTexture", &voxelTextureIdx);
	//voxelTextureIdx = math::clamp(voxelTextureIdx, 0, 2);

	uiDrawReq.makeCheckbox("isShowVoxelClipmap",		&isShowVoxelClipmap);
	uiDrawReq.makeCheckbox("isShowDebugVoxelClipmap",	&isShowDebugVoxelClipmap);

	uiDrawReq.dragFloat("lambda", &_rpfCsm->param.cascadeSplitLambda, 0.01f);
}

void 
VoxelConeTracing::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
VoxelConeTracing::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}

RdgPass* 
VoxelConeTracing::addTestCascadedShadowMappingPass(RenderGraph* oRdGraph, DrawData* drawData, const DrawSettings& drawSettings, RdgTextureHnd rtColor, RdgTextureHnd dsBuf, RpfCascadedShadowMapping_Result& csmResult)
{
	auto*	 rdGraph						= oRdGraph;
	RdgPass* passTestCascadedShadowMapping	= nullptr;
	#if 1
	{
		auto& pass = rdGraph->addPass("temp", RdgPassTypeFlags::Graphics);
		pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		pass.setDepthStencil(dsBuf,		RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::DontCare);
		#if RDS_USE_SHADOW
		pass.readTexture(csmResult.shadowMap);
		#endif // RDS_USE_SHADOW
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				//auto mtl = rpfVct->mtlVoxelConeTracing;
				rdReq.reset(rdGraph->renderContext(), drawData);

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor();
				clearValue->setClearDepth();

				#if RDS_USE_SHADOW
				constCast(drawSettings).setMaterialFn =
					[=](Material* mtl_)
					{
						//auto& csmResult = rpfCsm->result;
						mtl_->setArray("csm_matrices",		csmResult.lightMatrices);
						mtl_->setArray("csm_plane_dists",	csmResult.cascadePlaneDists);
						mtl_->setParam("csm_level_count",	sCast<u32>(csmResult.cascadePlaneDists.size()));
						mtl_->setParam("csm_shadowMap",		csmResult.shadowMap.texture2DArray());
						mtl_->setParam("csm_shadowMap",		SamplerState::makeNearestClampToEdge());
					};
				#endif // RDS_USE_SHADOW

				#if RDS_TEST_SHADOW
				drawData->drawScene(rdReq, drawSettings);
				#endif // RDS_USE_DEFAULT_SCENE
			}
		);
		passTestCascadedShadowMapping = &pass;
	}
	#endif
	return passTestCascadedShadowMapping;
}


#endif


}