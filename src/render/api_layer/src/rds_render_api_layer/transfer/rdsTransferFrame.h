#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"

#include "command/rdsTransferRequest.h"
#include "rdsLinearStagingBuffer.h"
#include "rds_render_api_layer/rdsRenderDevice.h"

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
	static CreateDesc				makeCDesc();
	static SPtr<TransferFrame>		make(RDS_DebugLabel_PARAM, CreateDesc& cDesc);

public:
	TransferFrame();
	virtual ~TransferFrame();

	void create(CreateDesc& cDesc);

protected:
	virtual void onDestroy();

public:
	void	reset(i64 frameCount);

	// void*	alloc(StagingHandle& oHnd, SizeType n);
	// void*	uploadToBuffer(	StagingHandle& oHnd, ByteSpan data);
	// void	uploadToDst(	u8* dst, StagingHandle hnd, SizeType n);

public:
	//virtual void	requestStagingHandle(	StagingHandle& out, SizeType	size)						= 0;
	//virtual void	uploadToStagingBuf(		StagingHandle& out, ByteSpan	data, SizeType offset = 0)	= 0;
	//virtual void*	mappedStagingBufData(	StagingHandle  hnd)											= 0;

public:
	void setRenderResourceDebugLabel(RenderResource* rdRsc, RDS_DebugLabel_PARAM);

	void createRenderContext(	RenderContext*		rdCtx);
	void createRenderGpuBuffer(	RenderGpuBuffer*	buffer, const RenderGpuBuffer_CreateDesc& cDesc);
	void createTexture(			Texture*			texture);

	void destroyRenderContext(	RenderContext*		rdCtx);
	void destroyRenderGpuBuffer(RenderGpuBuffer*	buffer);
	void destroyTexture(		Texture*			texture);

protected:
	virtual void onCreate(		CreateDesc& cDesc);
	virtual void onPostCreate(	CreateDesc& cDesc);
	virtual void onReset(i64 frameCount);

public:
	TransferRequest&			transferRequest();
	LinearStagingBuffer&		constBufferAllocator();
	TransferCommandSafeBuffer&	renderResourceBuffer_create();
	TransferCommandSafeBuffer&	renderResourceBuffer_destroy();
	TransferCommandSafeBuffer&	renderResourceBuffer_setDebugLabel();

private:
	LinearStagingBuffer _constBufAlloc;
	TransferRequest		_tsfReq;

	TransferCommandSafeBuffer	_rdRscBuf_create;
	TransferCommandSafeBuffer	_rdRscBuf_destroy;
	TransferCommandSafeBuffer	_rdRscBuf_setDebugLabel;		// ensure we setDebugName after create
};

inline TransferRequest&				TransferFrame::transferRequest()					{ return _tsfReq; }
inline LinearStagingBuffer&			TransferFrame::constBufferAllocator()				{ return _constBufAlloc; }
inline TransferCommandSafeBuffer&	TransferFrame::renderResourceBuffer_create()		{ return _rdRscBuf_create; }
inline TransferCommandSafeBuffer&	TransferFrame::renderResourceBuffer_destroy()		{ return _rdRscBuf_destroy; }
inline TransferCommandSafeBuffer&	TransferFrame::renderResourceBuffer_setDebugLabel()	{ return _rdRscBuf_setDebugLabel; }

#endif

}