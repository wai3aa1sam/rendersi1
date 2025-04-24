#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

namespace rds
{

struct StagingHandle
{
	using SizeType = RenderApiLayerTraits::SizeType;
public:
	static constexpr SizeType s_kInvalid = NumLimit<u32>::max();

	bool isValid()			const { return chunkId != s_kInvalid && offset != s_kInvalid; }
	void checkValid()		const { RDS_CORE_ASSERT(isValid(), "Invalid StagingHandle"); }

	bool isOffsetValid()	const { return offset != s_kInvalid; }
	void checkOffsetValid()	const { RDS_CORE_ASSERT(isOffsetValid(), "Invalid StagingHandle"); }

public:
	void create(SizeType chunkId_, SizeType offset_)
	{
		chunkId = sCast<u32>(chunkId_);
		offset	= sCast<u32>(offset_);
	}

public:
	u32 chunkId = s_kInvalid;
	u32 offset	= s_kInvalid;
};

#if 0
#pragma mark --- rdsLinearStagingBuffer-Decl ---
#endif // 0
#if 1

class LinearStagingBuffer : public NonCopyable
{
	using SizeType = RenderApiLayerTraits::SizeType;
public:
	void*	alloc(StagingHandle& oHnd, SizeType n);
	void*	uploadToBuffer(	StagingHandle& oHnd, ByteSpan data);
	void	uploadToDst(	u8* dst, StagingHandle hnd, SizeType n);

	void	clear();
	void	reset();

private:
	MutexProtected<LinearAllocator> _alloc;
};

#endif

}