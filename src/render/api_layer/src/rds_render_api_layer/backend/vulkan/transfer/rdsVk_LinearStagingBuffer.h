#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rds_render_api_layer/transfer/rdsLinearStagingBuffer.h"


#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class RenderDevice_Vk;
class TransferContext;
class Vk_Allocator;


#if 0
#pragma mark --- rdsVk_LinearStagingBuffer-Decl ---
#endif // 0
#if 1

class Vk_LinearStagingBuffer : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	struct Chunk
	{
		RDS_RENDER_API_LAYER_COMMON_BODY();
	public:
		Chunk(SizeType n, Vk_Allocator* alloc, RenderDevice_Vk* rdDevVk);
		~Chunk();

		StagingHandle	alloc(SizeType n);
		void			reset();

	public:
		void*			mappedData()		{ return _mappedData; }
		Vk_Buffer_T*	vkStagingBufHnd()	{ return _buf.hnd(); }

	private:
		Vk_Buffer	_buf;
		void*		_mappedData = nullptr;
		u32			_offset		= 0;
		u32			_size		= 0;
	};

	class Chunks : public NonCopyable
	{
		RDS_RENDER_API_LAYER_COMMON_BODY();
	public:
		Chunks();
		~Chunks();

		StagingHandle alloc(SizeType n, Vk_Allocator* vkAlloc, RenderDevice_Vk* rdDevVk);

		void reset();
		void clear();

	public:
		Chunk*				currentChunk();
		Vk_Buffer_T*		vkStagingBufHnd(StagingHandle hnd);
		template<class T> T mappedData(		StagingHandle hnd);

	protected:
		Chunk* makeNewChunk(SizeType n, Vk_Allocator* vkAlloc, RenderDevice_Vk* rdDevVk);

	private:
		Vector<UPtr<Chunk>, 8>	_chunks;
		u32						_chunkSize		= 0;
		u32						_curChunkIdx	= 0;
	};

public:
	Vk_LinearStagingBuffer();
	~Vk_LinearStagingBuffer();

	void create(RenderDevice_Vk* rdDevVk);
	void destroy();

	StagingHandle alloc(SizeType size);

	void reset();
	void clear();

public:
	template<class T> T mappedData(		StagingHandle hnd);
	Vk_Buffer_T*		vkStagingBufHnd(StagingHandle hnd);

private:
	RenderDevice_Vk*		_rdDevVk = nullptr;
	Vk_Allocator			_vkAlloc;
	SMutexProtected<Chunks>	_chunks;
};

template<class T> inline
T 
Vk_LinearStagingBuffer::Chunks::mappedData(StagingHandle hnd)
{
	hnd.checkValid();

	auto& chunk = _chunks[hnd.chunkId];

	void* mappedData = chunk->mappedData();
	void* dst = sCast<u8*>(mappedData) + hnd.offset;
	return sCast<T>(dst);
}

template<class T> inline
T 
Vk_LinearStagingBuffer::mappedData(StagingHandle hnd)
{
	auto data = _chunks.scopedSLock();
	return data->mappedData<T>(hnd);
}

#endif




}


#endif