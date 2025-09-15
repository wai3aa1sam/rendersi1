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
	RenderUtil::createShader(&_shaderSort, "asset/shader/demo/fluid_simulation/common/rdsSimpleGpuSort.shader"); 
}

GpuSort::~GpuSort()	
{

}

RdgPass& 
GpuSort::sort(StrView name, RdgBufferHnd buf_list, u32 listSize, RenderGraph* rdGraph)
{
	_mtlSortPool.reset();
	return addPass_quickSort(name, buf_list, listSize, rdGraph);
	//return addPass_bubbleSort(name, buf_list, listSize, rdGraph);
}

RdgPass& 
GpuSort::addPass_quickSort(StrView name, RdgBufferHnd buf_list, u32 listSize, RenderGraph* rdGraph)
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