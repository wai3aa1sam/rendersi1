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

	RdgTextureHnd rtColor	= rdGraph->createTexture("fs2d_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("fs2d_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil});

	float dt = 1 / 120.0f;

	SimArgs simArgs;
	simArgs.create(this, dt, rdGraph, drawData);

	addPass_simulateFluid2D(simArgs);

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
FluidSim2D_Gpu::addPass_simulateFluid2D(SimArgs& simArgs)
{
	Material*	mtl		= _mtlFs2d;
	{
		mtl->setParam("u_dt",						simArgs.dt);
		mtl->setParam("u_gravity",					_simConfig.gravity);
		mtl->setParam("u_gravityDir",				_simConfig.gravityDir.normalize());
		mtl->setParam("u_collisionDamping",			_simConfig.collisionDamping);

		mtl->setParam("u_interactionInputStrength",	_simState.interactionInputStrength);
		mtl->setParam("u_interactionInputRadius",	_simConfig.interactionRadius);
		mtl->setParam("u_interactionInputPoint",	_mouseRayWorld.origin.toVec2());

		mtl->setParam("u_boundarySize",				_simConfig.boundingRegion.size);
		mtl->setParam("u_obstacleCenter",			Vec2f{_simConfig.obstacle.pos} + Vec2f{_simConfig.obstacle.size} / 2.0f);
		mtl->setParam("u_obstacleSize",				_simConfig.obstacle.size);

		mtl->setParam("u_particleMass",				_simConfig.particleMass);
		mtl->setParam("u_particleCount",			simArgs.particleCount);
	}

	addPass_calcExternalForce(simArgs);
	addPass_updateSpatialLut(simArgs);
	addPass_calcDensityData(simArgs);
	addPass_calcViscosity(simArgs);
	addPass_calcPressureForce(simArgs);
	addPass_updatePosition(simArgs);
}

void
FluidSim2D_Gpu::addPass_calcExternalForce(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs2d;

	{
		auto& pass = rdGraph->addPass("fs2d_calcExternalForce", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.writeBuffer(simArgs.bufVel);
		pass.writeBuffer(simArgs.bufPredictedPos);
		pass.readBuffer(simArgs.bufPos);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				RDS_TODO("render graph / RenderGpuBuffer should have a func for upload data directly");
				constCast(simArgs).createOncePositionBuffer();

				mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
				mtl->setParam("u_predictedPositions",	simArgs.bufPredictedPos.renderResource());
				mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_calcExternalForce, Vec3u{simArgs.particleCount, 1, 1});
			}
		);
	}
}

void 
FluidSim2D_Gpu::addPass_updateSpatialLut(SimArgs& simArgs)
{
}

void 
FluidSim2D_Gpu::_addPass_sortSpatialLut(SimArgs& simArgs)
{
}


void FluidSim2D_Gpu::debug_addPass_renderSpatialLut(SimArgs& simArgs)
{

}

void 
FluidSim2D_Gpu::addPass_calcDensityData(SimArgs& simArgs)
{
}

void 
FluidSim2D_Gpu::addPass_calcViscosity(SimArgs& simArgs)
{
}

void 
FluidSim2D_Gpu::addPass_calcPressureForce(SimArgs& simArgs)
{
}

void 
FluidSim2D_Gpu::addPass_updatePosition(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs2d;

	{
		auto& pass = rdGraph->addPass("fs2d_updatePosition", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.writeBuffer(simArgs.bufPos);
		pass.writeBuffer(simArgs.bufVel);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				RDS_TODO("render graph / RenderGpuBuffer should have a func for upload data directly");
				constCast(simArgs).createOncePositionBuffer();

				mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
				mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_updatePosition, Vec3u{simArgs.particleCount, 1, 1});
			}
		);
	}
}

#endif

}