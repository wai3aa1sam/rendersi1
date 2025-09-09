#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim2D_Gpu.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_Gpu-Impl ---
#endif // 0
#if 1

void 
FluidSim2D_Gpu::onCreate(GraphicsDemo* parentDemo)
{
	Base::onCreate(parentDemo);

	RenderUtil::createMaterial(&_shaderFs2d, &_mtlFs2d,	"asset/shader/demo/fluid_simulation/2d/rdsFluidSim2D.shader");
}

void 
FluidSim2D_Gpu::onUpdate(float dt)
{
	Base::onUpdate(dt);

}

void 
FluidSim2D_Gpu::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);

}

void 
FluidSim2D_Gpu::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	RdgTextureHnd rtColor		= rdGraph->createTexture("fs2d_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf			= rdGraph->createTexture("fs2d_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil});

	SimArgs simArgs;
	simArgs.create(this, rdGraph, drawData);

	addPass_calcExternalForce(simArgs);

	{
		auto& passFluidSim2D_Cpu = rdGraph->addPass("fluid_sim_2d", RdgPassTypeFlags::Graphics);
		passFluidSim2D_Cpu.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
		passFluidSim2D_Cpu.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight

		passFluidSim2D_Cpu.readBuffer(simArgs.bufPos);
		passFluidSim2D_Cpu.readBuffer(simArgs.bufVel);

		RDS_TODO("******************remove later, jsut for test");
		passFluidSim2D_Cpu.readBuffer(simArgs.bufDensityData);
		passFluidSim2D_Cpu.readBuffer(simArgs.bufPredictedPos);
		// ---

		passFluidSim2D_Cpu.setExecuteFunc(
			[=, simArgs_ = simArgs](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData, drawData->lineMaterial());

				auto* clearValue = rdReq.clearFramebuffers();
				clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
				clearValue->setClearDepth(1.0f);

				drawData->setupMaterial(_parentDemo->mtlDrawCircle);
				rdReq.circleMaterial = _parentDemo->mtlDrawCircle;
				
				debug_drawBoundary(rdReq);
				debug_drawSpatialGrid(rdReq);
				//debug_drawSpatial(rdReq);
				debug_drawMouseInteraction(rdReq);

				_ptcDisplay.draw(rdReq, drawData
					, simArgs_.bufPos.renderResource(), simArgs_.bufVel.renderResource(), _simConfig.particleSize, simArgs_.particleCount);
			}
		);

		drawData->oTexPresent = rtColor;
	}
}

void 
FluidSim2D_Gpu::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	Base::onDrawGui(uiDrawReq);

}

void 
FluidSim2D_Gpu::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);

}

void 
FluidSim2D_Gpu::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);

}

void 
FluidSim2D_Gpu::update(float dt)
{

}

void 
FluidSim2D_Gpu::simulate(float dt)
{

}

void 
FluidSim2D_Gpu::addPass_calcExternalForce(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs2d;

	{
		auto& pass = rdGraph->addPass("fs2d_calcExternalForce", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.writeBuffer(simArgs.bufVel);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				RDS_TODO("render graph / RenderGpuBuffer should have a func for upload data directly");
				constCast(simArgs).createOncePositionBuffer();

				mtl->setParam("u_particleCount",	simArgs.particleCount);
				mtl->setParam("u_velocities",		simArgs.bufVel.renderResource());
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, Vec3u{simArgs.particleCount, 1, 1});
			}
		);
	}
}

#endif

}