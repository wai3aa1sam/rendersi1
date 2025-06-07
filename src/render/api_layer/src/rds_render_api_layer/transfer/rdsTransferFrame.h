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
	TransferFrame();
	virtual ~TransferFrame();

	void create(CreateDesc& cDesc);
	void destroy();
	
public:
	void	reset();

	// void*	alloc(StagingHandle& oHnd, SizeType n);
	// void*	uploadToBuffer(	StagingHandle& oHnd, ByteSpan data);
	// void	uploadToDst(	u8* dst, StagingHandle hnd, SizeType n);

public:
	//virtual void	requestStagingHandle(	StagingHandle& out, SizeType	size)						= 0;
	//virtual void	uploadToStagingBuf(		StagingHandle& out, ByteSpan	data, SizeType offset = 0)	= 0;
	//virtual void*	mappedStagingBufData(	StagingHandle  hnd)											= 0;

public:
	void setRenderResourceDebugName(RenderResource* rdRsc, StrView name);

	void createRenderGpuBuffer(	RenderGpuBuffer*	buffer);
	void createTexture(			Texture*			texture);

	void destroyRenderGpuBuffer(RenderGpuBuffer*	buffer);
	void destroyTexture(		Texture*			texture);

protected:
	virtual void onCreate(		CreateDesc& cDesc);
	virtual void onPostCreate(	CreateDesc& cDesc);
	virtual void onDestroy();
	virtual void onReset();
	
public:
	void _internal_requestDestroyObject();

public:
	TransferRequest&			transferRequest();
	LinearStagingBuffer&		constBufferAllocator();
	TransferCommandSafeBuffer&	renderResourceCreateQueue();
	TransferCommandSafeBuffer&	renderResourceDestroyQueue();

private:
	LinearStagingBuffer _constBufAlloc;
	TransferRequest		_tsfReq;

	TransferCommandSafeBuffer	_rdRscCreateQueue;
	TransferCommandSafeBuffer	_rdRscDestroyQueue;
};

inline TransferRequest&		TransferFrame::transferRequest()		{ return _tsfReq; }
inline LinearStagingBuffer&	TransferFrame::constBufferAllocator()	{ return _constBufAlloc; }

inline TransferCommandSafeBuffer& TransferFrame::renderResourceCreateQueue()	{ return _rdRscCreateQueue; }
inline TransferCommandSafeBuffer& TransferFrame::renderResourceDestroyQueue()	{ return _rdRscDestroyQueue; }


#endif

}