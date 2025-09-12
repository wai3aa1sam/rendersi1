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
	~FluidSim2D_Gpu()
	{

	}

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

		// debug
		SPtr<RenderGpuBuffer>	debug_positions						= nullptr;
		SPtr<RenderGpuBuffer>	debug_predictedPos					= nullptr;
		SPtr<RenderGpuBuffer>	debug_spatialLut					= nullptr;
		SPtr<RenderGpuBuffer>	debug_spatialLutKeyToStartIndex		= nullptr;

		RdgBufferHnd			debug_buf_positions;
		RdgBufferHnd			debug_buf_predictedPos;
		RdgBufferHnd			debug_buf_spatialLut;
		RdgBufferHnd			debug_buf_spatialLutKeyToStartIndex;

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
		static constexpr int s_kPassIdx_Cs_calcExternalForce				= 0;
		static constexpr int s_kPassIdx_Cs_updateSpatialLut					= 1;
		static constexpr int s_kPassIdx_Cs_updateSpatialLutKeyToStartIndex	= 2;
		static constexpr int s_kPassIdx_Cs_calcDensityData					= 3;
		static constexpr int s_kPassIdx_Cs_calcPressureForce				= 4;
		static constexpr int s_kPassIdx_Cs_calcViscosity					= 5;
		static constexpr int s_kPassIdx_Cs_updatePosition					= 6;

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
		RdgBufferHnd	bufSpatialLutKeyToStartIndex;

		// debug
		RdgBufferHnd	spatialLut_debug_buf_positions;
		//RdgBufferHnd	spatialLut_debug_buf_velocities;

		//RdgPass* pass_calcExternalForce		= nullptr;
		//RdgPass* pass_updateSpatialLut		= nullptr;
		//RdgPass* pass_calcDensityData		= nullptr;
		//RdgPass* pass_calcPressureForce		= nullptr;
		//RdgPass* pass_calcViscosity			= nullptr;
		//RdgPass* pass_updatePosition		= nullptr;

	public:
		void create(FluidSim2D_Gpu* fs2d_, float dt_, RenderGraph* rdGraph_, DrawData* drawData_)
		{
			_create(fs2d_, dt_, rdGraph_, drawData_);

			auto* cachedSimArgs = &_fs2d->_cachedSimArgs;
			auto n				= particleCount;
			auto gpuDim_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_DimT)	* n, sizeof(Gpu_DimT), RenderGpuBufferTypeFlags::Compute };
			auto gpuDim_CDesc_WithVs	= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_DimT)	* n, sizeof(Gpu_DimT), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex | RenderGpuBufferTypeFlags::TransferDst};
			auto gpuIdx_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_IdxT)	* n, sizeof(Gpu_IdxT), RenderGpuBufferTypeFlags::Compute};
			auto gpuIdx3_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_Idx3T) * n, sizeof(Gpu_Idx3T), RenderGpuBufferTypeFlags::Compute};

			if (cachedSimArgs->hasInit())
			{
				bufPos			= rdGraph->importBuffer(cachedSimArgs->positions->debugName(),		cachedSimArgs->positions);
				bufVel			= rdGraph->importBuffer(cachedSimArgs->velocities->debugName(),		cachedSimArgs->velocities);
				bufDensityData	= rdGraph->importBuffer(cachedSimArgs->densityData->debugName(),	cachedSimArgs->densityData);
				bufPredictedPos = rdGraph->importBuffer(cachedSimArgs->predictedPos->debugName(),	cachedSimArgs->predictedPos);
			}
			else
			{
				bufPos			= rdGraph->createBuffer("fs2d_bufPos",					gpuDim_CDesc_WithVs);
				bufVel			= rdGraph->createBuffer("fs2d_bufVel",					gpuDim_CDesc_WithVs);
				bufDensityData	= rdGraph->createBuffer("fs2d_bufDensityData",			gpuDim_CDesc);
				bufPredictedPos	= rdGraph->createBuffer("fs2d_bufPredictedPos",			gpuDim_CDesc);
			}

			// simple method
			#if 1
			// spatialLut
			bufSpatialLut					= rdGraph->createBuffer("fs2d_bufSpatialLut",					gpuIdx3_CDesc);
			bufSpatialLutKeyToStartIndex	= rdGraph->createBuffer("fs2d_bufSpatialLutKeyToStartIndex",	gpuIdx_CDesc);
			
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

			auto exportBufType	= RenderGpuBufferTypeFlags::Compute;	// add vertex flags too? but set pass.read also ok
			auto expBufAccess	= RenderAccess::Write;
			rdGraph->exportBuffer(&cachedSimArgs->positions,	bufPos,				exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->velocities,	bufVel,				exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->densityData,	bufDensityData,		exportBufType, expBufAccess);
			rdGraph->exportBuffer(&cachedSimArgs->predictedPos,	bufPredictedPos,	exportBufType, expBufAccess);
			cachedSimArgs->bufPos = bufPos;
			cachedSimArgs->bufVel = bufVel;
			
			// debug, cache for debug
			if (true)
			{
				//spatialLut_debug_buf_positions	= rdGraph->createBuffer("fs2d_spatialLut_debug_buf_positions",	gpuDim_CDesc_WithVs);

				rdGraph->exportBuffer(&cachedSimArgs->debug_predictedPos,				bufPredictedPos,					exportBufType, expBufAccess);
				rdGraph->exportBuffer(&cachedSimArgs->debug_spatialLut,					bufSpatialLut,						exportBufType, expBufAccess);
				rdGraph->exportBuffer(&cachedSimArgs->debug_spatialLutKeyToStartIndex,	bufSpatialLutKeyToStartIndex,		exportBufType, expBufAccess);
				//rdGraph->exportBuffer(&cachedSimArgs->debug_positions,					spatialLut_debug_buf_positions,		exportBufType, expBufAccess);

				//cachedSimArgs->debug_buf_positions					= spatialLut_debug_buf_positions;
				cachedSimArgs->debug_buf_predictedPos				= bufPredictedPos;
				cachedSimArgs->debug_buf_spatialLut					= bufSpatialLut;
				cachedSimArgs->debug_buf_spatialLutKeyToStartIndex	= bufSpatialLutKeyToStartIndex;
			}
			
		}

		void createOncePositionBuffer() const
		{
			if (hasInit())
				return;
			_fs2d->_particleSpawner.spawnTo(bufPos);
			_fs2d->_cachedSimArgs._hasInit = true;
		}

		void Debug_create(bool hasSim, CachedSimArgs& cached, FluidSim2D_Gpu* fs2d_, float dt_, RenderGraph* rdGraph_, DrawData* drawData_)
		{
			_create(fs2d_, dt_, rdGraph_, drawData_);

			auto n						= particleCount;
			auto gpuDim_CDesc_WithVs	= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_DimT)	* n, sizeof(Gpu_DimT), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex | RenderGpuBufferTypeFlags::TransferDst};
			spatialLut_debug_buf_positions = rdGraph->createBuffer("fs2d_spatialLut_debug_buf_positions",	gpuDim_CDesc_WithVs);

			bool  useCurSimRes = hasSim || !cached.predictedPos;
			auto initBuf = [&](auto& o, auto buf, SPtr<RenderGpuBuffer>& bufPtr) { o = useCurSimRes ? buf : rdGraph->importBuffer(bufPtr); };
			initBuf(bufPredictedPos,					cached.debug_buf_predictedPos,				cached.predictedPos);
			initBuf(bufSpatialLut,						cached.debug_buf_spatialLut,				cached.debug_spatialLut);
			initBuf(bufSpatialLutKeyToStartIndex,		cached.debug_buf_spatialLutKeyToStartIndex,	cached.debug_spatialLutKeyToStartIndex);
			//initBuf(spatialLut_debug_buf_positions,		cached.debug_buf_positions,					cached.debug_positions);
			//initBuf(spatialLut_debug_buf_velocities,	cached.debug_buf_velocities,				cached.debug_velocities);

			cached.debug_buf_positions = spatialLut_debug_buf_positions;
		}

	public:
		void readSpatialBuffer(RdgPass& pass)
		{
			pass.readBuffer(bufPredictedPos);
			pass.readBuffer(bufSpatialLut);
			pass.readBuffer(bufSpatialLutKeyToStartIndex);
		}

		void setSpatialParam(Material* mtl) const
		{
			mtl->setParam("u_predictedPositions",			bufPredictedPos.renderResource());
			mtl->setParam("u_spatialLut",					bufSpatialLut.renderResource());
			mtl->setParam("u_spatialLutKeyToStartIndex",	bufSpatialLutKeyToStartIndex.renderResource());
		}

	public:
		bool hasInit() const { return _fs2d->_cachedSimArgs.hasInit(); }

	private:
		void _create(FluidSim2D_Gpu* fs2d_, float dt_, RenderGraph* rdGraph_, DrawData* drawData_)
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
		FluidSim2D_Gpu*		_fs2d			= nullptr;
		//CachedSimArgs*	_cachedSimArgs	= nullptr;
	};

	RdgPass& addPass_simulateFluid2D(SimArgs& simArgs);

	RdgPass& addPass_calcExternalForce(SimArgs& simArgs);
	
	RdgPass& addPass_updateSpatialLut(SimArgs& simArgs);
	RdgPass& _addPass_sortSpatialLut(SimArgs& simArgs);
	RdgPass& Debug_addPass_debugSpatialLut(SimArgs& simArgs);

	RdgPass& addPass_calcDensityData(SimArgs& simArgs);
	RdgPass& addPass_calcPressureForce(SimArgs& simArgs);
	RdgPass& addPass_calcViscosity(SimArgs& simArgs);
	RdgPass& addPass_updatePosition(SimArgs& simArgs);

private:
	SPtr<Shader>	_shaderFs2d;
	SPtr<Material>	_mtlFs2d;

	SPtr<Shader>	_shaderSpatialLutDebug;
	SPtr<Material>	_mtlSpatialLutDebug;

	SPtr<Shader>	_shaderSort;
	MaterialPool	_mtlSortPool;
	SPtr<Material>	_mtlSort;

	SimArgs			_testSimArgs;
	CachedSimArgs	_cachedSimArgs;
};

#endif

}