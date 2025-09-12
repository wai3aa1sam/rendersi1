#pragma once

#include "rds_fluid_simulation/common/rds_fluid_simulation_common.h"

namespace rds
{

#if 0
#pragma mark --- rdsGpuSort-Decl ---
#endif // 0
#if 1

class GpuSort
{
public:
	GpuSort();
	~GpuSort();

private:
	struct SortArgs
	{
	public:
		StrView			name;
		RdgBufferHnd	buf_list;
		u32				listSize;
		RenderGraph*	rdGraph;

	public:
		void create(StrView name_, RdgBufferHnd buf_list_, u32 listSize_, RenderGraph* rdGraph_)
		{
			name		= name_;
			buf_list	= buf_list_;
			listSize	= listSize_;
			rdGraph		= rdGraph_;
		}
	};

public:
	RdgPass& sort(StrView name, RdgBufferHnd buf_list, u32 listSize, RenderGraph* rdGraph);

private:
	RdgPass& addPass_bitonicMergeSort(StrView name, RdgBufferHnd buf_list, u32 listSize, RenderGraph* rdGraph);
	RdgPass& addPass_bubbleSort(StrView name, RdgBufferHnd buf_list, u32 listSize, RenderGraph* rdGraph);

private:
	SPtr<Shader>	_shaderSort;
	MaterialPool	_mtlSortPool;
	//SPtr<Material>	_mtlSort;
};
#endif


}