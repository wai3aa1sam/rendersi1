#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"
#include "rdsFluidSim2D_Base.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_Cpu-Decl ---
#endif // 0
#if 1

class FluidSim2D_Cpu : public FluidSim2D_Base
{
public:
	using Base = FluidSim2D_Base;

public:
	virtual void onCreate(GraphicsDemo* parentDemo)						override;

	virtual void onUpdate(float dt)										override;

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

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
	void debug_logSpatial();
	void debug_drawSpatial(RenderRequest& rdReq);

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
};
#endif


}