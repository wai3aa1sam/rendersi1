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

	void setup()
	{
		SPtr<Texture2D> backBuffer;
		SPtr<Texture2D> outColor;
		SPtr<Texture2D> out1;

		auto depthTex = _rdGraph.createTexture("depth_tex", {});

		auto& depthPrePass = _rdGraph.addPass("depth_pre_pass", RdgPassTypeFlag::Graphics);
		depthPrePass.setDepthStencil(depthTex, RdgResourceAccess::Write);
		depthPrePass.setExecuteFunc(
			[=]()
			{

			});

		
		auto albedoTex		= _rdGraph.createTexture("albedo_tex",		{});
		auto normalTex		= _rdGraph.createTexture("normal_tex",		{});
		auto positionTex	= _rdGraph.createTexture("position_tex",	{});

		auto& gBufferPass = _rdGraph.addPass("g_buffer_pass", RdgPassTypeFlag::Graphics);
		gBufferPass.setRenderTarget({albedoTex, normalTex, positionTex});
		gBufferPass.setExecuteFunc(
			[=]()
			{

			}
		);

		auto colorTex = _rdGraph.createTexture("color_tex", {});

		auto& deferredLightingPass = _rdGraph.addPass("deferred_lighting_pass", RdgPassTypeFlag::Graphics);
		deferredLightingPass.readTextures({albedoTex, normalTex, positionTex});
		deferredLightingPass.setDepthStencil(depthTex, RdgResourceAccess::Read);
		deferredLightingPass.setRenderTarget(colorTex);
		deferredLightingPass.setExecuteFunc(
			[=]()
			{

			}
		);

		auto backBufferRt = _rdGraph.importTexture("back_buffer", backBuffer);

		auto& finalComposePass = _rdGraph.addPass("final_compose", RdgPassTypeFlag::Graphics);
		finalComposePass.readTexture(colorTex);
		finalComposePass.setRenderTarget(backBufferRt);
		finalComposePass.setExecuteFunc(
			[=]()
			{

			}
		);

		_rdGraph.exportTexture(outColor, colorTex);
		_rdGraph.exportTexture(out1, albedoTex);
	}

	void compile()
	{
		_rdGraph.compile();
	}

	void execute()
	{
		_rdGraph.execute();
	}

	void dump(StrView filename = "debug/render_graph")
	{
		_rdGraph.dumpGraphviz(filename);
	}


	void test()
	{

	}

public:
	RenderGraph _rdGraph;
};

}