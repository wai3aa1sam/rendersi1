#include "rds_fluid_simulation-pch.h"
#include "rdsGpuSort.h"

namespace rds
{

#if 0
#pragma mark --- rdsGpuSort-Decl ---
#endif // 0
#if 1

GpuSort::GpuSort()	
{ 
	//RenderUtil::createShader(&_shaderSort, "asset/shader/demo/fluid_simulation/common/rdsSimpleGpuSort.shader"); 
	RenderUtil::createShader(RDS_DebugLabel(), &_shaderSort, "asset/shader/demo/fluid_simulation/common/rdsBitonicMergeSort.shader"); 
}

GpuSort::~GpuSort()	
{

}

RdgPass& 
GpuSort::sort(StrView name, RdgBufferHnd buf_list, u32 listSize, RenderGraph* rdGraph)
{
	_mtlSortPool.reset();

	//SortArgs sortArgs;
	//sortArgs.create(name, buf_list, listSize, rdGraph);

	return addPass_bitonicMergeSort(name, buf_list, listSize, rdGraph);
	//return addPass_bubbleSort(name, buf_list, listSize, rdGraph);
}

RdgPass& 
GpuSort::addPass_bitonicMergeSort(StrView name, RdgBufferHnd buf_list, u32 listSize, RenderGraph* rdGraph)
{
	// references:
	// - https://github.com/SebLague/Fluid-Sim/blob/Episode-01/Assets/Scripts/Compute%20Helpers/GPU%20Sort/GPUSort.cs
	// Sorts given buffer of integer values using bitonic merge sort
	// Note: buffer size is not restricted to powers of 2 in this implementation
	auto size			= sCast<u32>(listSize);

	// Launch each step of the sorting algorithm (once the previous step is complete)
	// Number of steps = [log2(n) * (log2(n) + 1)] / 2
	// where n = nearest power of 2 that is greater or equal to the number of inputs

	RdgPass*	pass_prev = nullptr;
	u32			numStages = sCast<u32>(math::log2(math::nextPow2(size)));
	for (u32 stageIndex = 0; stageIndex < numStages; stageIndex++)
	{
		for (u32 stepIndex = 0; stepIndex < stageIndex + 1; stepIndex++)
		{
			//SPtr<Material> _mtl = Renderer::renderDevice()->createMaterial(_shaderSort);
			//Material* mtl = _mtl;
			//Material* mtl = _mtlSort;

			Material* mtl = _mtlSortPool.newObject(_shaderSort);

			auto& pass = rdGraph->addPass(RDS_RDG_EVENT_NAME("{}_bitonicMergeSort_stg{}_stp{}", name, stageIndex, stepIndex)
				, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
			pass.setDebugLabelGroup("bitonicMergeSort");
			pass.writeBuffer(buf_list);
			pass.setExecuteFunc(
				[=](RenderRequest& rdReq)
				{
					// Calculate some pattern stuff
					u32 groupWidth  = 1 << (stageIndex - stepIndex);
					u32 groupHeight = 2 * groupWidth - 1;

					mtl->setParam("u_groupWidth",	groupWidth);
					mtl->setParam("u_groupHeight",	groupHeight);
					mtl->setParam("u_stepIndex",	stepIndex);
					mtl->setParam("u_size",			size);
					mtl->setParam("u_list",			buf_list.renderResource());

					//RDS_DUMP_VAR(stageIndex, stepIndex, groupWidth, groupHeight);
					rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 0, Vec3u{sCast<u32>(math::nextPow2(size) / 2), 1, 1});
				}
			);
			pass.runAfter(pass_prev);
			pass_prev = &pass;
		}
	}

	return *pass_prev;
}

RdgPass& 
GpuSort::addPass_bubbleSort(StrView name, RdgBufferHnd buf_list, u32 listSize, RenderGraph* rdGraph)
{
	auto size			= sCast<u32>(listSize);

	Material* mtl = _mtlSortPool.newObject(_shaderSort);
	auto& pass = rdGraph->addPass(RDS_RDG_EVENT_NAME("{}_bubble_sort", name)
		, RdgPassTypeFlags::Graphics | RdgPassTypeFlags::Compute);
	pass.writeBuffer(buf_list);
	pass.setExecuteFunc(
		[=](RenderRequest& rdReq)
		{
			mtl->setParam("u_size",	size);
			mtl->setParam("u_list",	buf_list.renderResource());
			rdReq.dispatchExactThreadGroups(RDS_SRCLOC, mtl, 1, Vec3u{1, 1, 1});
		}
	);
	return pass;
}

#endif

}
