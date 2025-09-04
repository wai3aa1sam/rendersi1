#include "rds_fluid_simulation-pch.h"

#include "rds_fluid_simulation.h"

#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSimulation-Impl ---
#endif // 0
#if 1
void 
FluidSimulation::onCreate()
{
	Base::onCreate();
	createMaterial(&_shaderFluidSimulation, &_mtlFluidSimulation, "asset/shader/demo/fluid_simulation/fluid_simulation.shader"
					, [&](Material* mtl) {mtl->setParam("texture0", texUvChecker()); });
}

void 
FluidSimulation::onCreateScene(Scene* oScene)			
{
	Base::onCreateScene(oScene);
	createDefaultScene(oScene, _shaderFluidSimulation, meshAssets().suzanne, Vec3u{1, 1, 1});
}

void 
FluidSimulation::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);
}

void 
FluidSimulation::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	RdgTextureHnd rtColor	= rdGraph->createTexture("fluid_simulation_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("fluid_simulation_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passFluidSimulation = rdGraph->addPass("fluid_simulation", RdgPassTypeFlags::Graphics);
	passFluidSimulation.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passFluidSimulation.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
	passFluidSimulation.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			auto mtl = _mtlFluidSimulation;
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
FluidSimulation::onDrawGui(EditorUiDrawRequest& uiDrawReq)
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

		auto& mtl = _mtlFluidSimulation;

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
FluidSimulation::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);
}

void 
FluidSimulation::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}
#endif

}