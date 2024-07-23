#include "rds_forward_plus-pch.h"

#include "rds_forward_plus.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

#include "rds_render/pass_feature/geometry/rdsRpfPreDepth.h"
#include "rds_render/pass_feature/rendering/forward_plus/rdsRpfForwardPlusRendering.h"

namespace rds
{

#define RDS_DEBUG_MATERIAL 0


#if 0
#pragma mark --- rdsForwardPlus-Impl ---
#endif // 0
#if 1

void 
ForwardPlus::onCreate()
{
	Base::onCreate();

	auto& rdPassPipeline = *_rdPassPipelines[0];

	_rpfPreDepth		= rdPassPipeline.addRenderPassFeature<RpfPreDepth>();
	_rpfFwdpRendering	= rdPassPipeline.addRenderPassFeature<RpfForwardPlusRendering>();
}

void 
ForwardPlus::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	float scaleFactor = 64.0f; RDS_UNUSED(scaleFactor);
	createDefaultScene(oScene, nullptr, meshAssets().plane, Vec3u::s_one(), Vec3f{0.0f, -1.0f, 0.0f}, Vec3f::s_one() * 3, Vec3f{ scaleFactor, 1.0, scaleFactor });

	AABBox3f b;
	b.min = Vec3f{-300.0f,   0.0f, -150.0f};
	b.max = Vec3f{+300.0f, 200.0f, +150.0f};
	createLights(oScene, Vec3u{ 25, 25, 1 }, Vec3f::s_zero(), Vec3f::s_zero(), Quat4f::s_eulerDegX(141.4f), 100, 50, b);

	meshAssets().loadSponza(nullptr);
	if (meshAssets().sponza)
	{
		meshAssets().sponza->addToScene(oScene, Mat4f::s_scale(Vec3f::s_one() * 0.25));
	}

	{
		auto& camera = app().mainWindow().camera();

		camera.setFov(45.0f);
		camera.setAim(0, 0, 0);
		camera.setNearClip(0.1f);
		camera.setFarClip(1000.0f);

		camera.setPos(-226.403f, 132.114f, -13.010f);
		camera.setAim(-158.399f, 120.766f, -16.858f);
	}
}

void 
ForwardPlus::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);
}

void 
ForwardPlus::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	//Renderer::rdDev()->waitIdle();

	RdgTextureHnd rtColor		= rdGraph->createTexture("fwdp_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf			= rdGraph->createTexture("fwdp_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferSrc});
	RdgTextureHnd texDepth		= rdGraph->createTexture("fwdp_texDepth",	Texture2D_CreateDesc{ screenSize, ColorType::Rf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource /*| TextureUsageFlags::TransferDst*/});

	DrawSettings drawSettings;
	auto camera = *drawData->camera;
	drawSettings.cullingSetting.setCameraFrustum(camera.frustum());

	_rpfPreDepth->addPreDepthPass(drawSettings, dsBuf, &texDepth);

	#if 1
	RpfForwardPlusRendering::Result fwdpResult;
	_rpfFwdpRendering->addLightCullingPass(fwdpResult, texDepth);
	 _rpfFwdpRendering->addLightingPass(drawSettings, rtColor, dsBuf, fwdpResult, true, true);

	#endif // 0

	addSkyboxPass(rdGraph, drawData, skyboxDefault(), rtColor, dsBuf);

	addPostProcessingPass(rdGraph, drawData, &rtColor);

	//_rfpForwardPlus->renderDebugMakeFrustums(, rtColor);
	//addDrawLightOutlinePass(rdGraph, drawData, rtColor, nullptr);	
	//addDisplayAABBoxPass(rdGraph, drawData, rtColor, drawSettings);

	if (isShowHeatmap)
	{
		_rpfFwdpRendering->addDrawLightHeatmapPass(rtColor, fwdpResult, true);
	}

	drawData->oTexPresent = rtColor;
}

void 
ForwardPlus::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	auto wnd = uiDrawReq.makeWindow("fwdp");
	uiDrawReq.makeCheckbox("isShowHeatmap", &isShowHeatmap);
}

void 
ForwardPlus::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
ForwardPlus::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}
#endif


}