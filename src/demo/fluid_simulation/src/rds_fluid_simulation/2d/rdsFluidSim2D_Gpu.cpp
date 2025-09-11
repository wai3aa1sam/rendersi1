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
	RenderUtil::createShader(&_shaderSort, "asset/shader/demo/fluid_simulation/common/rdsBitonicMergeSort.shader");
	//RenderUtil::createMaterial(&_shaderSort, &_mtlSort,	"asset/shader/demo/fluid_simulation/common/rdsBitonicMergeSort.shader");

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
				passFluidSim2D.readBuffer(cachedSimArgs_.bufPos);
				passFluidSim2D.readBuffer(cachedSimArgs_.bufVel);
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
					//debug_drawSpatial(rdReq);
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
		passFluidSim2D.readBuffer(_cachedSimArgs.debug_buf_positions);
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

void 
FluidSim2D_Gpu::addPass_simulateFluid2D(SimArgs& simArgs)
{
	Material*	mtl		= _mtlFs2d;
	{
		mtl->setParam("u_dt",						simArgs.dt);
		mtl->setParam("u_gravity",					_simConfig.gravity);
		mtl->setParam("u_gravityDir",				_simConfig.gravityDir.normalize());
		mtl->setParam("u_collisionDamping",			_simConfig.collisionDamping);
		mtl->setParam("u_smoothingRadius",			_simConfig.smoothingRadius);

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
	_addPass_sortSpatialLut(simArgs);

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
	}
}

void 
FluidSim2D_Gpu::_addPass_sortSpatialLut(SimArgs& simArgs)
{
	_mtlSortPool.reset();

	auto*		rdGraph = simArgs.rdGraph;

	// references:
	// - https://github.com/SebLague/Fluid-Sim/blob/Episode-01/Assets/Scripts/Compute%20Helpers/GPU%20Sort/GPUSort.cs
	// Sorts given buffer of integer values using bitonic merge sort
	// Note: buffer size is not restricted to powers of 2 in this implementation
	{
		auto n				= sCast<u32>(simArgs.particleCount);
		auto bufList		= simArgs.bufSpatialLut;
		const char* name	= "fluid_sim";

		// Launch each step of the sorting algorithm (once the previous step is complete)
		// Number of steps = [log2(n) * (log2(n) + 1)] / 2
		// where n = nearest power of 2 that is greater or equal to the number of inputs

		RdgPass* pass_prev = nullptr;
		u32 numStages = sCast<u32>(math::log2(math::nextPow2(n)));
		for (u32 stageIndex = 0; stageIndex < numStages; stageIndex++)
		{
			for (u32 stepIndex = 0; stepIndex < stageIndex + 1; stepIndex++)
			{
				Material* mtl = _mtlSortPool.newObject(_shaderSort);

				auto& pass = rdGraph->addPass(RDS_RDG_EVENT_NAME("bitonicMergeSort_{}_stg{}_stp{}", name, stageIndex, stepIndex)
					, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
				pass.writeBuffer(bufList);
				pass.setExecuteFunc(
					[=](RenderRequest& rdReq)
					{
						// Calculate some pattern stuff
						u32 groupWidth  = 1 << (stageIndex - stepIndex);
						u32 groupHeight = 2 * groupWidth - 1;

						mtl->setParam("u_groupWidth",	groupWidth);
						mtl->setParam("u_groupHeight",	groupHeight);
						mtl->setParam("u_stepIndex",	stepIndex);
						mtl->setParam("u_size",			n);
						mtl->setParam("u_list",			bufList.renderResource());

						rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, Vec3u{sCast<u32>(math::nextPow2(n) / 2), 1, 1});
					}
				);
				pass.runAfter(pass_prev);
				pass_prev = &pass;
			}
		}
	}
}

void FluidSim2D_Gpu::Debug_addPass_debugSpatialLut(SimArgs& simArgs)
{
	auto samplingPt			= this->_mouseRayWorld.origin.toVec2();
	auto smoothingRadius	= _simConfig.smoothingRadius;

	auto*		rdGraph = simArgs.rdGraph;
	{
		Material*	mtl		= _mtlSpatialLutDebug;
		auto& pass = rdGraph->addPass("fs2d_debugSpatialLut", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
		pass.readBuffer(simArgs.bufPredictedPos);
		pass.readBuffer(simArgs.bufSpatialLut);
		pass.readBuffer(simArgs.bufSpatialLutKeyToStartIndex);
		pass.writeBuffer(simArgs.spatialLut_debug_buf_positions);
		//pass.writeBuffer(simArgs.spatialLut_debug_buf_velocities);
		pass.setExecuteFunc(
			[=](RenderRequest& rdReq)
			{
				mtl->setParam("u_samplingPt",		samplingPt);
				mtl->setParam("u_particleCount",	simArgs.particleCount);
				mtl->setParam("u_smoothingRadius",	smoothingRadius);

				mtl->setParam("u_spatialLutDebugPositions",		simArgs.spatialLut_debug_buf_positions.renderResource());
				//mtl->setParam("u_spatialLutDebugVelocities",	simArgs.spatialLut_debug_buf_velocities.renderResource());
				
				mtl->setParam("u_predictedPositions",			simArgs.bufPredictedPos.renderResource());
				mtl->setParam("u_spatialLut",					simArgs.bufSpatialLut.renderResource());
				mtl->setParam("u_spatialLutKeyToStartIndex",	simArgs.bufSpatialLutKeyToStartIndex.renderResource());
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, Vec3u{1, 1, 1});
			}
		);
	}
}

void 
FluidSim2D_Gpu::addPass_calcDensityData(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;

	auto& pass = rdGraph->addPass("fs2d_calcDensityData", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	RDS_TODO("******************remove later, jsut for test");
	pass.readBuffer(simArgs.bufDensityData);
	pass.readBuffer(simArgs.bufPredictedPos);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			
		}
	);
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
				mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
				mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());
				rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, SimArgs::s_kPassIdx_Cs_updatePosition, Vec3u{simArgs.particleCount, 1, 1});
			}
		);
	}
}

#endif

}