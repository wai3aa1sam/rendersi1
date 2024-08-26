#include "rds_pbr_ibl-pch.h"

#include "rds_pbr_ibl.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"


#define ZoneNamedTest( varname, name ) \
tracy::SourceLocationData TracyConcat(__tracy_source_location,TracyLine) { name, TracyFunction,  TracyFile, (uint32_t)TracyLine, 0 };  \
tracy::ScopedZone varname( &TracyConcat(__tracy_source_location,TracyLine), TRACY_CALLSTACK, true ) \
// ---

namespace rds
{

static String name;

#if 0
#pragma mark --- rdsPbrIbl-Impl ---
#endif // 0
#if 1

void 
PbrIbl::onCreate()
{
	Base::onCreate();
	// temporary
	auto& rdPassPipeline = *_rdPassPipelines[0];
	_rpfPbrIbl	= rdPassPipeline.addRenderPassFeature<RpfPbrIbl>();

	name.reserve(10000);
	name = "HELLOOOOOOOOOOOOOOOOOO";
}

void 
PbrIbl::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	u32 objCount = 7;

	auto shader = Renderer::rdDev()->createShader("asset/shader/lighting/rdsDefaultLighting.shader");
	createDefaultScene(oScene, shader, meshAssets().sphere, Vec3u{objCount, objCount, 1});
	createLights(oScene, Vec3u{ 1, 1, 1 }, Vec3f::s_zero(), Vec3f::s_one() * 3.0f, Quat4f::s_euler(Vec3f{math::radians(134.398f), math::radians(20.0f), math::radians(-4.4f)}));

	{
		/*auto totalCount = (objCount * objCount - 1);
		totalCount = math::clamp<u32>(totalCount, 1, totalCount);
		auto metalnessStep = 1.0f / totalCount;
		auto roughnessStep = 1.0f / totalCount;*/
		objCount = math::max(2, (int)objCount);

		u32 row = 0;
		u32 col = 0;

		for (auto& e : scene().entities())
		{
			//auto* transf = e->getComponent<CTransform>();
			auto* rdable = e->getComponent<CRenderableMesh>();
			if (!rdable || !rdable->meshAsset)
				continue;

			auto& mtl = rdable->material;
			
			mtl->setParam("metalness",			math::clamp((float)row / (objCount - 1), 0.0f, 1.0f));
			mtl->setParam("roughness",			math::clamp((float)col / (objCount - 1), 0.0f, 1.0f));
			mtl->setParam("ambientOcclusion",	1.0f);

			if (col < objCount)
			{
				col++;
			}
			else
			{
				row++;
				col = 0;
			}
		}
	}

	{
		auto& camera = app().mainWindow().camera();
		camera.setPos(11.656f,	14.808f,	26.251f);
		camera.setAim(8.410f,	9.803f,		0.226f);
		camera.setFov(45.0f);
		camera.setNearClip(	0.1f	* 10.0f);
		camera.setFarClip(	1000.0f * 10.0f);
	}
}

void 
PbrIbl::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);

	//auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	auto* rpfPbrIbl	= _rpfPbrIbl;
	rpfPbrIbl->addPreparePass(&_rpfPbrIblResult, defaultHdrEnv(), meshAssets().box->renderMesh);
}

void 
PbrIbl::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	ZoneNamedS(_asdasd, 32, true);

	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	//auto* rpfPbrIbl	= _rpfPbrIbl;

	RdgTextureHnd rtColor	= rdGraph->createTexture("pbr_ibl_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("pbr_ibl_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passPbrIbl = rdGraph->addPass("pbr_ibl", RdgPassTypeFlags::Graphics);
	passPbrIbl.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passPbrIbl.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);
	passPbrIbl.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData);

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			DrawSettings drawSettings;
			drawSettings.cullingSetting.setCameraFrustum(drawData->camera->frustum());
			drawSettings.setMaterialFn = 
				[=](Material* mtl_)
				{
					mtl_->setParam("tex_brdfLut",			_rpfPbrIblResult.brdfLut);
					mtl_->setParam("cube_irradianceEnv",	_rpfPbrIblResult.cubeIrradinceMap);
					mtl_->setParam("cube_prefilteredEnv",	_rpfPbrIblResult.cubePrefilteredMap);

					mtl_->setParam("tex_brdfLut",			SamplerState::makeLinearClampToEdge());
					mtl_->setParam("cube_irradianceEnv",	SamplerState::makeLinearClampToEdge());
					mtl_->setParam("cube_prefilteredEnv",	SamplerState::makeLinearClampToEdge());
				};
			drawData->drawScene(rdReq, drawSettings);
		}
	);

	addSkyboxPass(rdGraph, drawData, _rpfPbrIblResult.cubeEnvMap, rtColor, dsBuf);
	addPostProcessingPass(rdGraph, drawData, &rtColor);
	//addDrawLightOutlinePass(rdGraph, drawData, rtColor, drawData->mtlLine());

	drawData->oTexPresent = rtColor;
}

void 
PbrIbl::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	
}

void 
PbrIbl::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
PbrIbl::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}

#endif

}