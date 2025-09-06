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

	_particleSpawner.spawnTo(_positions, _velocities, _densities);

	auto& camera = parentDemo->app().mainWindow().camera();
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

			#if 1
			for (auto& e : _positions)
			{
				rdReq.drawCircle(e, _simConfig.particleSize, _simConfig.particleColor);
			}
			#endif // 0

			RDS_TODO("later put on debug, also, test orthographics mode");
			{
				if (_parentDemo)
				{
					static Ray3f ray;
					if (_parentDemo->uiMouseState.isDown(UiMouseEventButton::Left))
					{
						ray = _mouseRayWorld;
					}
					rdReq.drawLine(ray.origin, ray.origin + ray.dir * 9999.0f, Color4f(1.0f, 0.0f, 0.0f, 1.0f));
					//rdReq.drawCircle(ray.origin.toVec2(), 3.0f, Color4f(1.0f, 0.0f, 0.0f, 1.0f));
				}
				
				rdReq.drawAABBox(Vec3f{ _simConfig.boundingRegion.pos, 0.0f }, Vec3f{_simConfig.boundingRegion.size, 0.001f} / 2.0f);
			}
		}
	);

	drawData->oTexPresent = rtColor;
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
}

void 
FluidSim2D_Cpu::onUiKeyboardEvent(UiKeyboardEvent& ev)
{
	Base::onUiKeyboardEvent(ev);
}

void 
FluidSim2D_Cpu::update(float dt)
{
	simulate(dt);

	#if 0
	{
		float density = 0.0f;
		auto tarPos = _mouseRayWorld.origin.toVec2();
		for (size_t i = 0; i < _positions.size(); i++)
		{
			auto pos = _positions[i];
			auto dist = tarPos.distance(pos);
			auto influence = smoothingKernel(_simConfig.smoothingRadius, dist);
			density += _simConfig.particleMass * influence;
		}
		_simConfig.debugDensity = density;
	}
	#endif // 0

	_simConfig.debugMousePosViewport = _mousePosViewport;
	_simConfig.debugMousePosWorld	 = _mouseRayWorld.origin;
	_simConfig.debugMouseDirWorld	 = _mouseRayWorld.dir;
}

void
FluidSim2D_Cpu::simulate(float dt)
{
	if (true)
	{
		//return;
	}

	dt *= _simConfig.timeMultiplier;

	for (size_t i = 0; i < _positions.size(); i++)
	{
		_velocities[i] += _simConfig.gravityDir * _simConfig.gravity * dt;
		_densities[i]	= calcDensity(_positions[i]);
	}

	for (size_t i = 0; i < _positions.size(); i++)
	{
		DimT pressureForce	= calcPressureForce(i);
		DimT pressureAccel	= pressureForce / _densities[i];
		//_velocities[i] += pressureAccel * dt;
		_velocities[i] = pressureAccel * dt;		// no inertia for debug
	}

	for (size_t i = 0; i < _positions.size(); i++)
	{
		_positions[i] += _velocities[i] * dt;
		resolveCollisions(_positions[i], _velocities[i]);
	}
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
	
	// same but more sparky, like a triangle, solve the particle stick together
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
FluidSim2D_Cpu::calcSharedPressure(float densityA, float densityB)
{
	float pressureA = _simConfig.calcPressureByDensity(densityA);
	float pressureB = _simConfig.calcPressureByDensity(densityB);
	return (pressureA + pressureB) / 2.0f;
}

float 
FluidSim2D_Cpu::calcDensity(const DimT& samplingPt)
{
	// interpolation eq
	// p = density
	// K(x) = loop i ( K_i * mass / p_i * Kernel(||x - x_i|| )

	float v = 0.0f;

	for (size_t i = 0; i < _positions.size(); i++)
	{
		auto pos = _positions[i];
		auto dist = samplingPt.distance(pos);
		auto kernel = smoothingKernel(_simConfig.smoothingRadius, dist);
		v += _simConfig.particleMass * kernel;
	}

	return v;
}
FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcGradient(SizeT tarParticleIdx)
{
	//RandomT::instance()->direction<DimT>();

	DimT v = DimT::s_zero();

	auto tarPos = _positions[tarParticleIdx];
	for (size_t i = 0; i < _positions.size(); i++)
	{
		auto pos		= _positions[i];
		auto dist		= tarPos.distance(pos);
		auto dir		= !math::equals0(dist) ? (pos - tarPos) / dist : DimT::s_zero();
		auto dKernel	= smoothingKernelDerivative(_simConfig.smoothingRadius, dist);

		auto density    = _densities[i]; // calcDensity(i);
		v += -dir * (dKernel * _simConfig.particleMass / density);
	}

	return v;
}

FluidSim2D_Cpu::DimT 
FluidSim2D_Cpu::calcPressureForce(SizeT tarParticleIdx)
{
	DimT v = DimT::s_zero();

	auto tarPos = _positions[tarParticleIdx];
	for (size_t i = 0; i < _positions.size(); i++)
	{
		if (i == tarParticleIdx) continue;

		auto pos		= _positions[i];
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

#endif

u32 
ParticleSpawner2D::spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outVelocities, Vector<float>& outDensities)
{
	auto nParticlesPerAxis	= calcSpawnCountPerAxis();
	auto nParticles			= nParticlesPerAxis.x * nParticlesPerAxis.y;

	//auto& outPositions	= args.outPositions;
	//auto& outVelocities	= args.outVelocities;
	//auto& outDensities	= args.outDensities;

	outPositions.clear();
	outVelocities.clear();
	outDensities.clear();

	outPositions.reserve(nParticles);
	outVelocities.resize(nParticles);
	outDensities.resize(nParticles);

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

void
FluidSim2DConfig::drawGui(EditorUiDrawRequest& uiDrawReq)
{
	auto wnd = uiDrawReq.makeWindow("config");
	uiDrawReq.dragFloat("particleSize",			&particleSize,			0.01f);
	uiDrawReq.dragFloat("particleMass",			&particleMass,			0.01f);
	uiDrawReq.dragFloat("smoothingRadius",		&smoothingRadius,		0.1f);
	uiDrawReq.dragFloat("collisionDamping",		&collisionDamping,		0.1f);
	uiDrawReq.dragFloat("targetDensity",		&targetDensity,			0.1f);
	uiDrawReq.dragFloat("pressureMultiplier",	&pressureMultiplier,	1.0f);
	uiDrawReq.dragFloat("timeMultiplier",		&timeMultiplier,		0.01f);

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
	makeColorPicker4("particleColor", &particleColor);

	uiDrawReq.showText("debugDensity: {}",			debugDensity);
	uiDrawReq.showText("debugMousePosViewport: {}", debugMousePosViewport);
	uiDrawReq.showText("debugMousePosWorld: {}",	debugMousePosWorld);
	uiDrawReq.showText("debugMouseDirWorld: {}",	debugMouseDirWorld);
}

}