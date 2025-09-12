#include "rds_fluid_simulation-pch.h"
#include "rdsSpatialLut.h"

#include "rdsGpuSort.h"
#include "rds_fluid_simulation/common/rdsFluidSim_ParticleDisplay.h"

namespace rds
{

#if 0
#pragma mark --- rdsSpatialLut-Decl ---
#endif // 0
#if 1

void 
SpatialLut::create2D(StrView name)
{
	_is3D = false;
	_name = name;
	RenderUtil::createMaterial(&_shaderSpatialLut,		&_mtlSpatialLut,		"asset/shader/demo/fluid_simulation/2d/rdsSpatialLut2D.shader");
	RenderUtil::createMaterial(&_debug.shaderSpatialLut, &_debug.mtlSpatialLut,	"asset/shader/demo/fluid_simulation/2d/rdsSpatialLut2D_Debug.shader");
}

void 
SpatialLut::create3D(StrView name)
{
	_is3D = true;
	_name = name;
	RenderUtil::createMaterial(&_shaderSpatialLut,		&_mtlSpatialLut,		"asset/shader/demo/fluid_simulation/3d/rdsSpatialLut3D.shader");
	RenderUtil::createMaterial(&_debug.shaderSpatialLut, &_debug.mtlSpatialLut,	"asset/shader/demo/fluid_simulation/3d/rdsSpatialLut3D_Debug.shader");
}

RdgPass& 
SpatialLut::updateSpatialLut(GpuSort& gpuSort, RdgBufferHnd buf_positions, float radius, u32 elementCount, RenderGraph* rdGraph)
{
	createBuffer(elementCount, rdGraph);

	RdgPass& pass_initSpatialLut					= addPass_initSpatialLut(buf_positions, radius, elementCount, rdGraph);
	RdgPass& pass_gpuSort							= gpuSort.sort(_name, buf_spatialLut, elementCount, rdGraph);
	RdgPass& pass_updateSpatialLutKeyToStartIndex	= addPass_updateSpatialLutKeyToStartIndex(radius, elementCount, rdGraph);

	pass_gpuSort.runAfter(&pass_initSpatialLut);
	pass_updateSpatialLutKeyToStartIndex.runAfter(&pass_gpuSort);

	return pass_updateSpatialLutKeyToStartIndex;
}

void 
SpatialLut::getBufferTo(RdgBufferHnd& o_buf_spatialLut, RdgBufferHnd& o_buf_spatialLutKeyToStartIndex)
{
	o_buf_spatialLut				= buf_spatialLut;
	o_buf_spatialLutKeyToStartIndex = buf_spatialLutKeyToStartIndex;
}

void 
SpatialLut::createBuffer(u32 elementCount, RenderGraph* rdGraph)
{
	auto n = elementCount;
	auto gpuIdx_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_IdxT)	* n, sizeof(Gpu_IdxT),  RenderGpuBufferTypeFlags::Compute};
	auto gpuIdx3_CDesc			= RenderGpuBuffer_CreateDesc{ sizeof(Gpu_Idx3T) * n, sizeof(Gpu_Idx3T), RenderGpuBufferTypeFlags::Compute};

	buf_spatialLut					= rdGraph->createBuffer(fmtAs_T<TempString>("{}_bufSpatialLut",					_name),	gpuIdx3_CDesc);
	buf_spatialLutKeyToStartIndex	= rdGraph->createBuffer(fmtAs_T<TempString>("{}_bufSpatialLutKeyToStartIndex",	_name),	gpuIdx_CDesc);

	// spatialLut
	//bufSpatialKeys		= rdGraph->createBuffer("fs2d_bufSpatialKeys",			gpuIdx_CDesc);
	//bufSpatialOffsets		= rdGraph->createBuffer("fs2d_bufSpatialOffsets",		gpuIdx_CDesc);
	//bufSortedIdxs			= rdGraph->createBuffer("fs2d_bufSortedIdxs",			gpuIdx_CDesc);
}

RdgPass& 
SpatialLut::addPass_initSpatialLut(RdgBufferHnd buf_positions, float radius, u32 elementCount, RenderGraph* rdGraph)
{
	Material* mtl	= _mtlSpatialLut;

	auto& pass = rdGraph->addPass(RDS_RDG_EVENT_NAME("{}_initSpatialLut", _name), RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.readBuffer(buf_positions);
	pass.writeBuffer(buf_spatialLut);
	pass.writeBuffer(buf_spatialLutKeyToStartIndex);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_radius",						radius);
			mtl->setParam("u_elementCount",					elementCount);

			mtl->setParam("u_positions",					buf_positions.renderResource());
			mtl->setParam("u_spatialLut",					buf_spatialLut.renderResource());
			mtl->setParam("u_spatialLutKeyToStartIndex",	buf_spatialLutKeyToStartIndex.renderResource());
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, s_kPassIdx_Cs_initSpatialLut, Vec3u{elementCount, 1, 1});
		}
	);
	return pass;
}

RdgPass& 
SpatialLut::addPass_updateSpatialLutKeyToStartIndex(float radius, u32 elementCount, RenderGraph* rdGraph)
{
	Material*	mtl		= _mtlSpatialLut;

	auto& pass = rdGraph->addPass("fs2d_updateSpatialLutKeyToStartIndex", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.readBuffer(buf_spatialLut);
	pass.writeBuffer(buf_spatialLutKeyToStartIndex);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_radius",						radius);
			mtl->setParam("u_elementCount",					elementCount);

			mtl->setParam("u_spatialLut",					buf_spatialLut.renderResource());
			mtl->setParam("u_spatialLutKeyToStartIndex",	buf_spatialLutKeyToStartIndex.renderResource());
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, s_kPassIdx_Cs_updateSpatialLutKeyToStartIndex, Vec3u{elementCount, 1, 1});
		}
	);

	return pass;
}

#if 1


RdgPass& 
SpatialLut::Debug_updateSpatialLut(bool hasSimulated, Vec3f samplingPt, GpuSort& gpuSort, RdgBufferHnd buf_positions, float radius, u32 elementCount, RenderGraph* rdGraph)
{
	// must not use isValidRenderResource now (see the code in there)
	//if (!buf_spatialLut.isValidRdgResource())
	if (!hasSimulated)
		updateSpatialLut(gpuSort, buf_positions, radius, elementCount, rdGraph);

	#if 1
	auto n = elementCount;
	auto gpuDim_CDesc = is2D() ? 
		RenderGpuBuffer_CreateDesc{ sizeof(Gpu_Dim2T) * n, sizeof(Gpu_Dim2T), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex}
	: RenderGpuBuffer_CreateDesc{ sizeof(Gpu_Dim3T) * n, sizeof(Gpu_Dim3T), RenderGpuBufferTypeFlags::Compute | RenderGpuBufferTypeFlags::Vertex};
	_debug.buf_resultPositions = rdGraph->createBuffer(fmtAs_T<TempString>("{}_debug_buf_resultPositions", _name),	gpuDim_CDesc);
	#endif // 1

	RdgPass& pass_debugSpatialLut = Debug_addPass_debugSpatialLut(buf_positions, samplingPt, radius, elementCount, rdGraph);
	return pass_debugSpatialLut;
}

RdgPass& 
SpatialLut::Debug_addPass_debugSpatialLut(RdgBufferHnd buf_positions, Vec3f samplingPt, float radius, u32 elementCount, RenderGraph* rdGraph)
{
	Material* mtl	= _debug.mtlSpatialLut;
	auto& pass = rdGraph->addPass("fs2d_debugSpatialLut", RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(_debug.buf_resultPositions);
	pass.readBuffer(buf_positions);
	pass.readBuffer(buf_spatialLut);
	pass.readBuffer(buf_spatialLutKeyToStartIndex);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			if (is2D())
				mtl->setParam("u_samplingPt",	samplingPt.toVec2());
			else
				mtl->setParam("u_samplingPt",	samplingPt);
			mtl->setParam("u_particleCount",	elementCount);
			mtl->setParam("u_smoothingRadius",	radius);

			mtl->setParam("u_positions",						buf_positions.renderResource());
			mtl->setParam("u_spatialLut",						buf_spatialLut.renderResource());
			mtl->setParam("u_spatialLutKeyToStartIndex",		buf_spatialLutKeyToStartIndex.renderResource());
			mtl->setParam("u_spatialLutDebugResultPositions",	_debug.buf_resultPositions.renderResource());

			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, Vec3u{1, 1, 1});
		}
	);
	return pass;
}


RdgPass& 
SpatialLut::Debug_renderSpatialLut(FluidSim_ParticleDisplay& ptcDisplay, RdgTextureHnd rtColor, RdgTextureHnd dsBuf, float particleSize, u32 elementCount, RenderGraph* rdGraph, DrawData* drawData)
{
	auto n			= elementCount;
	auto buf_pos	= _debug.buf_resultPositions;

	auto& pass = rdGraph->addPass("fs2d_SpatialLut_renderDebugPositions", RdgPassTypeFlags::Graphics);
	pass.setRenderTarget(rtColor,	RenderTargetLoadOp::Load, RenderTargetStoreOp::Store);
	pass.setDepthStencil(dsBuf,	RdgAccess::Write, RenderTargetLoadOp::Load, RenderTargetLoadOp::Load);	// currently use the pre-pass will cause z-flight
	pass.readBuffer(buf_pos, RenderGpuBufferTypeFlags::Vertex, ShaderStageFlag::Vertex);
	pass.setExecuteFunc(
		[=, &ptcDisplay](RenderRequest& rdReq)
		{
			rdReq.reset(rdGraph->renderContext(), drawData, drawData->lineMaterial());
			auto* gpuBuf = buf_pos.renderResource();
			ptcDisplay.debug_draw(rdReq, drawData, gpuBuf, gpuBuf, particleSize, n);
		}
	);
	return pass;
}

#endif // 1


#endif

}