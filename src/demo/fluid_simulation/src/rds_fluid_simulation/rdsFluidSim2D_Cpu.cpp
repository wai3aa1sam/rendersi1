#include "rds_fluid_simulation-pch.h"
#include "rdsFluidSim2D_Cpu.h"
#include "rds_editor/ui/property/rdsEditorPropertyDrawer.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_Cpu-Impl ---
#endif // 0
#if 1

void 
FluidSim2D_Cpu::onCreate(GraphicsDemo* parentDemo)
{
	Base::onCreate(parentDemo);

	_simConfig.create(this);
	_particleSpawner.spawnRegion = _simConfig.spawnRegion;
	//_particleSpawner.spawnRegion = _simConfig.boundingRegion;

	_particleSpawner.spawnTo(_positions, _predictedPositions, _velocities, _densities, _densityData);
	_spatialLut.resize(_positions.size());
	_cellKeyStartIndices.resize(_positions.size());

	_simConfig.debugParticleCount = (u32)_positions.size();
	_ptcDisplay.create2D(_simConfig.makeColorGradient());

	auto& camera = parentDemo->app().mainWindow().camera();
	camera.setOrthographic(4.0f);
	camera.setPos(Vec3f{0.385f, 11.446f, 22.212f});
	camera.setAim(Vec3f{0.385f, 5.10f, 0.0f});

	camera.setPos(Vec3f{0.0f, 0.0f, 8.0f});
	camera.setAim(Vec3f{0.0f, 0.0f, 0.0f});
}

void 
FluidSim2D_Cpu::onUpdate(float dt)
{
	Base::onUpdate(dt);

	update(dt);
}

void 
FluidSim2D_Cpu::onPrepareRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onPrepareRender(renderPassPipeline);
}

void 
FluidSim2D_Cpu::onExecuteRender(RenderPassPipeline* renderPassPipeline)
{
	Base::onExecuteRender(renderPassPipeline);

	auto*	rdGraph		= renderPassPipeline->renderGraph();
	auto*	drawData	= renderPassPipeline->drawDataT<DrawData>();
	auto	screenSize	= drawData->resolution2u();

	RdgTextureHnd rtColor	= rdGraph->createTexture("fluid_simulation_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("fluid_simulation_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passFluidSim2D_Cpu = rdGraph->addPass("fluid_simulation", RdgPassTypeFlags::Graphics);
	passFluidSim2D_Cpu.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passFluidSim2D_Cpu.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
	passFluidSim2D_Cpu.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData, drawData->lineMaterial());

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			//drawData->drawScene(rdReq, mtl);
			drawData->setupMaterial(_parentDemo->mtlDrawCircle);
			rdReq.circleMaterial = _parentDemo->mtlDrawCircle;

			#if 0
			for (auto& e : _positions)
			{
				rdReq.drawCircle(e, _simConfig.particleSize, _simConfig.particleColor);
			}
			#endif // 0

			RDS_TODO("later put on debug, also, test orthographic mode");
			{
				#if 0
				if (_parentDemo)
				{
					static Ray3f ray;
					if (_parentDemo->uiMouseState.isDown(UiMouseEventButton::Left))
					{
						ray = _mouseRayWorld;
					}
					rdReq.drawLine(ray.origin, ray.origin + ray.dir * 9999.0f, Color4f(1.0f, 0.0f, 0.0f, 1.0f));
					rdReq.drawCircle(ray.origin.toVec2(), _simConfig.smoothingRadius, Color4f(0.2f, 0.0f, 0.0f, 0.005f));
				}
				#endif // 0

				if (_simState.isPullInteraction)
				{
					rdReq.drawCircle(_mouseRayWorld.origin.toVec2(), _simConfig.interactionRadius, Color4f(0.2f, 0.8f, 0.2f, 0.005f));
				}
				if (_simState.isPushInteraction)
				{
					rdReq.drawCircle(_mouseRayWorld.origin.toVec2(), _simConfig.interactionRadius, Color4f(0.2f, 0.2f, 0.8f, 0.005f));
				}
				
				rdReq.drawAABBox(Vec3f{ _simConfig.boundingRegion.pos, 0.0f }, Vec3f{_simConfig.boundingRegion.size, 0.001f} / 2.0f);
			}

			{
				Color4f color = Color4f(0.2f, 0.6f, 0.2f, 1.0f);

				auto gridCellCount = Vec2f{_simConfig.boundingRegion.size} / _simConfig.smoothingRadius;
				gridCellCount.x = math::ceil(gridCellCount.x);
				gridCellCount.y = math::ceil(gridCellCount.y);

				const auto& region = _simConfig.boundingRegion;
				auto cellSize = Vec2f{_simConfig.boundingRegion.size} / gridCellCount;

				auto minExtent = Vec2f{ region.pos } - Vec2f{ region.size } / 2.0f -cellSize / 2.0f;
				for (int y = -2; y < gridCellCount.y + 2; y++)
				{
					float posY = minExtent.y + cellSize.y * y;

					auto yLinePt0 = Vec3f{ minExtent.x, posY, 0.0f };
					auto yLinePt1 = yLinePt0 + Vec3f::s_right() * region.w * 2.0f;
					rdReq.drawLine(yLinePt0, yLinePt1, color);

					for (int x = -2; x < gridCellCount.x + 2; x++)
					{
						float posX = minExtent.x + cellSize.x * x;

						auto xLinePt0 = Vec3f{ posX, minExtent.y, 0.0f };
						auto xLinePt1 = xLinePt0 + Vec3f::s_up() * region.h * 2.0f;
						rdReq.drawLine(xLinePt0, xLinePt1, color);
					}
				}
			}

			if (_simConfig.useDebugSpatial)
			{
				auto count = 0;
				auto fn =
					[&](const NeighbourInfo& info)
					{
						count++;
						rdReq.drawCircle(_positions[info.index], _simConfig.particleSize, Color4f(1.0f, 1.0f, 1.0f, 1.0f));
					};
				debugForeachPointWithinRadius(_mouseRayWorld.origin.toVec2(), _simConfig.smoothingRadius, false, fn);
				_simConfig.debugWithinRadiusCount = count;
			}

			_ptcDisplay.draw(rdReq, drawData, _positions, _velocities, _simConfig.particleSize);
		}
	);

	drawData->oTexPresent = rtColor;
	
	//RdgTextureHnd rtTex;
	//_parentDemo->addPass_DebugTexture(_ptcDisplay.colorGradientTexture(), &rtTex);
	//drawData->oTexPresent = rtTex;
}

void 
FluidSim2D_Cpu::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	_simConfig.drawGui(uiDrawReq);

}

void 
FluidSim2D_Cpu::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);

	_simState.isPullInteraction = isFocusOnEditorViewport() && ev.isDown(UiMouseEventButton::Left);
	_simState.isPushInteraction = isFocusOnEditorViewport() && ev.isDown(UiMouseEventButton::Right);
}

void 
FluidSim2D_Cpu::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);

	if (ev.isUp(UiKeyboardEventButton::Space))
	{
		_simState.isStop = !_simState.isStop;
	}

	if (_simState.isStop)
	{
		if (ev.isUp(UiKeyboardEventButton::E))
		{
			_simState.isStepForward = !_simState.isStepForward;
		}

		if (ev.isUp(UiKeyboardEventButton::Q))
		{
			_simState.isStepBackward = !_simState.isStepBackward;
		}
	}
}

ParticleDisplay& 
FluidSim2D_Cpu::particleDisplay()
{
	return _ptcDisplay;
}

#if 1

void 
FluidSim2D_Cpu::update(float dt)
{
	if (_simConfig.isInvalidateColorMap)
	{
		ColorGradient colGrad = _simConfig.makeColorGradient();
		_ptcDisplay.invalidateColorGradient(colGrad);
		_simConfig.isInvalidateColorMap = false;
	}

	RDS_CALL_ONCE(
		updateSpatialLut(_positions, _simConfig.smoothingRadius); 
		logDebugSpatial(); 
		return; 
	);

	if (!_simState.isStop)
	{
		simulate(dt);
		logDebugSpatial();
	}
	else
	{
		auto newDt = dt;
		if (_simState.isStepBackward) newDt = -newDt;

		bool shdSim = _simState.isStepForward || _simState.isStepBackward;
		if (shdSim) simulate(newDt);

		_simState.isStepForward		= false;
		_simState.isStepBackward	= false;

		if (shdSim)
		{
			logDebugSpatial();
		}
	}

	_simConfig.debugMousePosViewport = _mousePosViewport;
	_simConfig.debugMousePosWorld	 = _mouseRayWorld.origin;
	_simConfig.debugMouseDirWorld	 = _mouseRayWorld.dir;
}

void 
FluidSim2D_Cpu::logDebugSpatial()
{
	if (!_simConfig.useDebugLog)
	{
		return;
	}

	RDS_LOG("----- logDebugSpatial:");

	RDS_LOG("_positions:");
	for (size_t i = 0; i < _positions.size(); i++)
	{
		RDS_LOG("pos[{}]: {}", i, _positions[i]);
	}

	RDS_LOG("_spatialLut:");
	for (size_t i = 0; i < _spatialLut.size(); i++)
	{
		RDS_LOG("particle_i: {}, cellKey: {}", _spatialLut[i].x, _spatialLut[i].y);
	}

	RDS_LOG("_cellKeyStartIndices:");
	for (size_t i = 0; i < _cellKeyStartIndices.size(); i++)
	{
		RDS_LOG("_cellKeyStartIndices[{}]: {}", i, _cellKeyStartIndices[i]);
	}

	RDS_LOG("_debugCellCoords:");
	for (size_t i = 0; i < _positions.size(); i++)
	{
		auto coord = positionToCellCoord(_positions[i], _simConfig.smoothingRadius);
		RDS_LOG("_pos[{}]: {}, cellKey: {}, _cellCoords: {}, vel: {}, _densities: {}"
			, i, _positions[i], calcCellKeyByCellCoord(coord), coord
			, _velocities[i], _densities[i]);
	}

	RDS_LOG("----- logDebugSpatial --- end");
}

void
FluidSim2D_Cpu::simulate(float dt)
{
	dt *= _simConfig.timeMultiplier;

	{
		float interactionStrength = 0.0f;
		if (_simState.isPullInteraction) interactionStrength += _simConfig.interactionStrength;
		if (_simState.isPushInteraction) interactionStrength -= _simConfig.interactionStrength;

		for (size_t i = 0; i < _positions.size(); i++)
		{
			auto accel = calcExternalForce(_mouseRayWorld.origin.toVec2(), _simConfig.interactionRadius, interactionStrength, i);
			_velocities[i] += accel * dt;
		}
	}
	
	for (size_t i = 0; i < _positions.size(); i++)
	{
		_velocities[i]			+= _simConfig.gravityDir * _simConfig.gravity * dt;
		_predictedPositions[i]	 = _positions[i] + _velocities[i] * dt;	// reduce chaos when start
	}

	if (_simConfig.useSpatialOptimization)
		updateSpatialLut(_predictedPositions, _simConfig.smoothingRadius);

	#define RDS_USE_DENSITY_DATA 1

	for (size_t i = 0; i < _positions.size(); i++)
	{
		#if RDS_USE_DENSITY_DATA
		_densityData[i]	= calcDensityData(i);
		#else
		_densities[i]	= calcDensity(i);
		#endif // RDS_USE_DENSITY_DATA
	}
	
	for (size_t i = 0; i < _positions.size(); i++)
	{
		auto viscosityForce = calcViscosityForce(i);
		_velocities[i] += viscosityForce * dt;
	}

	for (size_t i = 0; i < _positions.size(); i++)
	{
		#if RDS_USE_DENSITY_DATA
		auto pressureForce	= calcPressureForceByDensityData(i);
		auto pressureAccel	= pressureForce / _densityData[i].x;
		#else
		auto pressureForce	= calcPressureForce(i);
		auto pressureAccel	= pressureForce / _densities[i];
		#endif // RDS_USE_DENSITY_DATA

		_velocities[i] += pressureAccel * dt;
		//_velocities[i] = pressureAccel * dt;		// no inertia for debug

		#if 0
		// sp_debug
		if (_velocities[i].magnitude() > 1000)
		{
			RDS_LOG("error vel");
		}
		#endif // 0
	}

	for (size_t i = 0; i < _positions.size(); i++)
	{
		_positions[i] += _velocities[i] * dt;
		resolveCollisions(_positions[i], _velocities[i]);
	}

	#undef RDS_USE_DENSITY_DATA
}

void 
FluidSim2D_Cpu::resolveCollisions(DimT& outPos, DimT& outVel)
{
	auto pos = outPos;
	auto collisionDamping = _simConfig.collisionDamping;

	// Keep particle inside bounds
	const Vec2f halfSize = Vec2f{_simConfig.boundingRegion.size} * 0.5;
	Vec2f edgeDist = halfSize - Vec2f{abs(pos.x), abs(pos.y)};

	auto sign = [](float v) { return v < 0.0f ? -1.0f : +1.0f; };
	if (edgeDist.x <= 0)
	{
		outPos.x = halfSize.x * sign(pos.x);
		outVel.x *= -1 * collisionDamping;
	}
	if (edgeDist.y <= 0)
	{
		outPos.y = halfSize.y * sign(pos.y);
		outVel.y *= -1 * collisionDamping;
	}
}

float 
FluidSim2D_Cpu::smoothingKernel(float radius, float dist)
{
	#if 0
	// volume by integration shell method: (r^2 - d^2) * d
	float volume = math::PI<float>() * math::pow(radius, 8.0f) / 4.0f;	
	float v = math::max(0.0f, radius * radius - dist * dist);
	return (v * v * v) / volume;

	#else

	// same but more spiky, like a triangle, solve the particle stick together
	// close particle will experience high value
	if (dist >= radius) return 0;
	float volume = math::PI<float>() * math::pow(radius, 4.0f) / 6.0f;	
	float v = radius - dist;
	return (v * v) / volume;
	#endif // 0
}

float 
FluidSim2D_Cpu::smoothingKernelDerivative(float radius, float dist)
{
	#if 0
	// f' = smoothingKernel
	// f' ( (r^2 - d^2) ^ 3 / (pi * r^8 / 4) )
	if (dist >= radius) return 0.0f;
	float f = radius * radius - dist * dist;
	float scale = -24.0f / (math::PI<float>() * math::pow(radius, 8.0f));
	return scale * dist * f * f;
	#else

	if (dist >= radius) return 0.0f;
	float f = dist - radius;
	float scale = 12.0f / (math::PI<float>() * math::pow(radius, 4.0f));
	return scale * f;
	#endif
}

float 
FluidSim2D_Cpu::smoothingKernelPoly6(float radius, float dist)
{
	if (dist < radius)
	{
		float factor = 4.0f / (math::PI<float>() * math::pow(radius, 8.0f));
		float v = radius * radius - dist * dist;
		return v * v * v * factor;
	}
	return 0;
}

float 
FluidSim2D_Cpu::spikyKernelPow3(float radius, float dist)
{
	if (dist < radius)
	{
		float factor = 10.0f / (math::pow(radius, 5.0f) * math::PI<float>());
		float v = radius - dist;
		return v * v * v * factor;
	}
	return 0;
}

float 
FluidSim2D_Cpu::spikyKernelPow3Derivative(float radius, float dist)
{
	if (dist <= radius)
	{
		float factor = 30.0f / (math::pow(radius, 5.0f) * math::PI<float>());
		float v = radius - dist;
		return -v * v * factor;
	}
	return 0;
}

float
FluidSim2D_Cpu::calcSharedPressure(float densityA, float densityB)
{
	float pressureA = _simConfig.calcPressureByDensity(densityA);
	float pressureB = _simConfig.calcPressureByDensity(densityB);
	return (pressureA + pressureB) / 2.0f;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcSharedPressureByDensityData(DimT densityDataA, DimT densityDataB)
{
	auto pressureA = _simConfig.calcPressureByDensityData(densityDataA);
	auto pressureB = _simConfig.calcPressureByDensityData(densityDataB);
	auto pressureData = (pressureA + pressureB) / 2.0f;
	return pressureData;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcExternalForce(DimT inputPos, float radius, float strength, SizeT tarParticleIdx)
{
	DimT	interactionForce	= DimT::s_zero();
	DimT	offset				= inputPos - _positions[tarParticleIdx];
	float	distSq				= offset.dot(offset);

	if (distSq < radius * radius)
	{
		float dist = math::sqrt(distSq);
		DimT inputDirToPt = math::equals0(dist) ? DimT::s_zero() : offset / dist;
		float center = 1.0f - dist / radius;		// 1 is the pt, 0 when in edge
		interactionForce += (inputDirToPt * strength - _velocities[tarParticleIdx]) * center;
	}

	return interactionForce;
}

float FluidSim2D_Cpu::calcDensity(SizeT tarParticleIdx)
{
	return _simConfig.useSpatialOptimization ? _calcDensity_Spatial(tarParticleIdx) : _calcDensity_Raw(tarParticleIdx);

}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcDensityData(SizeT tarParticleIdx)
{
	auto v = DimT::s_zero();

	auto fn =
		[&](const NeighbourInfo& info)
		{
			auto kernel		= smoothingKernel(_simConfig.smoothingRadius, info.distance);
			auto kernelNear = spikyKernelPow3(_simConfig.smoothingRadius, info.distance);

			v.x += kernel;
			v.y += kernelNear;
		};
	foreachPointWithinRadius(tarParticleIdx, _simConfig.smoothingRadius, false, fn);

	return v;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcGradient(SizeT tarParticleIdx)
{
	return _simConfig.useSpatialOptimization ? _calcGradient_Spatial(tarParticleIdx) : _calcGradient_Raw(tarParticleIdx);
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcPressureForce(SizeT tarParticleIdx)
{
	return _simConfig.useSpatialOptimization ? _calcPressureForce_Spatial(tarParticleIdx) : _calcPressureForce_Raw(tarParticleIdx);
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcPressureForceByDensityData(SizeT tarParticleIdx)
{
	DimT v = DimT::s_zero();

	auto tarPos = _positions[tarParticleIdx];
	auto fn =
		[&](const NeighbourInfo& info)
		{
			auto dKernel		= smoothingKernelDerivative(_simConfig.smoothingRadius, info.distance);
			auto dKernelNear	= spikyKernelPow3Derivative(_simConfig.smoothingRadius, info.distance);

			const auto& neighbourdensitData = _densityData[info.index];
			// simple sol for applying newton 3rd law
			auto sharedPressureData = calcSharedPressureByDensityData(neighbourdensitData, _densityData[tarParticleIdx]);

			v += info.direction * (_simConfig.particleMass * dKernel		* sharedPressureData.x / neighbourdensitData.x);
			v += info.direction * (_simConfig.particleMass * dKernelNear	* sharedPressureData.y / neighbourdensitData.y);
		};
	foreachPointWithinRadius(tarParticleIdx, _simConfig.smoothingRadius, true, fn);

	return v;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcViscosityForce(SizeT tarParticleIdx)
{
	auto v = DimT::s_zero();

	auto fn =
		[&](const NeighbourInfo& info)
		{
			auto kernel = smoothingKernelPoly6(_simConfig.smoothingRadius, info.distance);
			v += (_velocities[info.index] - _velocities[tarParticleIdx]) * kernel;
		};
	foreachPointWithinRadius(tarParticleIdx, _simConfig.smoothingRadius, true, fn);

	return v * _simConfig.viscosityStrength;
}

float 
FluidSim2D_Cpu::_calcDensity_Spatial(SizeT tarParticleIdx)
{
	float v = 0.0f;

	auto fn =
		[&](const NeighbourInfo& info)
		{
			auto kernel = smoothingKernel(_simConfig.smoothingRadius, info.distance);
			v += _simConfig.particleMass * kernel;
		};
	foreachPointWithinRadius(tarParticleIdx, _simConfig.smoothingRadius, false, fn);

	#if 0
	// sp_debug
	if (math::equals0(v))
	{
		RDS_LOG_ERROR("Error");
	}
	#endif // 0

	return v;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::_calcGradient_Spatial(SizeT tarParticleIdx)
{
	DimT v = DimT::s_zero();

	auto tarPos = _positions[tarParticleIdx];
	auto fn =
		[&](const NeighbourInfo& info)
		{
			auto dKernel	= smoothingKernelDerivative(_simConfig.smoothingRadius, info.distance);
			auto density    = _densities[info.index];
			v += -info.direction * (dKernel * _simConfig.particleMass / density);
		};
	foreachPointWithinRadius(tarParticleIdx, _simConfig.smoothingRadius, true, fn);

	return v;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::_calcPressureForce_Spatial(SizeT tarParticleIdx)
{
	DimT v = DimT::s_zero();

	auto tarPos = _positions[tarParticleIdx];
	auto fn =
		[&](const NeighbourInfo& info)
		{
			auto dKernel	= smoothingKernelDerivative(_simConfig.smoothingRadius, info.distance);
			auto density    = _densities[info.index];

			// simple sol for applying newton 3rd law
			auto sharedPressure = calcSharedPressure(density, _densities[tarParticleIdx]);	
			v += sharedPressure * info.direction * (dKernel * _simConfig.particleMass / density);
		};
	foreachPointWithinRadius(tarParticleIdx, _simConfig.smoothingRadius, true, fn);

	return v;
}

float
FluidSim2D_Cpu::_calcDensity_Raw(SizeT tarParticleIdx)
{
	// interpolation eq
	// p = density
	// K(x) = loop i ( K_i * mass / p_i * Kernel(||x - x_i|| )

	float v = 0.0f;

	const auto& positions = _predictedPositions;
	auto tarPos = positions[tarParticleIdx];
	for (size_t i = 0; i < positions.size(); i++)
	{
		auto pos = positions[i];
		auto dist = tarPos.distance(pos);
		auto kernel = smoothingKernel(_simConfig.smoothingRadius, dist);
		v += _simConfig.particleMass * kernel;
	}

	return v;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::_calcGradient_Raw(SizeT tarParticleIdx)
{
	DimT v = DimT::s_zero();

	const auto& positions = _predictedPositions;
	auto tarPos = positions[tarParticleIdx];
	for (size_t i = 0; i < positions.size(); i++)
	{
		auto pos		= positions[i];
		auto dist		= tarPos.distance(pos);
		auto dir		= !math::equals0(dist) ? (pos - tarPos) / dist : DimT::s_zero();
		auto dKernel	= smoothingKernelDerivative(_simConfig.smoothingRadius, dist);

		auto density    = _densities[i]; // calcDensity(i);
		v += -dir * (dKernel * _simConfig.particleMass / density);
	}

	return v;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::_calcPressureForce_Raw(SizeT tarParticleIdx)
{
	DimT v = DimT::s_zero();

	const auto& positions = _predictedPositions;
	auto tarPos = positions[tarParticleIdx];
	for (size_t i = 0; i < positions.size(); i++)
	{
		if (i == tarParticleIdx) continue;

		auto pos		= positions[i];
		auto dist		= tarPos.distance(pos);
		auto dir		= !math::equals0(dist) ? (pos - tarPos) / dist : Random::instance()->direction<DimT>();
		auto dKernel	= smoothingKernelDerivative(_simConfig.smoothingRadius, dist);

		auto density		= _densities[i]; // calcDensity(i);

		// simple sol for applying newton 3rd law
		auto sharedPressure = calcSharedPressure(density, _densities[tarParticleIdx]);	
		v += sharedPressure * dir * (dKernel * _simConfig.particleMass / density);
	}

	return v;
}

void 
FluidSim2D_Cpu::foreachPointWithinRadius(SizeT tarParticleIdx, float radius, bool isSkipSelf, const Function<void(const NeighbourInfo&)>& callback)
{
	const auto& positions		= _predictedPositions;
	const auto& tarPos			= positions[tarParticleIdx];
	auto		radiusSq		= radius * radius;
	auto		tarCellCoord	= positionToCellCoord(tarPos, radius);

	Vector<DimT_i, 9> cellOffsets = {
		DimT_i(-1, +1),
		DimT_i(+0, +1),
		DimT_i(+1, +1),
		DimT_i(-1, +0),
		DimT_i(+0, +0),
		DimT_i(+1, +0),
		DimT_i(-1, -1),
		DimT_i(+0, -1),
		DimT_i(+1, -1),
	};

	for (size_t cellOffsetIdx = 0; cellOffsetIdx < cellOffsets.size(); cellOffsetIdx++)
	{
		IdxT curCellKey			= calcCellKeyByCellCoord(tarCellCoord + cellOffsets[cellOffsetIdx]);
		IdxT curCellStartIdx	= _cellKeyStartIndices[curCellKey];

		for (IdxT i = curCellStartIdx; i < _spatialLut.size(); i++)
		{
			auto neighbourIdx	= _spatialLut[i].x;
			auto cellKey		= _spatialLut[i].y;
			if (curCellKey != cellKey) break;

			// cal density shd not skip self, otherwise, when there is only one particle in the cell, the final density will become 0
			if (bool isSelf = tarParticleIdx == neighbourIdx && isSkipSelf) 
				continue;

			auto dirDiff = (positions[neighbourIdx] - tarPos);
			float distSq = positions[neighbourIdx].sqrDistance(tarPos);
			if (bool isOutSide = distSq > radiusSq) continue;

			// compute
			NeighbourInfo info;
			info.index		= neighbourIdx;
			info.distance	= math::sqrt(distSq);
			info.direction	= !math::equals0(info.distance) ? dirDiff / info.distance : Random::instance()->direction<DimT>();
			callback(info);
		}
	}
}

void 
FluidSim2D_Cpu::debugForeachPointWithinRadius(const DimT& samplingPt, float radius, bool isSkipSelf, const Function<void(const NeighbourInfo&)>& callback)
{
	const auto& positions		= _predictedPositions;
	const auto& tarPos			= samplingPt;
	auto		radiusSq		= radius * radius;
	auto		tarCellCoord	= positionToCellCoord(tarPos, radius);

	Vector<DimT_i, 9> cellOffsets = {
		DimT_i(-1, +1),
		DimT_i(+0, +1),
		DimT_i(+1, +1),
		DimT_i(-1, +0),
		DimT_i(+0, +0),
		DimT_i(+1, +0),
		DimT_i(-1, -1),
		DimT_i(+0, -1),
		DimT_i(+1, -1),
	};

	for (size_t cellOffsetIdx = 0; cellOffsetIdx < cellOffsets.size(); cellOffsetIdx++)
	{
		IdxT curCellKey			= calcCellKeyByCellCoord(tarCellCoord + cellOffsets[cellOffsetIdx]);
		IdxT curCellStartIdx	= _cellKeyStartIndices[curCellKey];

		for (IdxT i = curCellStartIdx; i < _spatialLut.size(); i++)
		{
			auto neighbourIdx	= _spatialLut[i].x;
			auto cellKey		= _spatialLut[i].y;
			if (curCellKey != cellKey) break;

			// cal density shd not skip self, otherwise, when there is only one particle in the cell, the final density will become 0
			/*if (bool isSelf = tarParticleIdx == neighbourIdx && isSkipSelf) 
			continue;*/

			auto dirDiff = (positions[neighbourIdx] - tarPos);
			float distSq = positions[neighbourIdx].sqrDistance(tarPos);
			if (bool isOutSide = distSq > radiusSq) continue;

			// compute
			NeighbourInfo info;
			info.index		= neighbourIdx;
			info.distance	= math::sqrt(distSq);
			info.direction	= !math::equals0(info.distance) ? dirDiff / info.distance : Random::instance()->direction<DimT>();
			callback(info);
		}
	}
}

void
FluidSim2D_Cpu::updateSpatialLut(const Vector<DimT>& pts, float radius)
{
	// gpu friendly approach for optimization
	// Particle Simulation using CUDA - Simon Green
	// https://web.archive.org/web/20140725014123/https://docs.nvidia.com/cuda/samples/5_Simulations/particles/doc/particles.pdf

	/*
	step:
	1. get cell key by hash cellCoord and % total cell count
	2. sort the lut by cellKey in ascending order
	3. then _cellKeyStartIndices store the first occurrence (as a index) of the cell key in lut 
	*/

	auto n = pts.size();

	_spatialLut.clear();
	_cellKeyStartIndices.clear();

	_spatialLut.resize(n);
	_cellKeyStartIndices.resize(n);

	for (u32 i = 0; i < n; i++)
	{
		auto pos		= pts[i];
		auto cellCoord	= positionToCellCoord(pos, radius);
		auto cellKey	= calcCellKeyByCellCoord(cellCoord);
		_spatialLut[i]			= DimT_u{i, cellKey};
		_cellKeyStartIndices[i] = NumLimit<DimT_u::ElementType>::max();
	}

	rds::sort(_spatialLut.begin(), _spatialLut.end(), [](const auto& lhs, const auto& rhs) { return lhs.y < rhs.y; } );

	for (u32 i = 0; i < n; i++)
	{
		auto cellKey = _spatialLut[i].y;
		auto prevCellKey = i == 0 ? NumLimit<DimT_u::ElementType>::max() : _spatialLut[i - 1].y;
		if (prevCellKey != cellKey)
		{
			_cellKeyStartIndices[cellKey] = i;
		}
	}
}

FluidSim2D_Cpu::DimT_i 
FluidSim2D_Cpu::positionToCellCoord(const DimT& pt, float radius)
{
	DimT	tmp = pt / radius;
	DimT_i	o	= {math::floorTo_Int(tmp.x), math::floorTo_Int(tmp.y)};
	return o;
}

FluidSim2D_Cpu::IdxT 
FluidSim2D_Cpu::calcCellKeyByCellCoord(const DimT_i& cellCoord)
{
	auto o = hashCellCoord(cellCoord);
	return o % _spatialLut.size();
}

FluidSim2D_Cpu::HashT 
FluidSim2D_Cpu::hashCellCoord(const DimT_i& cellCoord)
{
	// avoid hash collisions for nearby cells
	auto a = (IdxT)cellCoord.x * 15823;
	auto b = (IdxT)cellCoord.y * 9737333;
	return a + b;
}
#endif // 1

#endif

u32 
ParticleSpawner2D::spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outPredictedPositions
	, Vector<Vec2f>& outVelocities
	, Vector<float>& outDensities, Vector<Vec2f>& outDensityData)
{
	auto nParticlesPerAxis	= calcSpawnCountPerAxis();
	auto nParticles			= nParticlesPerAxis.x * nParticlesPerAxis.y;

	//auto& outPositions	= args.outPositions;
	//auto& outVelocities	= args.outVelocities;
	//auto& outDensities	= args.outDensities;

	outPositions.clear();
	outPredictedPositions.clear();
	outVelocities.clear();
	outDensities.clear();
	outDensityData.clear();

	outPositions.reserve(nParticles);

	outPredictedPositions.resize(nParticles);
	outVelocities.resize(nParticles);
	outDensities.resize(nParticles);
	outDensityData.resize(nParticles);

	auto spawnRegionCenter	= Vec2f{spawnRegion.pos} + Vec2f{spawnRegion.size} / 2.0f;
	auto spawnRegionSize	= Vec2f{spawnRegion.size};
	for (size_t y = 0; y < nParticlesPerAxis.y; y++)
	{
		for (size_t x = 0; x < nParticlesPerAxis.x; x++)
		{
			float tx = x / (nParticlesPerAxis.x - 1.0f);
			float ty = y / (nParticlesPerAxis.y - 1.0f);

			float px = (tx - 0.5f) * spawnRegionSize.x + spawnRegionCenter.x;
			float py = (ty - 0.5f) * spawnRegionSize.y + spawnRegionCenter.y;

			outPositions.emplace_back(px, py);
		}
	}

	for (size_t i = 0; i < outPositions.size(); i++)
	{
		auto rng = Random::instance();
		float angle		= (float)rng->range(0.0f, 1.0f) * 3.14f * 2;
		Vec2f dir		= Vec2f{math::cos(angle), math::sin(angle)};
		Vec2f jitter	= dir * jitterFct * (rng->range(0.0f, 1.0f) - 0.5f);

		outPositions[i]		= outPositions[i] + jitter;
		outVelocities[i]	= initVelocity;
	}

	return nParticles;
}

Vec2i 
ParticleSpawner2D::calcSpawnCountPerAxis() const
{
	auto size = Vec2f{spawnRegion.size};

	float area = size.x * size.y;
	int targetTotal = math::ceilToInt(area * spawnDensity);

	float lenSum = size.x + size.y;
	Vec2f t = size / lenSum;
	float m = math::sqrt(targetTotal / (t.x * t.y));
	int nx = math::ceilToInt(t.x * m);
	int ny = math::ceilToInt(t.y * m);

	return { nx, ny };
}

float 
FluidSim2DConfig::calcPressureByDensity(float dens)
{
	float densityDiff = dens - targetDensity;
	float pressure = densityDiff * pressureMultiplier;
	return pressure;
}

float 
FluidSim2DConfig::calcNearPressureByDensity(float nearDens)
{
	float nearPressure	= nearDens * nearPressureMultiplier;
	return nearPressure;
}

Vec2f 
FluidSim2DConfig::calcPressureByDensityData(const Vec2f& densData)
{
	float pressure		= calcPressureByDensity(densData.x);
	float nearPressure	= calcNearPressureByDensity(densData.y);
	return Vec2f{pressure, nearPressure};
}

void
FluidSim2DConfig::drawGui(EditorUiDrawRequest& uiDrawReq)
{
	if (!_fluSim)
		return;

	auto wnd = uiDrawReq.makeWindow("config");
	uiDrawReq.makeCheckbox("useSpatialOptimization",	&useSpatialOptimization);
	uiDrawReq.dragFloat("particleSize",					&particleSize,			0.01f);
	uiDrawReq.dragFloat("particleMass",					&particleMass,			0.01f);
	uiDrawReq.dragFloat("timeMultiplier",				&timeMultiplier,		0.01f);
	uiDrawReq.dragFloat("smoothingRadius",				&smoothingRadius,		0.1f, 0.1f);
	uiDrawReq.dragFloat("collisionDamping",				&collisionDamping,		0.1f);
	uiDrawReq.dragFloat("targetDensity",				&targetDensity,			0.1f);
	uiDrawReq.dragFloat("pressureMultiplier",			&pressureMultiplier,	1.0f);
	uiDrawReq.dragFloat("viscosityStrength",			&viscosityStrength,		0.01f);

	uiDrawReq.dragFloat("interactionRadius",			&interactionRadius,		0.01f, 0.01f);
	uiDrawReq.dragFloat("interactionStrength",			&interactionStrength,	1.0f);

	auto makeColorPicker4 = [](const char* label, Color4f* oColor)
		{
			float v[4];
			v[0] = oColor->r;
			v[1] = oColor->g;
			v[2] = oColor->b;
			v[3] = oColor->a;

			ImGui::ColorEdit4(label, v);

			oColor->r = v[0];
			oColor->g = v[1];
			oColor->b = v[2];
			oColor->a = v[3];
		};
	makeColorPicker4("particleColor",		&particleColor);
	makeColorPicker4("colorGradientKey0",	&colorGradientKey0);
	makeColorPicker4("colorGradientKey1",	&colorGradientKey1);
	makeColorPicker4("colorGradientKey2",	&colorGradientKey2);
	makeColorPicker4("colorGradientKey3",	&colorGradientKey3);

	uiDrawReq.makeCheckbox("isInvalidateColorMap", &isInvalidateColorMap);
	uiDrawReq.showText("colorGradientTexture:");
	uiDrawReq.showImage(_fluSim->particleDisplay().colorGradientTexture());
	RDS_TODO("sample this texture should use clamp sampler");

	uiDrawReq.showText("debugDensity: {}",				debugDensity);
	uiDrawReq.showText("debugMousePosViewport: {}",		debugMousePosViewport);
	uiDrawReq.showText("debugMousePosWorld: {}",		debugMousePosWorld);
	uiDrawReq.showText("debugMouseDirWorld: {}",		debugMouseDirWorld);

	uiDrawReq.showText("debugParticleCount: {}",		debugParticleCount);
	uiDrawReq.showText("debugWithinRadiusCount: {}",	debugWithinRadiusCount);
	uiDrawReq.showText("debugMouseCellCoord: {}",		_fluSim->positionToCellCoord(debugMousePosWorld.toVec2(), smoothingRadius));
}

void 
ParticleDisplay::s_createColorGradientTexture(SPtr<Texture2D>& oTex, const ColorGradient& colorGradient)
{
	int w = 64;
	int h = 4;
	auto texDesc = Texture2D::makeCDesc(RDS_SRCLOC);
	texDesc.usageFlags	= TextureUsageFlags::ShaderResource;
	texDesc.format		= ColorType::RGBAb;
	texDesc.mipCount	= 1;
	texDesc.size.set(w, h, 1);

	auto& image = texDesc.uploadImage;
	image.create(Color4b::s_kColorType, w, h);

	for (int y = 0; y < h; y++) 
	{
		auto span = image.row<Color4b>(y);
		for (int x = 0; x < w; x++) 
		{
			float t = x / (w - 1.0f);
			span[x] = colorGradient.evaluate(t).toColorRGBAb();
		}
	}

	oTex = Renderer::renderDevice()->createTexture2D(texDesc);
}

void 
ParticleDisplay::create2D(const ColorGradient& colorGrad)
{
	RenderMesh& rdMesh = _rdMesh;

	{
		using VtxT = Vertex_PosUv<1>;
		using IdxT = u16;
		EditMesh mesh;

		auto& pos = mesh.pos;
		pos.resize(4);
		pos[0] = Vec3f{Vec2f{ -1.0f, +1.0f }, 0.0f};
		pos[1] = Vec3f{Vec2f{ +1.0f, +1.0f }, 0.0f};
		pos[2] = Vec3f{Vec2f{ -1.0f, -1.0f }, 0.0f};
		pos[3] = Vec3f{Vec2f{ +1.0f, -1.0f }, 0.0f};

		auto& uv = mesh.uvs[0];
		uv.resize(4);
		uv[0] = Vec2f{0.0, 0.0};
		uv[1] = Vec2f{1.0, 0.0};
		uv[2] = Vec2f{0.0, 1.0};
		uv[3] = Vec2f{1.0, 1.0};

		auto& idxs = mesh.indices;
		idxs.reserve(6);
		idxs.emplace_back(0); idxs.emplace_back(2); idxs.emplace_back(1);
		idxs.emplace_back(3); idxs.emplace_back(1); idxs.emplace_back(2);

		rdMesh.create(mesh);
	}

	auto cDesc = RenderGpuBuffer::makeCDesc(RDS_SRCLOC);
	cDesc.bufSize	= 16;
	cDesc.stride	= sizeof(Vec2f);
	cDesc.typeFlags = RenderGpuBufferTypeFlags::Vertex | RenderGpuBufferTypeFlags::Compute;
	_posBufGpu = Renderer::renderDevice()->createRenderGpuMultiBuffer(cDesc);	_posBufGpu->setDebugName("_posBufGpu");

	cDesc.typeFlags = RenderGpuBufferTypeFlags::Index | RenderGpuBufferTypeFlags::Compute;
	_velBufGpu = Renderer::renderDevice()->createRenderGpuMultiBuffer(cDesc);	_velBufGpu->setDebugName("_velBufGpu");

	GraphicsDemo::createMaterial(&_shaderPtcDisplay, &_mtlPtcDisplay, "asset/shader/demo/fluid_simulation/rdsParticleDisplay2D.shader");

	invalidateColorGradient(colorGrad);
}

void 
ParticleDisplay::invalidateColorGradient(const ColorGradient& colorGrad)
{
	_colorGradient = colorGrad;
	s_createColorGradientTexture(_texColorGradient, _colorGradient);
}

Texture2D* 
ParticleDisplay::colorGradientTexture()
{
	return _texColorGradient;
}


}