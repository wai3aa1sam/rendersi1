#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"
#include "rdsFluidSim3D_Base.h"
#include "rds_fluid_simulation/common/rdsSpatialLut.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim3D_Gpu-Decl ---
#endif // 0
#if 1

class FluidSim3D_Gpu : public FluidSim3D_Base
{
public:
	using Base = FluidSim3D_Base;

	using DimT		= Vec3f;
	using DimT_i	= Vec3i;
	using DimT_u	= Vec3u;
	using IdxT		= u32;
	using HashT		= IdxT;

	using Gpu_DimT	= Tuple3f;
	using Gpu_IdxT	= u32;
	using Gpu_Idx3T	= Tuple3u;

public:
	virtual void onCreate(GraphicsDemo* parentDemo)							override;
	virtual void onUpdate(float dt, RenderPassPipeline* renderPassPipeline) override;

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

public:
			void update(float dt);
	virtual void simulate(float dt, RenderPassPipeline* renderPassPipeline) override;

public:
	struct CachedSimArgs
	{
	public:
		// save those gpuBuf for next frame
		SPtr<RenderGpuBuffer>	positions		= nullptr;
		SPtr<RenderGpuBuffer>	velocities		= nullptr;
		SPtr<RenderGpuBuffer>	densityData		= nullptr;
		SPtr<RenderGpuBuffer>	predictedPos	= nullptr;

		// buf for rendering if have cached
		RdgBufferHnd bufPos;
		RdgBufferHnd bufVel;
		RdgBufferHnd buf_predictedPos;

	public:
		void create(ParticleSpawner& spawner, u32 n)
		{
			RenderGpuBuffer::makeCDesc(RDS_SRCLOC);

			auto gpuDim_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_DimT)	* n, sizeof(Gpu_DimT), RenderGpuBufferTypeFlags::Compute };
			auto gpuDim_CDesc_WithVs	= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_DimT)	* n, sizeof(Gpu_DimT), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex};
			auto gpuIdx_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_IdxT)	* n, sizeof(Gpu_IdxT), RenderGpuBufferTypeFlags::Compute};
			auto gpuIdx3_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_Idx3T) * n, sizeof(Gpu_Idx3T), RenderGpuBufferTypeFlags::Compute};

			//auto cDesc = RenderGpuBuffer_CreateDesc{ sizeof(Gpu_DimT) * n, sizeof(Gpu_DimT), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex };
			positions		= Renderer::renderDevice()->createRenderGpuBuffer(gpuDim_CDesc_WithVs);
			velocities		= Renderer::renderDevice()->createRenderGpuBuffer(gpuDim_CDesc_WithVs);
			densityData		= Renderer::renderDevice()->createRenderGpuBuffer(gpuDim_CDesc);
			predictedPos	= Renderer::renderDevice()->createRenderGpuBuffer(gpuDim_CDesc);

			positions->setDebugName(	"fs2d_bufPos");
			velocities->setDebugName(	"fs2d_bufVel");
			densityData->setDebugName(	"fs2d_bufDensityData");
			predictedPos->setDebugName(	"fs2d_bufPredictedPos");

			spawner.spawnTo(positions, velocities, predictedPos);
		}
	};

	struct SimArgs
	{
	public:
		// TODO: get pass id by compute string
		static constexpr int s_kPassIdx_Cs_calcExternalForce				= 0;
		static constexpr int s_kPassIdx_Cs_calcDensityData					= 1;
		static constexpr int s_kPassIdx_Cs_calcPressureForce				= 2;
		static constexpr int s_kPassIdx_Cs_calcViscosity					= 3;
		static constexpr int s_kPassIdx_Cs_updatePosition					= 4;

	public:
		RenderGraph*	rdGraph		= nullptr;
		DrawData*		drawData	= nullptr;
		Vec2u			screenSize	= Vec2u::s_zero();
		float			dt			= 0.0f;

		IdxT			particleCount = 0;

		RdgBufferHnd	bufPos;
		RdgBufferHnd	bufVel;
		RdgBufferHnd	bufDensityData;
		RdgBufferHnd	bufPredictedPos;

	public:
		void create(FluidSim3D_Gpu* fs2d_, float dt_, RenderGraph* rdGraph_, DrawData* drawData_)
		{
			_create(fs2d_, dt_, rdGraph_, drawData_);

			auto* cachedSimArgs = &_fs2d->_cachedSimArgs;
			RDS_ASSERT(cachedSimArgs->positions, "cachedSimArgs not yet create");

			bufPos			= rdGraph->importBuffer(cachedSimArgs->positions);
			bufVel			= rdGraph->importBuffer(cachedSimArgs->velocities);
			bufDensityData	= rdGraph->importBuffer(cachedSimArgs->densityData);
			bufPredictedPos = rdGraph->importBuffer(cachedSimArgs->predictedPos);

			auto exportBufType	= RenderGpuBufferTypeFlags::Compute;	// add vertex flags too? but set pass.read also ok
			auto expBufAccess	= RenderAccess::Write;
			rdGraph->exportBuffer(&cachedSimArgs->positions,	bufPos,				exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->velocities,	bufVel,				exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->densityData,	bufDensityData,		exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->predictedPos,	bufPredictedPos,	exportBufType, expBufAccess);
			cachedSimArgs->bufPos			= bufPos;
			cachedSimArgs->bufVel			= bufVel;
			cachedSimArgs->buf_predictedPos = bufPredictedPos;
		}

	public:
		void readSpatialBuffer(RdgPass& pass)
		{
			RdgBufferHnd buf_spatialLut, buf_patialLutKeyToStartIndex;
			_fs2d->_spatialLut.getBufferTo(buf_spatialLut, buf_patialLutKeyToStartIndex);

			pass.readBuffer(bufPredictedPos);
			pass.readBuffer(buf_spatialLut);
			pass.readBuffer(buf_patialLutKeyToStartIndex);
		}
		void setSpatialParam(Material* mtl) const
		{
			RdgBufferHnd buf_spatialLut, buf_patialLutKeyToStartIndex;
			_fs2d->_spatialLut.getBufferTo(buf_spatialLut, buf_patialLutKeyToStartIndex);

			mtl->setParam("u_predictedPositions",			bufPredictedPos.renderResource());
			mtl->setParam("u_spatialLut",					buf_spatialLut.renderResource());
			mtl->setParam("u_spatialLutKeyToStartIndex",	buf_patialLutKeyToStartIndex.renderResource());
		}

	private:
		void _create(FluidSim3D_Gpu* fs2d_, float dt_, RenderGraph* rdGraph_, DrawData* drawData_)
		{
			RDS_CORE_ASSERT(fs2d_);

			_fs2d		= fs2d_;
			rdGraph		= rdGraph_;
			drawData	= drawData_;
			dt			= dt_;

			screenSize		= drawData->resolution2u();
			particleCount	= _fs2d->_particleSpawner.particleCount;
		}

	private:
		FluidSim3D_Gpu*		_fs2d			= nullptr;
		//CachedSimArgs*	_cachedSimArgs	= nullptr;
	};

	RdgPass& addPass_simulateFluid3D(SimArgs& simArgs);
	RdgPass& addPass_renderFluidSim3D(CachedSimArgs& cachedSimArgs, const Config& simConfig, bool useCurSimRes, RdgTextureHnd rtColor, RdgTextureHnd dsBuf, RenderGraph* rdGraph, DrawData* drawData);

private:
	RdgPass& addPass_calcExternalForce(SimArgs& simArgs);
	RdgPass& addPass_updateSpatialLut(SimArgs& simArgs);
	RdgPass& addPass_calcDensityData(SimArgs& simArgs);
	RdgPass& addPass_calcPressureForce(SimArgs& simArgs);
	RdgPass& addPass_calcViscosity(SimArgs& simArgs);
	RdgPass& addPass_updatePosition(SimArgs& simArgs);

private:
	SPtr<Shader>	_shaderFs3d;
	SPtr<Material>	_mtlFs3d;

	GpuSort			_gpuSort;
	SpatialLut		_spatialLut;

	CachedSimArgs	_cachedSimArgs;
};

#endif


}