#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim3D_Gpu.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim3D_Gpu-Impl ---
#endif // 0
#if 1

void 
FluidSim3D_Gpu::onCreate(GraphicsDemo* parentDemo)
{
	Base::onCreate(parentDemo);

	RenderUtil::createMaterial(&_shaderFs3d, &_mtlFs3d,	"asset/shader/demo/fluid_simulation/3d/rdsFluidSim3D.shader");
	_spatialLut.create3D("fs3d");

	_cachedSimArgs.create(_particleSpawner, _particleSpawner.particleCount);

	_voxelFluid = makeUPtr<VoxelFluid>();
	_voxelFluid->create();
}

void 
FluidSim3D_Gpu::onUpdate(float dt, RenderPassPipeline* renderPassPipeline)
{
	Base::onUpdate(dt, renderPassPipeline);

}

void 
FluidSim3D_Gpu::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);

}

void 
FluidSim3D_Gpu::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	// must not use isValidRenderResource now (see the code in there), just delay 1 frame for now
	bool hasSimulated = _simState.hasSimulated;
	bool useCurSimRes = hasSimulated || !_cachedSimArgs.positions;

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	RdgTextureHnd rtColor	= rdGraph->createTexture("fs3d_rtColor",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAh, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("fs3d_dsBuf",		Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil});

	addPass_renderFluidSim3D(_cachedSimArgs, _simConfig, useCurSimRes, rtColor, dsBuf, rdGraph, drawData);

	if (_debug.useVoxel)
	{
		auto n = _particleSpawner.particleCount;
		auto buf = useCurSimRes ? _cachedSimArgs.buf_predictedPos : rdGraph->importBuffer(_cachedSimArgs.predictedPos);

		VoxelFluid::PassArgs passArgs;
		passArgs.create(*_voxelFluid, _cachedSimArgs, &_spatialLut, useCurSimRes, rtColor, dsBuf
			, getBoundingBoxTransform(), _voxelFluid->voxelMapResolution
			, _particleSpawner.particleCount, _simConfig.smoothingRadius
			, _ptcDisplay.colorGradientTexture(), (float)_simConfig.maxValue
			, rdGraph, drawData);

		if (!useCurSimRes)
		{
			_spatialLut.updateSpatialLut(_gpuSort, buf, _simConfig.smoothingRadius, n, rdGraph);
		}
		auto* pass_particleToVoxelMap	= _voxelFluid->addPass_particleToTex3D(	passArgs); RDS_UNUSED(pass_particleToVoxelMap);
		auto& pass_renderVoxelMap		= _voxelFluid->addPass_renderVoxelMap(	passArgs); RDS_UNUSED(pass_renderVoxelMap);
	}

	drawData->oTexPresent = rtColor;

	// debug spatia lut
	#if 1
	if (_simConfig.useDebugSpatial)
	{
		auto n = _particleSpawner.particleCount;
		//auto buf = useCurSimRes ? _cachedSimArgs.buf_predictedPos : rdGraph->importBuffer(_cachedSimArgs.predictedPos);
		auto buf = useCurSimRes ? _cachedSimArgs.bufPos : rdGraph->importBuffer(_cachedSimArgs.positions);

		_spatialLut.Debug_updateSpatialLut(useCurSimRes, getDebugSpatialTransform()->localPosition(), _gpuSort, buf, _simConfig.smoothingRadius, n, rdGraph);
		_spatialLut.Debug_renderSpatialLut(_ptcDisplay, rtColor, dsBuf, getBoundingBoxTransform()->localPosition(), _simConfig.particleSize, n, rdGraph, drawData);
	}
	#endif // 0
}

void 
FluidSim3D_Gpu::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	Base::onDrawGui(uiDrawReq);

	//uiDrawReq.dragInt(	"voxelMapResolution",	&_voxelFluid->voxelMapResolution, 1, 1, 128);
	//uiDrawReq.dragFloat("voxelScale",			&_voxelFluid->voxelScale, 0.005f, 0.0001f);

	uiDrawReq.showText("voxel fluid simulation demo");
	uiDrawReq.showText("camera control (like unreal): keyboard: WASD, mouse pan / orbit");
	uiDrawReq.showText("press Space: start / stop simulation");
	uiDrawReq.showText("press Q: step backward");
	uiDrawReq.showText("press E: step forward");
	uiDrawReq.makeCheckbox("useVoxel", &_debug.useVoxel);
}

void 
FluidSim3D_Gpu::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);

}

void 
FluidSim3D_Gpu::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);

}

void 
FluidSim3D_Gpu::update(float dt)
{

}

void 
FluidSim3D_Gpu::simulate(float dt, RenderPassPipeline* renderPassPipeline)
{
	//float	dt			= 1 / 120.0f;
	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	
	RDS_TODO("each sim step should have its own material, otherwise it will be override, but safe in this case, all param is same");
	SimArgs simArgs;
	simArgs.create(this, dt, rdGraph, drawData);
	RdgPass* prevPass = nullptr;
	//for (size_t i = 0; i < _simConfig.simulationCountPerFrame; i++)
	{
		auto& pass = addPass_simulateFluid3D(simArgs);
		pass.runAfter(prevPass);
		prevPass = &pass;
	}

}

RdgPass& 
FluidSim3D_Gpu::addPass_simulateFluid3D(SimArgs& simArgs)
{
	Material*	mtl		= _mtlFs3d;
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
		mtl->setParam("u_interactionInputPoint",	getInteractionTransform()->localPosition());

		mtl->setParam("u_forceFieldPos",			getForceFieldTransform()->localPosition());
		mtl->setParam("u_forceFieldScale",			getForceFieldTransform()->localScale());
		mtl->setParam("u_forceFieldDir",			simConfig.forceFieldDir.normalize());
		mtl->setParam("u_forceFieldStrength",		simConfig.forceFieldStrength);

		mtl->setParam("u_boundarySize",				getBoundingBoxTransform()->localScale());
		//mtl->setParam("u_obstacleCenter",			AABBox3T_center(simConfig.obstacle3D));
		//mtl->setParam("u_obstacleSize",				AABBox3T_size(simConfig.obstacle3D));

		mtl->setParam("u_particleMass",				simConfig.particleMass);
		mtl->setParam("u_particleCount",			simArgs.particleCount);

		auto mat_world = getBoundingBoxTransform()->worldMatrix();
		mtl->setParam("u_objToWorld",				mat_world);
		mtl->setParam("u_worldToObj",				mat_world.inverse());

		//float smoothingRadius = simConfig.smoothingRadius;
		//mtl->setParam("u_poly6ScalingFactor",				4.0f  / (math::PI<float>() * math::pow(smoothingRadius, 8.0f)));
		//mtl->setParam("u_spikyPow3ScalingFactor",			10.0f / (math::PI<float>() * math::pow(smoothingRadius, 5.0f)));
		//mtl->setParam("u_spikyPow2ScalingFactor",			6.0f  / (math::PI<float>() * math::pow(smoothingRadius, 4.0f)));
		//mtl->setParam("u_spikyPow3DerivativeScalingFactor",	30.0f / (math::PI<float>() * math::pow(smoothingRadius, 5.0f)));
		//mtl->setParam("u_spikyPow2DerivativeScalingFactor",	12.0f / (math::PI<float>() * math::pow(smoothingRadius, 4.0f)));
	}

	auto& pass_calcExternalForce	= addPass_calcExternalForce(simArgs);		RDS_UNUSED(pass_calcExternalForce	);
	auto& pass_updateSpatialLut		= addPass_updateSpatialLut(simArgs);		RDS_UNUSED(pass_updateSpatialLut	);
	auto& pass_calcDensityData		= addPass_calcDensityData(simArgs);			RDS_UNUSED(pass_calcDensityData		);
	auto& pass_calcPressureForce	= addPass_calcPressureForce(simArgs);		RDS_UNUSED(pass_calcPressureForce	);
	auto& pass_calcViscosity		= addPass_calcViscosity(simArgs);			RDS_UNUSED(pass_calcViscosity		);
	auto& pass_updatePosition		= addPass_updatePosition(simArgs);			RDS_UNUSED(pass_updatePosition		);
	
	// debug
	#if 0
	auto& pass = simArgs.rdGraph->addPass("fs3d_debug", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.readBuffer(simArgs.bufPos);
	pass.readBuffer(simArgs.bufVel);
	pass.readBuffer(simArgs.bufDensityData);
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
			mtl->setParam("u_predictedPositions",	simArgs.bufPredictedPos.renderResource());
			mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());
			mtl->setParam("u_densityData",			simArgs.bufDensityData.renderResource());
			simArgs.setSpatialParam(mtl);
		}
	);
	#endif // 1

	return pass_updatePosition;
}

RdgPass& 
FluidSim3D_Gpu::addPass_renderFluidSim3D(CachedSimArgs& cachedSimArgs, const Config& simConfig, bool useCurSimRes, RdgTextureHnd rtColor, RdgTextureHnd dsBuf, RenderGraph* rdGraph, DrawData* drawData)
{
	auto n = _particleSpawner.particleCount;

	auto& pass = rdGraph->addPass("fluid_sim_3d", RdgPassTypeFlags::Graphics);
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
			rdReq.reset(rdGraph->renderContext(), drawData, drawData->lineMaterial());

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			if (!_debug.useVoxel)
			{
				auto& v = constCast(cachedSimArgs);
				// must not use isValidRenderResource now (see the code in there)
				//auto* pos		= v.bufPos.isValidRenderResource() ? v.bufPos.renderResource() : v.positions.ptr();
				//auto* vel		= v.bufVel.isValidRenderResource() ? v.bufVel.renderResource() : v.velocities.ptr();

				auto* pos		= useCurSimRes ? v.bufPos.renderResource() : v.positions.ptr();
				auto* vel		= useCurSimRes ? v.bufVel.renderResource() : v.velocities.ptr();
				_ptcDisplay.draw(rdReq, drawData, pos, vel, getBoundingBoxTransform()->localPosition(), _simConfig.particleSize, n);
			}

			drawData->drawScene(rdReq);
		}
	);

	return pass;
}

RdgPass&
FluidSim3D_Gpu::addPass_calcExternalForce(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs3d;

	auto& pass = rdGraph->addPass("fs3d_calcExternalForce", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufVel);
	pass.writeBuffer(simArgs.bufPredictedPos);
	pass.readBuffer(simArgs.bufPos);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
			mtl->setParam("u_predictedPositions",	simArgs.bufPredictedPos.renderResource());
			mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());

			static auto passId = mtl->makeCsPassId("Cs_calcExternalForce");
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, passId, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	//simArgs.pass_calcExternalForce = &pass;
	return pass;
}

RdgPass& 
FluidSim3D_Gpu::addPass_updateSpatialLut(SimArgs& simArgs)
{
	auto* rdGraph = simArgs.rdGraph;
	return _spatialLut.updateSpatialLut(_gpuSort, simArgs.bufPredictedPos, _simConfig.smoothingRadius, simArgs.particleCount, rdGraph);
}

RdgPass& 
FluidSim3D_Gpu::addPass_calcDensityData(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs3d;

	auto& pass = rdGraph->addPass("fs3d_calcDensityData", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufDensityData);
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_densityData",					simArgs.bufDensityData.renderResource());
			simArgs.setSpatialParam(mtl);

			static auto passId = mtl->makeCsPassId("Cs_calcDensityData");
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, passId, Vec3u{simArgs.particleCount, 1, 1});
		}
	);
	return pass;
}

RdgPass& 
FluidSim3D_Gpu::addPass_calcPressureForce(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs3d;

	auto& pass = rdGraph->addPass("fs3d_calcPressureForce", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufVel);
	pass.readBuffer(simArgs.bufDensityData);
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_velocities", simArgs.bufVel.renderResource());
			simArgs.setSpatialParam(mtl);

			static auto passId = mtl->makeCsPassId("Cs_calcPressureForce");
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, passId, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	return pass;
}

RdgPass& 
FluidSim3D_Gpu::addPass_calcViscosity(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs3d;

	auto& pass = rdGraph->addPass("fs3d_calcViscosity", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufVel);
	simArgs.readSpatialBuffer(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_velocities", simArgs.bufVel.renderResource());
			simArgs.setSpatialParam(mtl);

			static auto passId = mtl->makeCsPassId("Cs_calcViscosity");
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, passId, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	return pass;
}

RdgPass& 
FluidSim3D_Gpu::addPass_updatePosition(SimArgs& simArgs)
{
	auto*		rdGraph = simArgs.rdGraph;
	Material*	mtl		= _mtlFs3d;

	auto& pass = rdGraph->addPass("fs3d_updatePosition", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(simArgs.bufPos);
	pass.writeBuffer(simArgs.bufVel);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_positions",			simArgs.bufPos.renderResource());
			mtl->setParam("u_velocities",			simArgs.bufVel.renderResource());

			static auto passId = mtl->makeCsPassId("Cs_updatePosition");
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, passId, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	return pass;
}

#endif

RdgPass* 
VoxelFluid::addPass_particleToTex3D(PassArgs& passArgs)
{
	// if not simulated, all buf hnd is invalid

	auto*		rdGraph = passArgs.rdGraph;
	Material*	mtl		= _mtl_particleToVoxelMap;

	auto& pass = rdGraph->addPass("voxel_particleToTex3D", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeTexture(passArgs.tex_voxelMap);
	//pass.readBuffer(passArgs.buf_positions);
	pass.readBuffer(passArgs.buf_densityData);
	passArgs.spatialLut->readBuffers(pass);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setImage("u_tex3D",				passArgs.tex_voxelMap.renderResource(), 0);
			mtl->setParam("u_densityData",			passArgs.buf_densityData.renderResource());
			passArgs.spatialLut->setBuffersToMaterial(mtl, "u_positions");

			mtl->setParam("u_particleCount",		passArgs.particleCount);
			mtl->setParam("u_smoothingRadius",		passArgs.smoothingRadius);
			mtl->setParam("u_boundingSize",			passArgs.boundingBoxTransform->localScale());
			mtl->setParam("u_voxelMapSize",			passArgs.voxelMapSize);

			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 0, Vec3u::s_one() * passArgs.voxelMapSize);
			//rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 0, Vec3u{simArgs.particleCount, 1, 1});
		}
	);

	return &pass;
}

RdgPass& 
VoxelFluid::addPass_renderVoxelMap(PassArgs& passArgs)
{
	auto*		rdGraph		= passArgs.rdGraph;
	auto*		drawData	= passArgs.drawData;
	Material*	mtl			= _mtl_renderVoxelMap;

	auto& pass = rdGraph->addPass("voxel_renderVoxelParticle", RdgPassTypeFlags::Graphics);
	pass.readTexture(passArgs.tex_voxelMap, TextureUsageFlags::ShaderResource, ShaderStageFlag::Pixel);

	pass.setRenderTarget(passArgs.rtColor,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
	pass.setDepthStencil(passArgs.dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData);
			drawData->setupMaterial(mtl);

			auto voxelMapSize = passArgs.voxelMapSize;
			mtl->setParam("u_voxelMap",				passArgs.tex_voxelMap.renderResource());
			mtl->setParam("u_voxelMapSize",			voxelMapSize);
			mtl->setParam("u_voxelScale",			voxelScale);
			mtl->setParam("u_boundingPos",			passArgs.boundingBoxTransform->localPosition());

			mtl->setParam("u_colorMap",				passArgs.colorMap);
			mtl->setParam("u_densityMax",			passArgs.densityMax);

			auto drawCall = rdReq.addDrawCall(sizeof(PerObjectParam));
			drawCall->setDebugSrcLoc(RDS_SRCLOC);
			drawCall->renderPrimitiveType = RenderPrimitiveType::Point;
			drawCall->vertexCount = voxelMapSize.x * voxelMapSize.y * voxelMapSize.z;
			drawCall->setMaterial(mtl);

			/*PerObjectParam objParam;
			objParam.id = sCast<decltype(PerObjectParam::id)>(s_entVctVoxelVisualizationId);
			drawCall->setExtraData(objParam);*/
		}
	);
	return pass;
}
}