#include "rds_render_api_layer-pch.h"
#include "rdsLinearStagingBuffer.h"

namespace rds
{

#if 0
#pragma mark --- rdsLinearStagingBuffer-Impl ---
#endif // 0
#if 1

void 
LinearStagingBuffer::clear()
{
	auto data = _alloc.scopedULock();
	data->clear();
}

void 
LinearStagingBuffer::reset()
{
	RDS_TODO("LinearAllocator have a reset, no need to free those chunks");
	auto data = _alloc.scopedULock();
	data->clear();
}

void* 
LinearStagingBuffer::alloc(StagingHandle& oHnd, SizeType n)
{
	RDS_TODO("this design is not good, we must tie the allocation and the upload with a lock"
		"otherwise, when submit to other thread and destroy, the ptr will be danggling"
		"or we use a design that will confirm all the Job will be completed before submit if use this design"
	);

	SizeType chunkId	= StagingHandle::s_kInvalid;
	SizeType offset		= StagingHandle::s_kInvalid;
	void*	 buf		= nullptr;
	{
		auto data = _alloc.scopedULock();
		buf = data->alloc(&chunkId, &offset, n);
	}
	
	oHnd.create(chunkId, offset);
	return buf;
}

void*	
LinearStagingBuffer::uploadToBuffer(StagingHandle& oHnd, ByteSpan data)
{
	void* buf = alloc(oHnd, data.size());
	memory_copy(sCast<u8*>(buf), data.data(), data.size());
	return buf;
}

void
LinearStagingBuffer::uploadToDst(u8* dst, StagingHandle hnd, SizeType n)
{
	auto data = _alloc.scopedULock();
	memory_copy(dst, data->chunks()[hnd.chunkId]->data() + hnd.offset, n);
}

#endif

}