#include "rds_hello_triangle-pch.h"

#include "rds_hello_triangle.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{

#if 0
#pragma mark --- rdsHelloTriangle-Impl ---
#endif // 0
#if 1
void 
HelloTriangle::onCreate()
{
	Base::onCreate();
	createMaterial(&_shaderHelloTriangle, &_mtlHelloTriangle, "asset/shader/demo/hello_triangle/hello_triangle.shader"
					, [&](Material* mtl) {mtl->setParam("texture0", texUvChecker()); });
}

void 
HelloTriangle::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);
	createDefaultScene(oScene, _shaderHelloTriangle, meshAssets().suzanne, Vec3u{1, 1, 1});
}

void 
HelloTriangle::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);
}

void 
HelloTriangle::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	RdgTextureHnd rtColor	= rdGraph->createTexture("hello_triangle_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("hello_triangle_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passHelloTriangle = rdGraph->addPass("hello_triangle", RdgPassTypeFlags::Graphics);
	passHelloTriangle.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passHelloTriangle.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
	passHelloTriangle.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			auto mtl = _mtlHelloTriangle;
			rdReq.reset(rdGraph->renderContext(), drawData);

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			drawData->drawScene(rdReq, mtl);
		}
	);

	addDisplayAABBoxPass(rdGraph, drawData, rtColor, {});

	drawData->oTexPresent = rtColor;
}

void 
HelloTriangle::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	#if 0
	{
		ImGui::Begin("Permutation Test");

		static bool isCheckBox = false;
		static bool isENABLE_feature = false;
		static bool isClearPermutation = false;
		ImGui::Checkbox("ENABLE_permutation", &isCheckBox);
		ImGui::Checkbox("RDS_ENABLE_FEATURE_1", &isENABLE_feature);
		ImGui::Checkbox("isClearPermutation", &isClearPermutation);

		auto& mtl = _mtlHelloTriangle;

		if (isCheckBox)
		{
			if (isENABLE_feature)
				mtl->setPermutation("RDS_ENABLE_FEATURE_1", "1");
			else
				mtl->setPermutation("RDS_ENABLE_FEATURE_2", "0");
		}
		if (isClearPermutation)
		{
			Renderer::renderDevice()->waitIdle();
			mtl->clearPermutation();
			isClearPermutation = false;
		}

		ImGui::End();
	}
	#endif // 0
}

void 
HelloTriangle::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
HelloTriangle::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}
#endif

}