#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "command/rdsTransferRequest.h"
#include "rdsLinearStagingBuffer.h"

namespace rds
{
struct TransferFrame_CreateDesc : public RenderResource_CreateDesc // RenderResource_CreateDescT<TransferFrame_CreateDesc>
{
public:
	RDS_RenderResource_CreateDesc_COMMON_BODY(TransferFrame_CreateDesc);

public:

};

#if 0
#pragma mark --- rdsTransferFrame-Decl ---
#endif // 0
#if 1

class TransferFrame : public RenderResource_T<TransferFrame, RenderResourceType::TransferFrame>
{
	friend class RenderDevice;
public:
	using Base			= RenderResource;
	using CreateDesc	= TransferFrame_CreateDesc;

	using SizeType		= RenderApiLayerTraits::SizeType;

public:
	static CreateDesc				makeCDesc(RDS_DEBUG_SRCLOC_PARAM);
	static UPtr<TransferFrame>		make(CreateDesc& cDesc);

public:
	void create(CreateDesc& cDesc);
	void destroy();

public:
	//virtual void	requestStagingHandle(	StagingHandle& out, SizeType	size)						= 0;
	//virtual void	uploadToStagingBuf(		StagingHandle& out, ByteSpan	data, SizeType offset = 0)	= 0;
	//virtual void*	mappedStagingBufData(	StagingHandle  hnd)											= 0;

protected:
	virtual void onCreate(		CreateDesc& cDesc);
	virtual void onPostCreate(	CreateDesc& cDesc);
	virtual void onDestroy();
	
public:
	void _internal_requestDestroyObject();

public:
	// void*	alloc(StagingHandle& oHnd, SizeType n);
	// void*	uploadToBuffer(	StagingHandle& oHnd, ByteSpan data);
	// void	uploadToDst(	u8* dst, StagingHandle hnd, SizeType n);

	void	clear();

public:
	TransferRequest&		transferRequest();
	LinearStagingBuffer&	constBufferAllocator();

private:
	LinearStagingBuffer _constBufAlloc;
	TransferRequest		_tsfReq;
};

inline TransferRequest&		TransferFrame::transferRequest()		{ return _tsfReq; }
inline LinearStagingBuffer&	TransferFrame::constBufferAllocator()	{ return _constBufAlloc; }


#endif

}