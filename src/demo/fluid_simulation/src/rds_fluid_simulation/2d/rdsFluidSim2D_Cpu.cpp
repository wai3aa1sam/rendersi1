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

	_particleSpawner.spawnRegion = _simConfig.spawnRegion;
	//_particleSpawner.spawnRegion = _simConfig.boundingRegion;

	_simConfig.debugParticleCount = _particleSpawner.spawnTo(_positions, _predictedPositions, _velocities, _densities, _densityData);
	_spatialLut.resize(_positions.size());
	_cellKeyStartIndices.resize(_positions.size());
}

void 
FluidSim2D_Cpu::onUpdate(float dt, RenderPassPipeline* renderPassPipeline)
{
	Base::onUpdate(dt, renderPassPipeline);

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

	RdgTextureHnd rtColor	= rdGraph->createTexture("fs2d_color",	Texture2D_CreateDesc{ screenSize, ColorType::RGBAb, TextureUsageFlags::RenderTarget | TextureUsageFlags::ShaderResource});
	RdgTextureHnd dsBuf		= rdGraph->createTexture("fs2d_depth",	Texture2D_CreateDesc{ screenSize, ColorType::Depth, TextureUsageFlags::DepthStencil | TextureUsageFlags::ShaderResource});

	auto& passFluidSim2D_Cpu = rdGraph->addPass("fluid_sim_2d", RdgPassTypeFlags::Graphics);
	passFluidSim2D_Cpu.setRenderTarget(rtColor,	RenderTargetLoadOp::Clear, RenderTargetStoreOp::Store);
	passFluidSim2D_Cpu.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Clear, RenderTargetLoadOp::Clear);	// currently use the pre-pass will cause z-flight
	passFluidSim2D_Cpu.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData);

			auto* clearValue = rdReq.clearFramebuffers();
			clearValue->setClearColor(Color4f{ 0.1f, 0.2f, 0.3f, 1.0f });
			clearValue->setClearDepth(1.0f);

			debug_drawBoundary(rdReq);
			debug_drawSpatialGrid(rdReq);
			debug_drawSpatial(rdReq);
			debug_drawMouseInteraction(rdReq);

			_ptcDisplay.draw(rdReq, drawData, _positions, _velocities, _simConfig.particleSize);
		}
	);

	drawData->oTexPresent = rtColor;
}

void 
FluidSim2D_Cpu::onDrawGui(EditorUiDrawRequest& uiDrawReq)
{
	Base::onDrawGui(uiDrawReq);
}

void 
FluidSim2D_Cpu::onUiMouseEvent(UiMouseEvent& ev)
{
	Base::onUiMouseEvent(ev);

}

void 
FluidSim2D_Cpu::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);

}

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
		debug_logSpatial(); 
		return; 
	);
}

#if 1

void
FluidSim2D_Cpu::simulate(float dt, RenderPassPipeline* renderPassPipeline)
{
	dt *= _simConfig.timeMultiplier;

	{
		for (size_t i = 0; i < _positions.size(); i++)
		{
			auto accel = calcExternalForce(_mouseRayWorld.origin.toVec2(), _simConfig.interactionRadius, _simState.interactionInputStrength, i);
			_velocities[i] += accel * dt;
		}
	}
	
	for (size_t i = 0; i < _positions.size(); i++)
	{
		_velocities[i]			+= _simConfig.gravityDir.toVec2() * _simConfig.gravity * dt;
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

	debug_logSpatial();
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

#if 1


void 
FluidSim2D_Cpu::debug_logSpatial()
{
	if (!_simConfig.useDebugLog)
	{
		return;
	}

	RDS_LOG("----- debug_logSpatial:");

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

	RDS_LOG("----- debug_logSpatial --- end");
}

void 
FluidSim2D_Cpu::debug_drawSpatial(RenderRequest& rdReq)
{
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
}


#endif // 1


#endif

}