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

static constexpr int N = 100;
static int arr[N];

void 
ForwardPlus::onCreate()
{
	#if 0
	_rdPassPipelines.emplace_back(makeUPtr<RenderPassPipeline>());
	if (true)
	{
		return;
	}
	#endif // 0

	Base::onCreate();

	auto& rdPassPipeline = *_rdPassPipelines[0];

	_rpfPreDepth		= rdPassPipeline.addRenderPassFeature<RpfPreDepth>();
	_rpfFwdpRendering	= rdPassPipeline.addRenderPassFeature<RpfForwardPlusRendering>();

	// just for test address sanitizers
	#if 0
	for (size_t i = 0; i < N; i++)
	{
		arr[i] = sCast<int>(i);
	}
	arr[1000] = 0;
	#endif // 0
}

void 
ForwardPlus::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	#if 1
	{
		auto& scene = *oScene;
		auto* ent = scene.addEntity("Debug Frustum"); RDS_UNUSED(ent);

		//auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		//rdableMesh->material	= _rfpForwardPlus->mtlFwdp;
		//rdableMesh->meshAsset	= meshAssets().fullScreenTriangle;

		//auto* transform	= ent->getComponent<CTransform>();
		//transform->setLocalPosition(startPos.x + step.x * c, 0.0f, startPos.y + step.y * r);
	}
	#if 0
	{
		auto& scene = *oScene;
		auto* ent = scene.addEntity("fwdp culling"); RDS_UNUSED(ent);
		auto* rdableMesh = ent->addComponent<CRenderableMesh>();
		rdableMesh->material	= _rpfFwdpRendering->_mtlFwdp;
	}
	#endif // 0

	#endif // 0

	createDefaultScene(oScene, nullptr, meshAssets().plane, Vec3u::s_one());
	for (auto& e : scene().entities())
	{
		auto& transform = e->transform();
		float scaleFactor = 64.0f; RDS_UNUSED(scaleFactor);
		transform.setLocalScale(Vec3f{ scaleFactor, 1.0, scaleFactor });
	}

	{
		auto& camera = app().mainWindow().camera();
		//camera.setPos(0, 10, 35);
		camera.setPos(-0.423f, 4.870f, 4.728f);
		camera.setPos(-0.348f, 4.011f, 3.894f);
		//camera.setPos(-0.572f, 6.589f, 6.397f); // threshold to reproduce the bug

		camera.setFov(45.0f);
		camera.setAim(0, 0, 0);
		camera.setNearClip(0.1f);
		camera.setFarClip(1000.0f);

		#if 0
		Vec3f scale;
		Quat4f rotation;
		Vec3f translation;
		Vec3f skew;
		Vec4f perspective;
		glm::decompose(camera.viewMatrix(), scale, rotation, translation, skew, perspective);
		RDS_DUMP_VAR(scale, rotation, translation, skew, perspective);
		RDS_DUMP_VAR(scale, rotation, translation, skew, perspective);
		#endif // 0
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

	#if 0
	if (true)
	{
		return;
	}
	#endif // 0

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData*>();
	auto	screenSize	= drawData->resolution2u();

	//Renderer::rdDev()->waitIdle();

	RdgTextureHnd rtColor		= rdGraph->createTexture("fwdp_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf			= rdGraph->createTexture("fwdp_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource | TextureUsageFlags::TransferSrc});
	RdgTextureHnd texDepth		= rdGraph->createTexture("fwdp_texDepth",	Texture2D_CreateDesc{ screenSize, ColorType::Rf,	TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource /*| TextureUsageFlags::TransferDst*/});

	RdgPass* fwdpPassFLighting = nullptr;

	_rpfPreDepth->addPreDepthPass({}, dsBuf, &texDepth);

	//_rpfFwdpRendering->addFwdpDebugPass();
	#if 1
	RpfForwardPlusRendering::Result fwdpResult;
	_rpfFwdpRendering->addLightCullingPass(fwdpResult, texDepth);
	fwdpPassFLighting = _rpfFwdpRendering->addLightingPass(rtColor, dsBuf, fwdpResult, true, true);
	#endif // 0

	//if (passFwdpLighting)
	//	addSkyboxPass(, skyboxDefault(), rtColor, dsBuf);
	//addPostProcessPass(, "debug_fwdp", rtColor,)

	//_rfpForwardPlus->renderDebugMakeFrustums(, rtColor);
	if (fwdpPassFLighting)
		addDrawLightOutlinePass(rdGraph, drawData, rtColor, nullptr);	

	drawData->oTexPresent = fwdpPassFLighting ? rtColor : RdgTextureHnd{};
}

void 
ForwardPlus::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{

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