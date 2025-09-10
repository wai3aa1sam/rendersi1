#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"
#include "rdsFluidSim2D_Base.h"

namespace rds
{

#if 0
#pragma mark --- rdsFluidSim2D_Gpu-Decl ---
#endif // 0
#if 1

class FluidSim2D_Gpu : public FluidSim2D_Base
{
public:
	using Base = FluidSim2D_Base;

	using DimT		= Vec2f;
	using DimT_i	= Vec2i;
	using DimT_u	= Vec2u;
	using IdxT		= u32;
	using HashT		= IdxT;

public:
	virtual void onCreate(GraphicsDemo* parentDemo)						override;

	virtual void onUpdate(float dt)										override;

	virtual void onPrepareRender(RenderPassPipeline* renderPassPipeline) override;
	virtual void onExecuteRender(RenderPassPipeline* renderPassPipeline) override;

	virtual void onDrawGui(EditorUiDrawRequest& uiDrawReq) override;

	virtual void onUiMouseEvent(	UiMouseEvent&		ev) override;
	virtual void onUiKeyboardEvent(	UiKeyboardEvent&	ev) override;

public:
	void	update(float dt);
	void	simulate(float dt);

public:
	struct CachedSimArgs
	{
	public:
		SPtr<RenderGpuBuffer>	bufPos			= nullptr;
		SPtr<RenderGpuBuffer>	bufVel			= nullptr;
		SPtr<RenderGpuBuffer>	bufDensityData	= nullptr;
		SPtr<RenderGpuBuffer>	bufPredictedPos	= nullptr;

	public:
		bool hasInit() const { return _hasInit; }

	public:
		bool _hasInit = false;		// RDS_TODO("later let the rdg buffer init data, then no need this flags");
	};

	struct SimArgs
	{
	public:
		using Gpu_DimT	= Tuple2f;
		using Gpu_IdxT	= u32;
		using Gpu_Idx3T	= Tuple3u;

	public:
		// TODO: get pass id by compute string
		static constexpr int s_kPassIdx_Cs_calcExternalForce	= 0;
		static constexpr int s_kPassIdx_Cs_updateSpatialLut		= 1;
		static constexpr int s_kPassIdx_Cs_calcDensityData		= 2;
		static constexpr int s_kPassIdx_Cs_calcViscosity		= 3;
		static constexpr int s_kPassIdx_Cs_calcPressureForce	= 4;
		static constexpr int s_kPassIdx_Cs_updatePosition		= 5;

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

		// spatialLut
		RdgBufferHnd	bufSpatialLut;
		RdgBufferHnd	bufSpatialOffsets;

	public:
		void create(FluidSim2D_Gpu* fs2d_, float dt_, RenderGraph* rdGraph_, DrawData* drawData_)
		{
			RDS_CORE_ASSERT(fs2d_);

			_fs2d		= fs2d_;
			rdGraph		= rdGraph_;
			drawData	= drawData_;
			dt			= dt_;

			auto* cachedSimArgs = &_fs2d->_cachedSimArgs;

			screenSize		= drawData->resolution2u();
			particleCount	= _fs2d->_particleSpawner.particleCount;

			auto n = particleCount;
			auto gpuDim_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_DimT)	* n, sizeof(Gpu_DimT), RenderGpuBufferTypeFlags::Compute };
			auto gpuDim_CDesc_WithVs	= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_DimT)	* n, sizeof(Gpu_DimT), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex | RenderGpuBufferTypeFlags::TransferDst};
			auto gpuIdx_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_IdxT)	* n, sizeof(Gpu_IdxT), RenderGpuBufferTypeFlags::Compute};
			auto gpuIdx3_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_Idx3T) * n, sizeof(Gpu_Idx3T), RenderGpuBufferTypeFlags::Compute};

			if (cachedSimArgs->hasInit())
			{
				bufPos			= rdGraph->importBuffer(cachedSimArgs->bufPos->debugName(),				cachedSimArgs->bufPos);
				bufVel			= rdGraph->importBuffer(cachedSimArgs->bufVel->debugName(),				cachedSimArgs->bufVel);
				bufDensityData	= rdGraph->importBuffer(cachedSimArgs->bufDensityData->debugName(),		cachedSimArgs->bufDensityData);
				bufPredictedPos = rdGraph->importBuffer(cachedSimArgs->bufPredictedPos->debugName(),	cachedSimArgs->bufPredictedPos);
			}
			else
			{
				bufPos				= rdGraph->createBuffer("fs2d_bufPos",					gpuDim_CDesc_WithVs);
				bufVel				= rdGraph->createBuffer("fs2d_bufVel",					gpuDim_CDesc_WithVs);
				bufDensityData		= rdGraph->createBuffer("fs2d_bufDensityData",			gpuDim_CDesc);
				bufPredictedPos		= rdGraph->createBuffer("fs2d_bufPredictedPos",			gpuDim_CDesc);
			}

			// simple method
			#if 1
			// spatialLut
			bufSpatialLut			= rdGraph->createBuffer("fs2d_bufSpatialLut",			gpuIdx3_CDesc);
			bufSpatialOffsets		= rdGraph->createBuffer("fs2d_bufSpatialOffsets",		gpuIdx_CDesc);

			#else
			// spatialLut
			bufSpatialKeys			= rdGraph->createBuffer("fs2d_bufSpatialKeys",			gpuIdx_CDesc);
			bufSpatialOffsets		= rdGraph->createBuffer("fs2d_bufSpatialOffsets",		gpuIdx_CDesc);
			bufSortedIdxs			= rdGraph->createBuffer("fs2d_bufSortedIdxs",			gpuIdx_CDesc);

			// sortTarget
			bufSortedPos			= rdGraph->createBuffer("fs2d_bufSortedPos",			gpuDim_CDesc);
			bufSortedPredictedPos	= rdGraph->createBuffer("fs2d_bufSortedPredictedPos",	gpuDim_CDesc);
			bufSortedVel			= rdGraph->createBuffer("fs2d_bufSortedVel",			gpuDim_CDesc);
			#endif // 0

			auto exportBufType	= RenderGpuBufferTypeFlags::Compute;
			auto expBufAccess	= RenderAccess::Write;
			rdGraph->exportBuffer(&cachedSimArgs->bufPos,			bufPos,				exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->bufVel,			bufVel,				exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->bufDensityData,	bufDensityData,		exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->bufPredictedPos,	bufPredictedPos,	exportBufType, expBufAccess);
		}

		void createOncePositionBuffer()
		{
			if (hasInit())
				return;
			_fs2d->_particleSpawner.spawnTo(bufPos);
			_fs2d->_cachedSimArgs._hasInit = true;
		}

	public:
		bool hasInit() const { return _fs2d->_cachedSimArgs.hasInit(); }

	private:
		FluidSim2D_Gpu*		_fs2d			= nullptr;
		//CachedSimArgs*	_cachedSimArgs	= nullptr;
	};

	void addPass_simulateFluid2D(SimArgs& simArgs);

	void addPass_calcExternalForce(SimArgs& simArgs);
	
	void addPass_updateSpatialLut(SimArgs& simArgs);
	void _addPass_sortSpatialLut(SimArgs& simArgs);
	void debug_addPass_renderSpatialLut(SimArgs& simArgs);

	void addPass_calcDensityData(SimArgs& simArgs);
	void addPass_calcViscosity(SimArgs& simArgs);
	void addPass_calcPressureForce(SimArgs& simArgs);
	void addPass_updatePosition(SimArgs& simArgs);

private:
	SPtr<Shader>	_shaderFs2d;
	SPtr<Material>	_mtlFs2d;

	CachedSimArgs	_cachedSimArgs;
};

#endif

}