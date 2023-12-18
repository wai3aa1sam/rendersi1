#pragma once

#include "rds_render_api_layer/mesh/rdsRenderMesh.h"
#include "rds_render_api_layer/command/rdsRenderRequest.h"
#include "rds_render_api_layer/rdsRenderFrame.h"
#include "rds_render_api_layer/graph/rdsRenderGraph.h"

#include "rds_editor.h"

namespace rds
{

class Test_RenderGraph : public NonCopyable
{
public:

	Test_RenderGraph()
	{
	}

	void setup(RenderContext* rdCtx, bool create = true)
	{
		if (create)
		{
			_rdCtx = rdCtx;
			_rdGraph.create("Test Render Graph", rdCtx);
			//return;
		}

		SPtr<Texture2D> backBuffer;
		SPtr<Texture2D> outColor;
		SPtr<Texture2D> out1;

		auto screenSize = Vec2u::s_cast(rdCtx->framebufferSize()).toTuple2();

		auto depthTex = _rdGraph.createTexture("depth_tex", Texture2D_CreateDesc{ screenSize, ColorType::Depth, 1, TextureFlags::DepthStencil | TextureFlags::ShaderResource});

		auto& depthPrePass = _rdGraph.addPass("depth_pre_pass", RdgPassTypeFlags::Graphics);
		depthPrePass.setDepthStencil(depthTex, RdgAccess::Write, RenderTargetLoadOp::DontCare, RenderTargetLoadOp::DontCare);
		depthPrePass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearDepth(1.0f);


			});

		
		auto albedoTex		= _rdGraph.createTexture("albedo_tex",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });
		auto normalTex		= _rdGraph.createTexture("normal_tex",		Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });
		auto positionTex	= _rdGraph.createTexture("position_tex",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });
		
		auto& gBufferPass = _rdGraph.addPass("g_buffer_pass", RdgPassTypeFlags::Graphics);
		//gBufferPass.setRenderTarget({albedoTex, normalTex, positionTex});
		gBufferPass.setRenderTarget(albedoTex,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setRenderTarget(normalTex,		RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setRenderTarget(positionTex,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		gBufferPass.setDepthStencil(depthTex,		RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		gBufferPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{

			}
		);

		auto colorTex = _rdGraph.createTexture("color_tex", { screenSize, ColorType::RGBAb, 1, TextureFlags::RenderTarget });

		auto& deferredLightingPass = _rdGraph.addPass("deferred_lighting_pass", RdgPassTypeFlags::Graphics);
		deferredLightingPass.readTextures({albedoTex, normalTex, positionTex, depthTex});
		//deferredLightingPass.setDepthStencil(depthTex, RdgAccess::Read, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		deferredLightingPass.setRenderTarget(colorTex, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		//deferredLightingPass.setDepthStencil(depthTex, RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
		deferredLightingPass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{

			}
		);

		/*auto backBufferRt = _rdGraph.importTexture("back_buffer", backBuffer);

		auto& finalComposePass = _rdGraph.addPass("final_compose", RdgPassTypeFlags::Graphics);
		finalComposePass.readTexture(colorTex);
		finalComposePass.setRenderTarget(backBufferRt, RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		finalComposePass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{

			}
		);*/

		_rdGraph.exportTexture(outColor, colorTex);
		_rdGraph.exportTexture(out1, albedoTex);
	}

	void compile()
	{
		setup(_rdCtx, false);
		_rdGraph.compile();
	}

	void execute()
	{
		_rdGraph.execute();
	}

	void commit()
	{
		_rdGraph.commit();
	}

	void dump(StrView filename = "debug/render_graph")
	{
		_rdGraph.dumpGraphviz(filename);
	}


	void test()
	{

	}

public:
	RenderContext*	_rdCtx = nullptr;
	RenderGraph		_rdGraph;
};

}