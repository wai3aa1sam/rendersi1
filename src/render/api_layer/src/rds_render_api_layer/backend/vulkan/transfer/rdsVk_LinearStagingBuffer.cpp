#include "rds_render_api_layer-pch.h"
#include "rdsVk_LinearStagingBuffer.h"
#include "rds_render_api_layer/backend/vulkan/rdsRenderDevice_Vk.h"

#include "rds_render_api_layer/rdsRenderer.h"

#if RDS_RENDER_HAS_VULKAN

namespace rds
{

#if 0
#pragma mark --- rdsVk_LinearStagingBuffer::Chunk-Impl ---
#endif // 0
#if 1

Vk_LinearStagingBuffer::Chunk::Chunk(SizeType n, Vk_Allocator* alloc, RenderDevice_Vk* rdDevVk)
{
	Vk_AllocInfo allocInfo = {};
	allocInfo.usage = RenderMemoryUsage::AutoPreferCpu;
	allocInfo.flags = RenderAllocFlags::PersistentMapped | RenderAllocFlags::HostWrite;

	_buf.create(rdDevVk, alloc, &allocInfo, n, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, QueueTypeFlags::Transfer);
	_mappedData = allocInfo.outMappedData;
	_size = sCast<u32>(n);
}

Vk_LinearStagingBuffer::Chunk::~Chunk()
{
	_buf.destroy();
}

StagingHandle 
Vk_LinearStagingBuffer::Chunk::alloc(SizeType n)
{
	u32 align = 64;

	auto o = StagingHandle{};

	auto newOffset = sCast<u32>(math::alignTo(_offset, align));
	auto available = _size - newOffset;
	if (n > available)
	{
		return o;
	}

	o.offset = newOffset;
	_offset = sCast<u32>(newOffset + n);
	return o;
}

void 
Vk_LinearStagingBuffer::Chunk::reset()
{
	_offset = 0;
}


#endif

#if 0
#pragma mark --- rdsVk_LinearStagingBuffer::Chunks-Impl ---
#endif // 0
#if 1

Vk_LinearStagingBuffer::Chunks::Chunks()
{
	_chunkSize = math::MBToByte<u32>(128);
}

Vk_LinearStagingBuffer::Chunks::~Chunks()
{

}

StagingHandle 
Vk_LinearStagingBuffer::Chunks::alloc(SizeType n, Vk_Allocator* vkAlloc, RenderDevice_Vk* rdDevVk)
{
	if (_chunks.is_empty())
	{
		makeNewChunk(n, vkAlloc, rdDevVk);
	}

	auto* curChunk = currentChunk();
	RDS_CORE_ASSERT(curChunk);

	auto hnd = curChunk->alloc(n);
	if (!hnd.isOffsetValid())
	{
		curChunk = makeNewChunk(n, vkAlloc, rdDevVk);
		hnd = curChunk->alloc(n);
		hnd.checkOffsetValid();
	}

	hnd.chunkId = _curChunkIdx;
	hnd.checkValid();
	return hnd;
}

void 
Vk_LinearStagingBuffer::Chunks::clear()
{
	_chunks.clear();
	_curChunkIdx = 0;
}

void 
Vk_LinearStagingBuffer::Chunks::reset()
{
	for (auto& e : _chunks)
	{
		e->reset();
	}
	_curChunkIdx = 0;
}

Vk_Buffer_T* 
Vk_LinearStagingBuffer::Chunks::vkStagingBufHnd(StagingHandle hnd)
{
	hnd.checkValid();
	auto& chunk = _chunks[hnd.chunkId];
	return chunk->vkStagingBufHnd();
}

Vk_LinearStagingBuffer::Chunk*
Vk_LinearStagingBuffer::Chunks::currentChunk()
{
	return !_chunks.is_empty() ? _chunks[_curChunkIdx].ptr() : nullptr;
}

Vk_LinearStagingBuffer::Chunk* 
Vk_LinearStagingBuffer::Chunks::makeNewChunk(SizeType n, Vk_Allocator* vkAlloc, RenderDevice_Vk* rdDevVk)
{
	auto reqSize = math::max(n, sCast<SizeType>(_chunkSize));
	auto& chunk = _chunks.emplace_back(makeUPtr<Chunk>(reqSize, vkAlloc, rdDevVk));
	_curChunkIdx = sCast<u32>(_chunks.size() - 1);
	return chunk;
}


#endif

#if 0
#pragma mark --- rdsVk_LinearStagingBuffer-Impl ---
#endif // 0
#if 1

Vk_LinearStagingBuffer::Vk_LinearStagingBuffer()
{

}

Vk_LinearStagingBuffer::~Vk_LinearStagingBuffer()
{
	destroy();
}

void 
Vk_LinearStagingBuffer::create(RenderDevice_Vk* rdDevVk)
{
	destroy();

	_rdDevVk = rdDevVk;
	_vkAlloc.create(rdDevVk);
}

void 
Vk_LinearStagingBuffer::destroy()
{
	if (!_rdDevVk)
		return;
	{
		auto data = _chunks.scopedULock();
		data->clear();
	}

	_vkAlloc.destroy();
	_rdDevVk = nullptr;
}

StagingHandle 
Vk_LinearStagingBuffer::alloc(SizeType size)
{
	RDS_CORE_ASSERT(_rdDevVk, "not yet create()");

	auto data = _chunks.scopedULock();
	auto hnd = data->alloc(size, &_vkAlloc, _rdDevVk);
	return hnd;
}

void 
Vk_LinearStagingBuffer::reset()
{
	auto data = _chunks.scopedULock();
	data->reset();
}

void 
Vk_LinearStagingBuffer::clear()
{
	auto data = _chunks.scopedULock();
	data->clear();
}

Vk_Buffer_T* 
Vk_LinearStagingBuffer::vkStagingBufHnd(StagingHandle hnd)
{
	auto data = _chunks.scopedSLock();
	auto* bufHnd = data->vkStagingBufHnd(hnd);
	return bufHnd;
}



#endif

}


#endif