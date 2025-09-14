#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"
#include "rds_fluid_simulation/common/rdsGpuSort.h"

namespace rds
{

class GpuSort;
class FluidSim_ParticleDisplay;

#if 0
#pragma mark --- rdsSpatialLut-Decl ---
#endif // 0
#if 1

class SpatialLut : public NonCopyable
{
public:
	static constexpr int s_kPassIdx_Cs_initSpatialLut					= 0;
	static constexpr int s_kPassIdx_Cs_updateSpatialLutKeyToStartIndex	= 1;

public:
	using Gpu_IdxT	= u32;
	using Gpu_Idx3T	= Tuple3u;
	using Gpu_Dim2T	= Tuple2f;
	using Gpu_Dim3T	= Tuple3f;

public:
	void create2D(StrView name);
	void create3D(StrView name);

public:
	RdgPass& updateSpatialLut(GpuSort& gpuSort, RdgBufferHnd buf_positions, float radius, u32 elementCount, RenderGraph* rdGraph);
	void getBufferTo(RdgBufferHnd& o_buf_spatialLut, RdgBufferHnd& o_buf_spatialLutKeyToStartIndex);

public:
	bool is2D() const;
	bool is3D() const;

public:
	#if 1
	RdgPass& Debug_updateSpatialLut(bool hasSimulated, Vec3f samplingPt, GpuSort& gpuSort, RdgBufferHnd buf_positions, float radius, u32 elementCount, RenderGraph* rdGraph);
	RdgPass& Debug_addPass_debugSpatialLut(RdgBufferHnd buf_positions, Vec3f samplingPt, float radius, u32 elementCount, RenderGraph* rdGraph);
	RdgPass& Debug_renderSpatialLut(FluidSim_ParticleDisplay& ptcDisplay, RdgTextureHnd rtColor, RdgTextureHnd dsBuf, Vec3f pos, float particleSize, u32 elementCount, RenderGraph* rdGraph, DrawData* drawData);
	#endif // 0

private:
	void createBuffer(u32 elementCount, RenderGraph* rdGraph);

	RdgPass& addPass_initSpatialLut(RdgBufferHnd buf_positions, float radius, u32 elementCount, RenderGraph* rdGraph);
	RdgPass& addPass_updateSpatialLutKeyToStartIndex(float radius, u32 elementCount, RenderGraph* rdGraph);

private:
	String _name;

	SPtr<Shader>	_shaderSpatialLut;
	SPtr<Material>	_mtlSpatialLut;

	RdgBufferHnd buf_spatialLut;
	RdgBufferHnd buf_spatialLutKeyToStartIndex;
	//GpuSort _gpuSort;

	bool _is3D = false;

	struct Debug
	{
		SPtr<Shader>				shaderSpatialLut;
		SPtr<Material>				mtlSpatialLut;
		RdgBufferHnd				buf_resultPositions;
	} _debug;
};

inline bool SpatialLut::is2D() const { return !is3D(); }
inline bool SpatialLut::is3D() const { return _is3D; }

#endif

}