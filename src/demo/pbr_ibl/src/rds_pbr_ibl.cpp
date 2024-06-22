#include "rds_pbr_ibl-pch.h"

#include "rds_pbr_ibl.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

#include "rds_render/pass_feature/lighting/pbr/rdsRpfPbrIbl.h"

namespace rds
{

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
}

void 
PbrIbl::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	u32 objCount = 6;

	auto shader = Renderer::rdDev()->createShader("asset/shader/lighting/rdsDefaultLighting.shader");
	createDefaultScene(oScene, shader, meshAssets().sphere, Vec3u{objCount, objCount, 1});

	auto totalCount = (objCount * objCount - 1);
	auto metalnessStep = 1.0f / totalCount;
	auto roughnessStep = 1.0f / totalCount;

	auto i = 0;
	for (auto& e : scene().entities())
	{
		auto* transf = e->getComponent<CTransform>();

		{
			auto* light = e->getComponent<CLight>();
			if (light)
			{
				transf->setLocalRotation(Vec3f(math::radians(134.398f), math::radians(20.0f), math::radians(-4.4f)));
			}
		}
		auto* rdable = e->getComponent<CRenderableMesh>();
		if (!rdable)
			continue;

		auto& mtl = rdable->material;
		mtl->setParam("metalness",			metalnessStep * (totalCount - i));
		mtl->setParam("roughness",			roughnessStep * i);
		mtl->setParam("ambientOcclusion",	0.2f);
		++i;
	}

	{
		auto& camera = app().mainWindow().camera();
		camera.setPos(12.987f,	14.578f,	20.333f);
		camera.setAim(8.021f,	7.989f,		-0.492f);
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
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData*>();
	auto	screenSize	= drawData->resolution2u();

	auto* rpfPbrIbl	= _rpfPbrIbl;
	rpfPbrIbl->addPreparePass(defaultHdrEnv(), meshAssets().box->renderMesh);
}

void 
PbrIbl::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData*>();
	auto	screenSize	= drawData->resolution2u();

	auto* rpfPbrIbl	= _rpfPbrIbl;

	RdgTextureHnd rtColor	= rdGraph->createTexture("pbr_ibl_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
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
					mtl_->setParam("tex_brdfLut",			_rpfPbrIbl->result.brdfLut);
					mtl_->setParam("cube_irradianceEnv",	_rpfPbrIbl->result.cubeIrradinceMap);
					mtl_->setParam("cube_prefilteredEnv",	_rpfPbrIbl->result.cubePrefilteredMap);
				};

			drawData->drawScene(rdReq, drawSettings);
		}
	);

	addSkyboxPass(rdGraph, drawData, rpfPbrIbl->result.cubeEnvMap, rtColor, dsBuf);


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