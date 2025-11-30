#include "rds_render_api_layer-pch.h"
#include "rdsTransferFrame.h"
#include "rds_render_api_layer/rdsRenderer.h"
#include "rds_render_api_layer/rdsRenderDevice.h"
#include "rds_render_api_layer/transfer/command/rdsTransferCommand_Impl.h"

#define RDS_RENDER_CMD_CREATE_RENDER_RSC(T, v)								\
auto lock = _createRdRscBuf.scopedULock();									\
auto* cmd = lock->newCommand<RDS_CONCAT(TransferCommand_Create, T)>();		\
cmd->dst = v;																\
// ---

#define RDS_RENDER_CMD_DESTROY_RENDER_RSC(T, v)								\
auto lock = _destroyRdRscBuf.scopedULock();									\
auto* cmd = lock->newCommand<RDS_CONCAT(TransferCommand_Destroy, T)>();		\
cmd->dst = v;																\
// ---

namespace rds
{

#if 0
#pragma mark --- rdsTransferFrame-Impl ---
#endif // 0
#if 1

TransferFrame::CreateDesc 
TransferFrame::makeCDesc()
{
	return CreateDesc{};
}

SPtr<TransferFrame> 
TransferFrame::make(RDS_DebugLabel_PARAM, CreateDesc& cDesc)
{
	return Renderer::renderDevice()->createTransferFrame(RDS_DebugLabel_ARG, cDesc); 
}

TransferFrame::TransferFrame()
{
}

TransferFrame::~TransferFrame()
{

}

void TransferFrame::create(CreateDesc& cDesc)
{
	Base::create(cDesc);
	onCreate(cDesc);
	onPostCreate(cDesc);
}

void TransferFrame::onDestroy()
{
	TransferFrame::onReset(0);
}

void 
TransferFrame::reset(i64 frameCount)
{
	checkMainThreadExclusive(RDS_SRCLOC);
	onReset(frameCount);
}

void TransferFrame::onCreate(CreateDesc& cDesc)
{

}

void TransferFrame::onPostCreate(CreateDesc& cDesc)
{

}

void 
TransferFrame::onReset(i64 frameCount)
{
	_constBufAlloc.reset();
	auto fn_rdRscBuf = [](auto& buf) { auto data = buf.scopedULock(); data->clear(); };
	fn_rdRscBuf(_destroyRdRscBuf);		// must reset before create, since create will release SPtr
	fn_rdRscBuf(_createRdRscBuf);
	fn_rdRscBuf(_setDebugName_rdRscBuf);
	_tsfReq.reset(&transferContext());
}

#if 1

void 
TransferFrame::setRenderResourceDebugName(RenderResource* rdRsc, StrView name)
{
	auto lock = _setDebugName_rdRscBuf.scopedULock();
	auto* cmd = lock->newCommand<TransferCommand_SetDebugLabel>();

	cmd->dst	= rdRsc;
	//cmd->name	= name;
}

void 
TransferFrame::createRenderContext(RenderContext* rdCtx)
{
	RDS_RENDER_CMD_CREATE_RENDER_RSC(RenderContext, rdCtx);
}

void
TransferFrame::createRenderGpuBuffer(RenderGpuBuffer* buffer, const RenderGpuBuffer_CreateDesc& cDesc)
{
	auto lock = _createRdRscBuf.scopedULock();
	auto* cmd = lock->newCommand<TransferCommand_CreateRenderGpuBuffer>();

	cmd->dst	= buffer;
	cmd->cDesc	= cDesc;
}

void 
TransferFrame::createTexture(Texture* texture)
{
	RDS_TODO("rework command data member for debug SRCLOC, transfer and render also need to rework!!!");

	auto lock = _createRdRscBuf.scopedULock();
	auto* cmd = lock->newCommand<TransferCommand_CreateTexture>();

	cmd->dst = texture;
}

void 
TransferFrame::destroyRenderContext(RenderContext* rdCtx)
{
	RDS_RENDER_CMD_DESTROY_RENDER_RSC(RenderContext, rdCtx);
}

void
TransferFrame::destroyRenderGpuBuffer(RenderGpuBuffer* buffer)
{
	auto lock = _destroyRdRscBuf.scopedULock();
	auto* cmd = lock->newCommand<TransferCommand_DestroyRenderGpuBuffer>();

	//OsUtil::sleep_ms(1);
	cmd->dst = buffer;
}

void 
TransferFrame::destroyTexture(Texture* texture)
{
	auto lock = _destroyRdRscBuf.scopedULock();
	auto* cmd = lock->newCommand<TransferCommand_DestroyTexture>();

	//OsUtil::sleep_ms(1);
	cmd->dst = texture;
}


#endif // 1



#if 0

void 
TransferFrame::clear()
{
	auto data = _alloc.scopedULock();
	data->clear();
}

void* 
TransferFrame::alloc(StagingHandle& oHnd, SizeType n)
{
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
TransferFrame::uploadToBuffer(StagingHandle& oHnd, ByteSpan data)
{
	void* buf = alloc(oHnd, data.size());
	memory_copy(sCast<u8*>(buf), data.data(), data.size());
	return buf;
}

void
TransferFrame::uploadToDst(u8* dst, StagingHandle hnd, SizeType n)
{
	auto data = _alloc.scopedULock();
	memory_copy(dst, data->chunks()[hnd.chunkId]->data() + hnd.offset, n);
}

#endif // 0


#endif


}