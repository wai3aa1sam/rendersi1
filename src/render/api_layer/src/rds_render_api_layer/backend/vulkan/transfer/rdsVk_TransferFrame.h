#pragma once

#include "rds_render_api_layer/backend/vulkan/common/rdsVk_RenderApi_Common.h"
#include "rds_render_api_layer/backend/vulkan/rdsVk_Allocator.h"
#include "rds_render_api_layer/backend/vulkan/command/rdsVk_CommandPool.h"

#include "rds_render_api_layer/command/rdsTransferCommand.h"


#if RDS_RENDER_HAS_VULKAN

namespace rds
{

class TransferContext;

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
	public:
		Chunk(SizeType n, Vk_Allocator* alloc, RenderDevice_Vk* rdDevVk)
		{
			Vk_AllocInfo allocInfo = {};
			allocInfo.usage = RenderMemoryUsage::AutoPreferCpu;
			allocInfo.flags = RenderAllocFlags::PersistentMapped | RenderAllocFlags::HostWrite;

			_buf.create(rdDevVk, alloc, &allocInfo, n, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, QueueTypeFlags::Transfer);
			_mappedData = allocInfo.outMappedData;
			_size = sCast<u32>(n);
		}

		~Chunk()
		{
			_buf.destroy();
		}

		StagingHandle alloc(SizeType n)
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

		void reset()
		{
			_offset = 0;
		}

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
	public:
		Chunks();
		~Chunks();

		StagingHandle alloc(SizeType n, Vk_Allocator* vkAlloc, RenderDevice_Vk* rdDevVk)
		{
			if (_chunks.is_empty())
			{
				auto reqSize = math::max(n, sCast<SizeType>(_chunkSize));
				_chunks.emplace_back(makeUPtr<Chunk>(reqSize, vkAlloc, rdDevVk));
				_curChunkIdx = sCast<u32>(_chunks.size() - 1);
			}

			auto* curChunk = currentChunk();
			RDS_CORE_ASSERT(curChunk);

			auto hnd = curChunk->alloc(n);
			hnd.chunkId = _curChunkIdx;
			return hnd;
		}

		void clear()
		{
			_chunks.clear();
			_curChunkIdx = 0;
		}

		void reset()
		{
			for (auto& e : _chunks)
			{
				e->reset();
			}
			_curChunkIdx = 0;
		}

		Vk_Buffer_T* vkStagingBufHnd(StagingHandle hnd)
		{
			hnd.checkValid();
			auto& chunk = _chunks[hnd.chunkId];
			return chunk->vkStagingBufHnd();
		}

		template<class T>
		T mappedData(StagingHandle hnd)
		{
			hnd.checkValid();

			auto& chunk = _chunks[hnd.chunkId];

			void* mappedData = chunk->mappedData();
			void* dst = sCast<u8*>(mappedData) + hnd.offset;
			return sCast<T>(dst);
		}

		Chunk* currentChunk()
		{
			return !_chunks.is_empty() ? _chunks[_curChunkIdx].ptr() : nullptr;
		}

	private:
		Vector<UPtr<Chunk>, 8>	_chunks;
		u32						_chunkSize		= 0;
		u32						_curChunkIdx	= 0;
	};

public:
	Vk_LinearStagingBuffer()
	{
		
	}

	~Vk_LinearStagingBuffer()
	{
		destroy();
	}

	void create(RenderDevice_Vk* rdDevVk)
	{
		destroy();

		_rdDevVk = rdDevVk;
		_vkAlloc.create(rdDevVk);
	}

	void destroy()
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

	StagingHandle alloc(SizeType size)
	{
		RDS_CORE_ASSERT(_rdDevVk, "not yet create()");

		auto data = _chunks.scopedULock();
		auto hnd = data->alloc(size, &_vkAlloc, _rdDevVk);
		return hnd;
	}

	void reset()
	{
		auto data = _chunks.scopedULock();
		data->reset();
	}

	void clear()
	{
		auto data = _chunks.scopedULock();
		data->clear();
	}

	template<class T>
	T mappedData(StagingHandle hnd)
	{
		auto data = _chunks.scopedSLock();
		return data->mappedData<T>(hnd);
	}

	Vk_Buffer_T* vkStagingBufHnd(StagingHandle hnd)
	{
		auto data = _chunks.scopedSLock();
		auto* bufHnd = data->vkStagingBufHnd(hnd);
		return bufHnd;
	}

private:
	RenderDevice_Vk*		_rdDevVk = nullptr;
	Vk_Allocator			_vkAlloc;
	SMutexProtected<Chunks>	_chunks;
};

#endif

#if 0
#pragma mark --- rdsVk_TransferFrame-Decl ---
#endif // 0
#if 1

class Vk_TransferFrame : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();

	friend class TransferContext_Vk;

public:
	using Util = Vk_RenderApiUtil;

	// should be pointer, if the vector is resized, then gg
	//using Vk_UploadBufferRequests	= MutexProtected<Vector<Vk_UploadBufferRequest, 64> >;
	//using Vk_UploadImageRequests	= MutexProtected<Vector<Vk_UploadImageRequest, 64> >;

public:
	Vk_TransferFrame();
	~Vk_TransferFrame();

	Vk_TransferFrame(Vk_TransferFrame&&)	{ throwIf(true, ""); }
	void operator=(Vk_TransferFrame&&)		{ throwIf(true, ""); }

	void create	(TransferContext_Vk* tsfCtxVk);
	void destroy();

	void clear();

	void	requestStagingHandle	(StagingHandle& out, VkDeviceSize	size);
	void	uploadToStagingBuf		(StagingHandle& out, ByteSpan		data, SizeType offset);
	void*	mappedStagingBufData	(StagingHandle  hnd);

	// VK_COMMAND_BUFFER_LEVEL_PRIMARY
	Vk_CommandBuffer* requestCommandBuffer			(QueueTypeFlags type, VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestGraphicsCommandBuffer	(VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestTransferCommandBuffer	(VkCommandBufferLevel level, StrView debugName);
	Vk_CommandBuffer* requestComputeCommandBuffer	(VkCommandBufferLevel level, StrView debugName);

	Vk_Fence*		requestInFlightVkFnc	(QueueTypeFlags type);
	Vk_Semaphore*	requestCompletedVkSmp	(QueueTypeFlags type);

	RenderDevice_Vk* renderDeviceVk();

	bool hasTransferedGraphicsResoures()	const;
	bool hasTransferedComputeResoures()		const;

protected:
	Vk_Buffer_T* getVkStagingBufHnd(u32 idx);
	Vk_Buffer_T* getVkStagingBufHnd(StagingHandle hnd);

	void _setDebugName();

protected:
	TransferContext_Vk*	_tsfCtxVk = nullptr;
	Vk_Fence			_inFlightVkFnc;
	Vk_Semaphore		_completedVkSmp;
	Vk_CommandPool		_transferVkCmdPool;

	Vk_Fence			_graphicsInFlightVkFnc;
	Vk_Semaphore		_graphicsCompletedVkSmp;
	Vk_CommandPool		_graphicsVkCmdPool;

	bool _hasTransferedGraphicsResoures	= false;
	bool _hasTransferedComputeResoures	= false;

	Vk_LinearStagingBuffer _linearStagingBuf;
};

inline bool Vk_TransferFrame::hasTransferedGraphicsResoures() const { return _hasTransferedGraphicsResoures; }
inline bool Vk_TransferFrame::hasTransferedComputeResoures() const	{ return _hasTransferedComputeResoures; }

inline 
Vk_CommandBuffer* 
Vk_TransferFrame::requestCommandBuffer(QueueTypeFlags type, VkCommandBufferLevel level, StrView debugName)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return requestGraphicsCommandBuffer(level, debugName); } break;
		//case SRC::Compute:	{ return _graphicsVkCmdPool.requestCommandBuffer(level); } break;
		case SRC::Transfer: { return requestTransferCommandBuffer(level, debugName); } break;
		default: { RDS_THROW(""); }
	}
}

inline Vk_CommandBuffer* Vk_TransferFrame::requestGraphicsCommandBuffer(VkCommandBufferLevel level, StrView debugName) { return _graphicsVkCmdPool.requestCommandBuffer(level, debugName, renderDeviceVk()); }
inline Vk_CommandBuffer* Vk_TransferFrame::requestTransferCommandBuffer(VkCommandBufferLevel level, StrView debugName) { return _transferVkCmdPool.requestCommandBuffer(level, debugName, renderDeviceVk()); }

inline Vk_Fence*		
Vk_TransferFrame::requestInFlightVkFnc(QueueTypeFlags type)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return _hasTransferedGraphicsResoures ? &_graphicsInFlightVkFnc : nullptr; } break;
			//case SRC::Compute:	{ return _graphicsVkCmdPool.requestCommandBuffer(level); } break;
		//case SRC::Transfer: { return &_inFlightVkFnc; } break;
		default: { RDS_THROW(""); }
	}
}

inline Vk_Semaphore*
Vk_TransferFrame::requestCompletedVkSmp(QueueTypeFlags type)
{
	using SRC = QueueTypeFlags;
	switch (type)
	{
		case SRC::Graphics: { return _hasTransferedGraphicsResoures ? &_graphicsCompletedVkSmp : nullptr; } break;
			//case SRC::Compute:	{ return _graphicsVkCmdPool.requestCommandBuffer(level); } break;
		//case SRC::Transfer: { return _completedVkSmp; } break;
		default: { RDS_THROW(""); }
	}
}


#endif


}


#endif