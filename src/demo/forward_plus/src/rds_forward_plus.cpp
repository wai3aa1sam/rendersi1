#include "rds_forward_plus-pch.h"

#include "rds_forward_plus.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{

void 
ForwardPlus::onCreate()
{
	Base::onCreate();

	createMaterial(&_shaderForwardPlus, &_mtlForwardPlus, "asset/shader/demo/forward_plus/forward_plus.shader"
					, [&](Material* mtl) {mtl->setParam("texture0", texUvChecker()); });
	createMaterial(&_shaderSkybox, &_mtlSkybox, "asset/shader/skybox.shader"
					, [&](Material* mtl) {mtl->setParam("skybox", skyboxDefault()); });
}

void 
ForwardPlus::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);

	createDefaultScene(oScene, _mtlForwardPlus, meshAssets().suzanne, 25);
}

void 
ForwardPlus::onPrepareRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onPrepareRender(oRdGraph, drawData);

}

void 
ForwardPlus::onExecuteRender(RenderGraph* oRdGraph, DrawData* drawData)
{
	Base::onExecuteRender(oRdGraph, drawData);

	auto*	rdGraph		= oRdGraph;
	auto*	rdCtx		= rdGraph->renderContext();
	auto	screenSize	= Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

	RdgTextureHnd texColor	= rdGraph->createTexture("forward_plus_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd texDepth	= rdGraph->createTexture("forward_plus_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passForwardPlus = rdGraph->addPass("forward_plus", RdgPassTypeFlags::Graphics);
	passForwardPlus.setRenderTarget(texColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passForwardPlus.setDepthStencil(texDepth,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
	passForwardPlus.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			auto mtl = _mtlForwardPlus;
			rdReq.reset(rdGraph->renderContext(), *drawData);

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			drawData->sceneView->drawScene(rdReq, mtl, drawData);
		}
	);

	auto& skyboxPass = rdGraph->addPass("skybox", RdgPassTypeFlags::Graphics);
		skyboxPass.setRenderTarget(texColor, RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		if (texDepth)
			skyboxPass.setDepthStencil(texDepth, RenderAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		skyboxPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), *drawData);
				auto mtl = _mtlSkybox;

				mtl->setParam("skybox", skyboxDefault());
				drawData->setupMaterial(mtl);
				rdReq.drawMesh(RDS_SRCLOC, meshAssets().box->rdMesh, mtl);
			});

	drawData->oTexPresent = texColor;
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

}