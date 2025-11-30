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

	RenderUtil::createMaterial(RDS_DebugLabel(), &_shaderFs2d, &_mtlFs2d,	"asset/shader/demo/fluid_simulation/2d/rdsFluidSim2D.shader");
	_spatialLut.create2D("fs2d");

	_cachedSimArgs.create(_particleSpawner, _particleSpawner.particleCount);
}

void 
FluidSim2D_Gpu::onUpdate(float dt, RenderPassPipeline* renderPassPipeline)
{
	Base::onUpdate(dt, renderPassPipeline);

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

	// must not use isValidRenderResource now (see the code in there), just delay 1 frame for now
	bool hasSimulated = _simState.hasSimulated;
	bool useCurSimRes = hasSimulated || !_cachedSimArgs.positions;

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	RdgTextureHnd rtColor	= rdGraph->createTexture("fs2d_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("fs2d_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil});

	addPass_renderFluidSim2D(_cachedSimArgs, _simConfig, useCurSimRes, rtColor, dsBuf, rdGraph, drawData);
	drawData->oTexPresent = rtColor;

	if (_simConfig.useDebugSpatial)
	{
		auto n = _particleSpawner.particleCount;
		//auto buf = useCurSimRes ? _cachedSimArgs.buf_predictedPos : rdGraph->importBuffer(_cachedSimArgs.predictedPos);
		auto buf = useCurSimRes ? _cachedSimArgs.bufPos : rdGraph->importBuffer(_cachedSimArgs.positions);

		_spatialLut.Debug_updateSpatialLut(useCurSimRes, _mouseRayWorld.origin, _gpuSort, buf, _simConfig.smoothingRadius, n, rdGraph);
		_spatialLut.Debug_renderSpatialLut(_ptcDisplay, rtColor, dsBuf, Vec3f::s_zero(), _simConfig.particleSize, n, rdGraph, drawData);
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
FluidSim2D_Gpu::simulate(float dt, RenderPassPipeline* renderPassPipeline)
{
	//float	dt			= 1 / 120.0f;
	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();

	SimArgs simArgs;
	simArgs.create(this, dt, rdGraph, drawData);
	addPass_simulateFluid2D(simArgs);
}

RdgPass& 
FluidSim2D_Gpu::addPass_simulateFluid2D(SimArgs& simArgs)
{
	Material*	mtl		= _mtlFs2d;
	{
		const auto& simState  = _simState;
		const auto& simConfig = _simConfig;

		mtl->setParam("u_dt",						simArgs.dt);
		mtl->setParam("u_gravity",					simConfig.gravity);
		mtl->setParam("u_gravityDir",				simConfig.gravityDir.toVec2().normalize());
		mtl->setParam("u_collisionDamping",			simConfig.collisionDamping);
		mtl->setParam("u_smoothingRadius",			simConfig.smoothingRadius);

		mtl->setParam("u_targetDensity",			simConfig.targetDensity);
		mtl->setParam("u_pressureMultiplier",		simConfig.pressureMultiplier);
		mtl->setParam("u_nearPressureMultiplier",	simConfig.nearPressureMultiplier);
		mtl->setParam("u_viscosityStrength",		simConfig.viscosityStrength);

		mtl->setParam("u_interactionInputStrength",	simState.interactionInputStrength);
		mtl->setParam("u_interactionInputRadius",	simConfig.interactionRadius);
		mtl->setParam("u_interactionInputPoint",	_mouseRayWorld.origin.toVec2());

		mtl->setParam("u_boundarySize",				simConfig.boundingRegion.size);
		mtl->setParam("u_obstacleCenter",			simConfig.obstacle.center());
		mtl->setParam("u_obstacleSize",				simConfig.obstacle.size);

		mtl->setParam("u_particleMass",				simConfig.particleMass);
		mtl->setParam("u_particleCount",			simArgs.particleCount);

		float smoothingRadius = simConfig.smoothingRadius;
		mtl->setParam("u_poly6ScalingFactor",				4.0f  / (math::PI<float>() * math::pow(smoothingRadius, 8.0f)));
		mtl->setParam("u_spikyPow3ScalingFactor",			10.0f / (math::PI<float>() * math::pow(smoothingRadius, 5.0f)));
		mtl->setParam("u_spikyPow2ScalingFactor",			6.0f  / (math::PI<float>() * math::pow(smoothingRadius, 4.0f)));
		mtl->setParam("u_spikyPow3DerivativeScalingFactor",	30.0f / (math::PI<float>() * math::pow(smoothingRadius, 5.0f)));
		mtl->setParam("u_spikyPow2DerivativeScalingFactor",	12.0f / (math::PI<float>() * math::pow(smoothingRadius, 4.0f)));
	}

	auto& pass_calcExternalForce	= addPass_calcExternalForce(simArgs);
	auto& pass_updateSpatialLut		= addPass_updateSpatialLut(simArgs);
	auto& pass_calcDensityData		= addPass_calcDensityData(simArgs);
	auto& pass_calcPressureForce	= addPass_calcPressureForce(simArgs);
	auto& pass_calcViscosity		= addPass_calcViscosity(simArgs);
	auto& pass_updatePosition		= addPass_updatePosition(simArgs);

	// force dependency
	pass_updateSpatialLut.runAfter(		&pass_calcExternalForce);
	pass_calcDensityData.runAfter(		&pass_updateSpatialLut);
	pass_calcPressureForce.runAfter(	&pass_calcDensityData);
	pass_calcViscosity.runAfter(		&pass_calcPressureForce);
	pass_updatePosition.runAfter(		&pass_calcViscosity);

	return pass_updatePosition;
}

RdgPass& 
FluidSim2D_Gpu::addPass_renderFluidSim2D(CachedSimArgs& cachedSimArgs, const Config& simConfig, bool useCurSimRes, RdgTextureHnd rtColor, RdgTextureHnd dsBuf, RenderGraph* rdGraph, DrawData* drawData)
{
	auto n = _particleSpawner.particleCount;

	auto& pass = rdGraph->addPass("fluid_sim_2d", RdgPassTypeFlags::Graphics);
	pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	pass.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight

	#if 1
	if (useCurSimRes)
	{
		pass.readBuffer(cachedSimArgs.bufPos, RenderGpuBufferTypeFlags::Vertex, ShaderStageFlag::Vertex);
		pass.readBuffer(cachedSimArgs.bufVel, RenderGpuBufferTypeFlags::Vertex, ShaderStageFlag::Vertex);
	}
	#endif // 0

	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData);

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			debug_drawBoundary(rdReq);
			debug_drawSpatialGrid(rdReq);
			debug_drawSmoothRadius(rdReq);
			debug_drawMouseInteraction(rdReq);

			if (1)
			{
				auto& v = constCast(cachedSimArgs);
				// must not use isValidRenderResource now (see the code in there)
				//auto* pos		= v.bufPos.isValidRenderResource() ? v.bufPos.renderResource() : v.positions.ptr();
				//auto* vel		= v.bufVel.isValidRenderResource() ? v.bufVel.renderResource() : v.velocities.ptr();

				auto* pos		= useCurSimRes ? v.bufPos.renderResource() : v.positions.ptr();
				auto* vel		= useCurSimRes ? v.bufVel.renderResource() : v.velocities.ptr();
				_ptcDisplay.draw(rdReq, drawData, pos, vel, Vec3f::s_zero(), _simConfig.particleSize, n);
			}
		}
	);

	return pass;
}

RdgPass&
FluidSim2D_Gpu::addPass_calcExternalForce(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs2d;

	auto& pass = rdGraph->addPass("fs2d_calcExternalForce", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufVel);
	pass.writeBuffer(simArgs.bufPredictedPos);
	pass.readBuffer(simArgs.bufPos);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
			mtl->setParam("u_predictedPositions",	simArgs.bufPredictedPos.renderResource());
			mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());
			rdReq.dispatchExactThreadGroups(RDS_DebugLabel(), mtl, SimArgs::s_kPassIdx_Cs_calcExternalForce, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	//simArgs.pass_calcExternalForce = &pass;
	return pass;
}

RdgPass& 
FluidSim2D_Gpu::addPass_updateSpatialLut(SimArgs& simArgs)
{
	auto* rdGraph = simArgs.rdGraph;
	return _spatialLut.updateSpatialLut(_gpuSort, simArgs.bufPredictedPos, _simConfig.smoothingRadius, simArgs.particleCount, rdGraph);
}

RdgPass& 
FluidSim2D_Gpu::addPass_calcDensityData(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs2d;

	auto& pass = rdGraph->addPass("fs2d_calcDensityData", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufDensityData);
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_densityData",					simArgs.bufDensityData.renderResource());
			simArgs.setSpatialParam(mtl);
			rdReq.dispatchExactThreadGroups(RDS_DebugLabel(), mtl, SimArgs::s_kPassIdx_Cs_calcDensityData, Vec3u{simArgs.particleCount, 1, 1});
		}
	);
	return pass;
}

RdgPass& 
FluidSim2D_Gpu::addPass_calcPressureForce(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs2d;

	auto& pass = rdGraph->addPass("fs2d_calcPressureForce", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufVel);
	pass.readBuffer(simArgs.bufDensityData);
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_velocities", simArgs.bufVel.renderResource());
			simArgs.setSpatialParam(mtl);
			rdReq.dispatchExactThreadGroups(RDS_DebugLabel(), mtl, SimArgs::s_kPassIdx_Cs_calcPressureForce, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	return pass;
}

RdgPass& 
FluidSim2D_Gpu::addPass_calcViscosity(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs2d;

	auto& pass = rdGraph->addPass("fs2d_calcViscosity", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufVel);
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_velocities", simArgs.bufVel.renderResource());
			simArgs.setSpatialParam(mtl);
			rdReq.dispatchExactThreadGroups(RDS_DebugLabel(), mtl, SimArgs::s_kPassIdx_Cs_calcViscosity, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	return pass;
}

RdgPass& 
FluidSim2D_Gpu::addPass_updatePosition(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs2d;

	auto& pass = rdGraph->addPass("fs2d_updatePosition", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufPos);
	pass.writeBuffer(simArgs.bufVel);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
			mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());
			rdReq.dispatchExactThreadGroups(RDS_DebugLabel(), mtl, SimArgs::s_kPassIdx_Cs_updatePosition, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	return pass;
}

#endif

}