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
	RenderUtil::createMaterial(&_shaderSpatialLutDebug, &_mtlSpatialLutDebug,	"asset/shader/demo/fluid_simulation/2d/rdsFluidSim2D_SpatialLutDebug.shader");
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

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	RdgTextureHnd rtColor	= rdGraph->createTexture("fs2d_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("fs2d_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil});

	float dt = 1.0f / 120.0f;
	if (!_cachedSimArgs.hasInit())
		simulate(dt, renderPassPipeline);

	bool hasSimulated = _simState.hasSimulated;

	RdgPass* rdgPass = nullptr;
	auto fnRender = 
		[&](CachedSimArgs& cachedSimArgs_, const Config& config)
		{
			auto& passFluidSim2D = rdGraph->addPass("fluid_sim_2d", RdgPassTypeFlags::Graphics);
			passFluidSim2D.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
			passFluidSim2D.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight

			bool  useCurSimRes = hasSimulated || !cachedSimArgs_.positions;

			auto n = _particleSpawner.particleCount;
			//bool isValid = cachedSimArgs_.bufPos.type() != RdgResourceType::None;
			if (useCurSimRes)
			{
				passFluidSim2D.readBuffer(cachedSimArgs_.bufPos, RenderGpuBufferTypeFlags::Vertex, ShaderStageFlag::Vertex);
				passFluidSim2D.readBuffer(cachedSimArgs_.bufVel, RenderGpuBufferTypeFlags::Vertex, ShaderStageFlag::Vertex);
			}

			passFluidSim2D.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					rdReq.reset(rdGraph->renderContext(), drawData, drawData->lineMaterial());

					auto* clearValue = rdReq.clearFramebuffers();
					clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
					clearValue->setClearDepth(1.0f);

					drawData->setupMaterial(_parentDemo->mtlDrawCircle);
					rdReq.circleMaterial = _parentDemo->mtlDrawCircle;

					debug_drawBoundary(rdReq);
					debug_drawSpatialGrid(rdReq);
					debug_drawSmoothRadius(rdReq);
					debug_drawMouseInteraction(rdReq);

					if (true)
					{
						auto& v = constCast(cachedSimArgs_);
						auto* pos		= useCurSimRes ? v.bufPos.renderResource()					: v.positions.ptr();
						auto* vel		= useCurSimRes ? v.bufVel.renderResource()					: v.velocities.ptr();
						_ptcDisplay.draw(rdReq, drawData, pos, vel, config.particleSize, n);
					}
				}
			);

			drawData->oTexPresent = rtColor;
			rdgPass = &passFluidSim2D;
	};

	#if 1
	SimArgs simArgs;
	simArgs.Debug_create(hasSimulated, _cachedSimArgs, this, dt, rdGraph, drawData);
	Debug_addPass_debugSpatialLut(simArgs);
	fnRender(_cachedSimArgs, _simConfig);

	#if 1
	{
		auto& config = _simConfig;

		if (!config.useDebugSpatial)
			return;

		auto n = _particleSpawner.particleCount;

		auto& passFluidSim2D = rdGraph->addPass("fs2d_SpatialLut_renderDebugPositions", RdgPassTypeFlags::Graphics);
		passFluidSim2D.setRenderTarget(rtColor,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
		passFluidSim2D.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);	// currently use the pre-pass will cause z-flight
		passFluidSim2D.readBuffer(_cachedSimArgs.debug_buf_positions, RenderGpuBufferTypeFlags::Vertex, ShaderStageFlag::Vertex);
		passFluidSim2D.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				rdReq.reset(rdGraph->renderContext(), drawData, drawData->lineMaterial());
				//auto& v = constCast(_cachedSimArgs);
				auto* buf = _cachedSimArgs.debug_buf_positions.renderResource();
				 _ptcDisplay.debug_draw(rdReq, drawData, buf, buf, config.particleSize, n);
			}
		);

		//passFluidSim2D.runAfter(rdgPass);
	}

	#endif // 0

	#endif // 1
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
		mtl->setParam("u_gravityDir",				simConfig.gravityDir.normalize());
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
		mtl->setParam("u_obstacleCenter",			Vec2f{simConfig.obstacle.pos} + Vec2f{simConfig.obstacle.size} / 2.0f);
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
			RDS_TODO("render graph / RenderGpuBuffer should have a func for upload data directly");
			simArgs.createOncePositionBuffer();

			mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
			mtl->setParam("u_predictedPositions",	simArgs.bufPredictedPos.renderResource());
			mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_calcExternalForce, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	//simArgs.pass_calcExternalForce = &pass;
	return pass;
}

RdgPass& 
FluidSim2D_Gpu::addPass_updateSpatialLut(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;

	// fs2d_updateSpatialLut
	{
		Material*	mtl		= _mtlFs2d;

		auto& pass = rdGraph->addPass("fs2d_updateSpatialLut", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.readBuffer(simArgs.bufPredictedPos);
		pass.writeBuffer(simArgs.bufSpatialLut);
		pass.writeBuffer(simArgs.bufSpatialLutKeyToStartIndex);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				mtl->setParam("u_predictedPositions",			simArgs.bufPredictedPos.renderResource());
				mtl->setParam("u_spatialLut",					simArgs.bufSpatialLut.renderResource());
				mtl->setParam("u_spatialLutKeyToStartIndex",	simArgs.bufSpatialLutKeyToStartIndex.renderResource());
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_updateSpatialLut, Vec3u{simArgs.particleCount, 1, 1});
			}
		);
	}

	// sort lut by key
	_gpuSort.sort("fs2d", simArgs.bufSpatialLut, simArgs.particleCount, simArgs.rdGraph);

	RdgPass* pass_updateSpatialLutKeyToStartIndex = nullptr;

	// update key to start index
	{
		Material*	mtl		= _mtlFs2d;

		auto& pass = rdGraph->addPass("fs2d_updateSpatialLutKeyToStartIndex", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.readBuffer(simArgs.bufSpatialLut);
		pass.writeBuffer(simArgs.bufSpatialLutKeyToStartIndex);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				mtl->setParam("u_spatialLut",					simArgs.bufSpatialLut.renderResource());
				mtl->setParam("u_spatialLutKeyToStartIndex",	simArgs.bufSpatialLutKeyToStartIndex.renderResource());
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_updateSpatialLutKeyToStartIndex, Vec3u{simArgs.particleCount, 1, 1});
			}
		);
		pass_updateSpatialLutKeyToStartIndex = &pass;
	}

	return *pass_updateSpatialLutKeyToStartIndex;
}

RdgPass& 
FluidSim2D_Gpu::Debug_addPass_debugSpatialLut(SimArgs& simArgs)
{
	auto	samplingPt		= this->_mouseRayWorld.origin.toVec2();
	auto	smoothingRadius	= _simConfig.smoothingRadius;

	auto*	rdGraph	= simArgs.rdGraph;

	Material*	mtl		= _mtlSpatialLutDebug;
	auto& pass = rdGraph->addPass("fs2d_debugSpatialLut", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.spatialLut_debug_buf_positions);
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_samplingPt",		samplingPt);
			mtl->setParam("u_particleCount",	simArgs.particleCount);
			mtl->setParam("u_smoothingRadius",	smoothingRadius);

			mtl->setParam("u_spatialLutDebugPositions",		simArgs.spatialLut_debug_buf_positions.renderResource());
			simArgs.setSpatialParam(mtl);
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, Vec3u{1, 1, 1});
		}
	);

	return pass;
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
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_calcDensityData, Vec3u{simArgs.particleCount, 1, 1});
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
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_velocities", simArgs.bufVel.renderResource());
			simArgs.setSpatialParam(mtl);
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_calcPressureForce, Vec3u{simArgs.particleCount, 1, 1});
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
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_calcViscosity, Vec3u{simArgs.particleCount, 1, 1});
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
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_updatePosition, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	return pass;
}

#endif

}