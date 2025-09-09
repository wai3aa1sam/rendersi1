#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

class FluidSim2D_Cpu;

#if 0
#pragma mark --- rdsGradient-Decl ---
#endif // 0
#if 1

struct ColorGradientKey 
{
public:
	using ColorT = Color4f;

public:
	ColorT color;
	float time;

public:
	ColorGradientKey(const ColorT& col, float t) : color(col), time(t) {}

	bool operator<(const ColorGradientKey& rhs) const { return time < rhs.time; }
	bool operator>(const ColorGradientKey& rhs) const { return operator<(rhs); }
};
inline void swap(ColorGradientKey& a, ColorGradientKey& b) noexcept { rds::swap<ColorGradientKey>(a, b); }

class ColorGradient
{
public:
	using ColorT = Color4f;

	/*
	* prompt:
	* how to create gradient color in c++. don't use library, just like unity Gradient, guess its implementation. after set ColorGradientKey, i call gradient.Evaluate(t) to get the color
	*/

public:
	static ColorT s_lerp(const ColorT& a, const ColorT& b, float t) {
		return ColorT(
			a.r + t * (b.r - a.r),
			a.g + t * (b.g - a.g),
			a.b + t * (b.b - a.b),
			a.a + t * (b.a - a.a)
		);
	}

public:
	ColorT evaluate(float t) const
	{
		// If t is before first key
		if (t <= _colorKeys.front().time) 
		{
			return _colorKeys.front().color;
		}

		// If t is after last key
		if (t >= _colorKeys.back().time) 
		{
			return _colorKeys.back().color;
		}

		// Find the two keys to interpolate between
		for (size_t i = 0; i < _colorKeys.size() - 1; ++i) 
		{
			const ColorGradientKey& key1 = _colorKeys[i];
			const ColorGradientKey& key2 = _colorKeys[i + 1];

			if (t >= key1.time && t <= key2.time) 
			{
				// Normalize t between the two keys
				float normalizedT = (t - key1.time) / (key2.time - key1.time);
				return s_lerp(key1.color, key2.color, normalizedT);
			}
		}

		// Fallback (should not reach here if sorted properly)
		return _colorKeys.back().color;
	}

	void addColorKey(const ColorGradientKey& key)
	{
		_colorKeys.emplace_back(key);
		rds::sort(_colorKeys.begin(), _colorKeys.end());
	}

	void clear()
	{
		_colorKeys.clear();
		//_isSorted = false;
	}

public:


private:
	using ColorKeys = Vector<ColorGradientKey, 4>;
	ColorKeys	_colorKeys;
	//bool		_isSorted = false; // Flag to check if sorted
};

#endif

struct FluidSim2DConfig
{
public:
	float		particleMass		= 1.0f;
	float		particleSize		= 0.1f / 2.0f;
	Color4f		particleColor		= Color4f{0.2f, 1.0f, 1.0f, 1.0f};

	Color4f		colorGradientKey0	= Color4f{0.075f, 0.584f, 0.827f, 1.0f};
	Color4f		colorGradientKey1	= Color4f{0.0f, 1.0f, 0.0f, 1.0f};
	Color4f		colorGradientKey2	= Color4f{1.0f, 1.0f, 0.0f, 1.0f};
	Color4f		colorGradientKey3	= Color4f{1.0f, 0.0f, 0.0f, 1.0f};

	float		smoothingRadius			= 0.35f;
	float		collisionDamping		= 0.95f;
	float		targetDensity			= 55.0f;
	float		pressureMultiplier		= 500.0f;
	float		nearPressureMultiplier	= 5.0f;
	float		viscosityStrength		= 0.03f;

	float		interactionRadius	= 2.0f;
	float		interactionStrength = 90.0f;

	float		timeMultiplier		= 1.0f;
	float		gravity				= 12.0f;
	Vec2f		gravityDir			= Vec2f::s_down();

	Rect2f		boundingRegion		= { Vec2f{0.0, 0.0},	Vec2f{5.0,	2.5} * 2.0f };
	Rect2f		spawnRegion			= { Vec2f{0.0, 0.0},	Vec2f{boundingRegion.size} / 3.0f };

	bool useSpatialOptimization = 1;
	bool isInvalidateColorMap	= 1;

	bool useDebugLog			= 0;
	bool useDebugSpatial		= 0;

public:
	FluidSim2DConfig()
	{

	}

	void create(FluidSim2D_Cpu* sim)
	{
		_fluSim = sim;
	}

	ColorGradient makeColorGradient()
	{
		ColorGradient colGrad;
		colGrad.addColorKey(ColorGradientKey{colorGradientKey0, 0.05f});
		colGrad.addColorKey(ColorGradientKey{colorGradientKey1, 0.5f});
		colGrad.addColorKey(ColorGradientKey{colorGradientKey2, 0.6f});
		colGrad.addColorKey(ColorGradientKey{colorGradientKey3, 1.0f});
		return colGrad;
	}

public:
	float calcPressureByDensity(	float dens);
	float calcNearPressureByDensity(float nearDens);
	Vec2f calcPressureByDensityData(const Vec2f& densData);

public:
	float		debugDensity			= 128.0f;
	Vec2f		debugMousePosViewport	= Vec2f::s_zero();

	Vec3f		debugMousePosWorld		= Vec3f::s_zero();
	Vec3f		debugMouseDirWorld		= Vec3f::s_zero();

	u32			debugParticleCount		= 0;
	u32			debugWithinRadiusCount	= 0;

public:
	void drawGui(EditorUiDrawRequest& uiDrawReq);

private:
	FluidSim2D_Cpu* _fluSim = nullptr;
};

class ParticleSpawner2D
{
public:
	Vec2f initVelocity		= Vec2f{0.0, 0.0};
	float jitterFct			= 0.03f;
	float spawnDensity		= 160.0f; // 128.0;

	Rect2f spawnRegion;

public:
	struct SpawnArgs
	{
		Vector<Vec2f>& outPositions;
		Vector<Vec2f>& outVelocities;
		Vector<float>& outDensities;
	};
	u32		spawnTo(Vector<Vec2f>& outPositions, Vector<Vec2f>& outPredictedPositions, Vector<Vec2f>& outVelocities, Vector<float>& outDensities, Vector<Vec2f>& outDensityData);
	Vec2i	calcSpawnCountPerAxis() const;
};

class ParticleDisplay
{
public:
	void s_createColorGradientTexture(SPtr<Texture2D>& oTex, const ColorGradient& colorGradient);

public:
	void create2D(const ColorGradient& colorGrad);
	void invalidateColorGradient(const ColorGradient& colorGrad);

	void draw(RenderRequest& rdReq, DrawData* drawData, const Span<Vec2f>& positions, const Span<Vec2f>& velocities, float radius)
	{
		RDS_CORE_ASSERT(_posBufGpu && _velBufGpu);

		_posBufGpu->uploadToGpu(makeByteSpan(positions));
		_velBufGpu->uploadToGpu(makeByteSpan(velocities));

		_mtlPtcDisplay->setParam("u_colorMap",		_texColorGradient);
		_mtlPtcDisplay->setParam("u_colorMap",		SamplerState::makeLinearClampToEdge());

		_mtlPtcDisplay->setParam("u_positions",		_posBufGpu->renderGpuBuffer());
		_mtlPtcDisplay->setParam("u_velocities",	_velBufGpu->renderGpuBuffer());
		_mtlPtcDisplay->setParam("u_scale",			radius);
		_mtlPtcDisplay->setParam("u_velocityMax",	6.5f);
		_mtlPtcDisplay->setParam("u_objToWorld",	Mat4f::s_identity());
		_mtlPtcDisplay->setParam("u_worldToObj",	Mat4f::s_identity());

		drawData->setupMaterial(_mtlPtcDisplay);
		rdReq.drawMesh_Instanced(RDS_SRCLOC, _rdMesh, _mtlPtcDisplay, positions.size());
	}

public:
	Texture2D*	colorGradientTexture();

private:
	SPtr<Shader>	_shaderPtcDisplay;
	SPtr<Material>	_mtlPtcDisplay;
	SPtr<Texture2D>	_texColorGradient;

	RenderMesh					_rdMesh;
	SPtr<RenderGpuMultiBuffer>	_posBufGpu;
	SPtr<RenderGpuMultiBuffer>	_velBufGpu;

	ColorGradient	_colorGradient;
};

#if 0
#pragma mark --- rdsFluidSimulation-Decl ---
#endif // 0
#if 1

class FluidSimDemo_Base : public NonCopyable
{
public:
	virtual void onCreate(GraphicsDemo* parentDemo)							{ _parentDemo = parentDemo; };

	virtual void onUpdate(float dt)
	{
		if (_parentDemo)
		{
			_mousePosViewport	= _parentDemo->mousePosViewport;
			_mouseRayWorld		= _parentDemo->mouseRayWorldSpace;
		}
	};

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline)	{};
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline)	{};

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) {};

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) {};
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) {};

protected:
			DemoEditorLayer* demoLayer()		{ return _parentDemo ? _parentDemo->demoLayer() : nullptr; }
	const	DemoEditorLayer* demoLayer() const	{ return _parentDemo ? _parentDemo->demoLayer() : nullptr; }

protected:
	bool isFocusOnEditorViewport()	const	{ return demoLayer() ? demoLayer()->isFocusOnEditorViewport() : false; }

protected:
	GraphicsDemo*	_parentDemo = nullptr;
	Vec2f			_mousePosViewport;
	Ray3f			_mouseRayWorld;
};

class FluidSim2D_Cpu : public FluidSimDemo_Base
{
public:
	using Base = FluidSimDemo_Base;
	
	struct SimState
	{
		bool isStop			= true;		// : 1
		bool isStepForward	= false;
		bool isStepBackward	= false;

		bool	isPullInteraction = false;
		bool	isPushInteraction = false;
	};
	SimState _simState;

public:
	virtual void onCreate(GraphicsDemo* parentDemo)						override;

	virtual void onUpdate(float dt)										override;

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

public:
	ParticleDisplay& particleDisplay();

public:
	using DimT		= Vec2f;
	using DimT_i	= Vec2i;
	using DimT_u	= Vec2u;
	using IdxT		= u32;
	using HashT		= IdxT;

	using SizeT = DemoTraits::SizeType;

	void	update(float dt);
	void	simulate(float dt);
	void	resolveCollisions(DimT& outPos, DimT& outVel);

	float	smoothingKernel(			float radius, float dist);
	float	smoothingKernelDerivative(	float radius, float dist);
	float	smoothingKernelPoly6(		float radius, float dist);
	float	spikyKernelPow3(			float radius, float dist);
	float	spikyKernelPow3Derivative(	float radius, float dist);

	float	calcSharedPressure(float densityA, float densityB);
	DimT	calcSharedPressureByDensityData(DimT densityDataA, DimT densityDataB);

	DimT	calcExternalForce(	DimT inputPos, float radius, float strength, SizeT tarParticleIdx);

	float	calcDensity(					SizeT tarParticleIdx);
	DimT	calcDensityData(				SizeT tarParticleIdx);
	DimT	calcGradient(					SizeT tarParticleIdx);
	DimT	calcPressureForce(				SizeT tarParticleIdx);
	DimT	calcPressureForceByDensityData(	SizeT tarParticleIdx);		// add near density make the droplet together instead of split out
	DimT	calcViscosityForce(				SizeT tarParticleIdx);

	float	_calcDensity_Spatial(		SizeT tarParticleIdx);
	DimT	_calcGradient_Spatial(		SizeT tarParticleIdx);
	DimT	_calcPressureForce_Spatial(	SizeT tarParticleIdx);

	float	_calcDensity_Raw(		SizeT tarParticleIdx);
	DimT	_calcGradient_Raw(		SizeT tarParticleIdx);
	DimT	_calcPressureForce_Raw(	SizeT tarParticleIdx);

public:
	void logDebugSpatial();

public:
	struct NeighbourInfo
	{
		IdxT	index;
		float	distance;
		DimT	direction;
	};
	void	foreachPointWithinRadius(SizeT tarParticleIdx, float radius, bool isSkipSelf, const Function<void(const NeighbourInfo&)>& callback);
	void	debugForeachPointWithinRadius(const DimT& samplingPt, float radius, bool isSkipSelf, const Function<void(const NeighbourInfo&)>& callback);

	void	updateSpatialLut(const Vector<DimT>& pts, float radius);
	DimT_i	positionToCellCoord(const DimT& pt, float radius);
	IdxT	calcCellKeyByCellCoord(const DimT_i& cellCoord);
	HashT	hashCellCoord(const DimT_i& cellCoord);

private:
	Vector<DimT>	_positions;
	Vector<DimT>	_predictedPositions;

	Vector<DimT>	_velocities;
	
	Vector<float>	_densities;
	Vector<DimT>	_densityData;	// density, nearDesnsity

	Vector<DimT_u>	_spatialLut;
	Vector<IdxT>	_cellKeyStartIndices;
	Vector<DimT_i>	_debugCellCoords;

protected:
	SPtr<Shader>	_shaderFluidSimulation;
	SPtr<Material>	_mtlFluidSimulation;
	SPtr<Texture2D>	_texColorGradient;

	ParticleSpawner2D	_particleSpawner;
	FluidSim2DConfig	_simConfig;
	ParticleDisplay		_ptcDisplay;
};
#endif


}