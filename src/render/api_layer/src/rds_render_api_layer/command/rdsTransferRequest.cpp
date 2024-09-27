#include "rds_render_api_layer-pch.h"
#include "rdsTransferRequest.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferRequest-Impl ---
#endif // 0
#if 1

TransferRequest::TransferRequest()
{
	
}

TransferRequest::~TransferRequest()
{
	reset(nullptr, nullptr);
}

void 
TransferRequest::reset(TransferContext* tsfCtx, TransferFrame* tsfFrame)
{
	if (!tsfCtx || !tsfFrame)
		return;

	_tsfCtx	= tsfCtx;
	_tsfCmdBuf.clear();
}

void 
TransferRequest::commit(RenderFrameParam& rdFrameParam, bool isWaitImmediate)
{
	RDS_PROFILE_SCOPED();
	_tsfCtx->commit(rdFrameParam, *this, isWaitImmediate);
}

void 
TransferRequest::createBuffer(RenderGpuBuffer* buffer)
{
	auto& cmdBuf	= transferCommandBuffer();
	auto* cmd		= cmdBuf.newCommand<TransferCommand_CreateBuffer>();
	cmd->dst = buffer;
}

void 
TransferRequest::createTexture(Texture* texture)
{
	auto& cmdBuf	= transferCommandBuffer();
	auto* cmd		= cmdBuf.newCommand<TransferCommand_CreateTexture>();
	cmd->dst = texture;
}

void 
TransferRequest::destroyBuffer(RenderGpuBuffer* buffer)
{
	auto& cmdBuf	= transferCommandBuffer();
	auto* cmd		= cmdBuf.newCommand<TransferCommand_DestroyBuffer>();
	cmd->dst = buffer;
	RDS_CORE_ASSERT(cmd->dst->_refCount == 0, "only call when refCount is 0");
}

void 
TransferRequest::destroyTexture(Texture* texture)
{
	auto& cmdBuf	= transferCommandBuffer();
	auto* cmd		= cmdBuf.newCommand<TransferCommand_DestroyTexture>();
	cmd->dst = texture;
	RDS_CORE_ASSERT(cmd->dst->_refCount == 0, "only call when refCount is 0");
}

TransferCommand_UploadTexture*
TransferRequest::uploadTexture(Texture* tex)
{
	auto& cmdBuf	= transferCommandBuffer();
	auto* cmd		= cmdBuf.uploadTexture();
	cmd->dst = tex;
	return cmd;

}

//TransferCommand_UploadTexture*
//TransferRequest::uploadTexture(Texture* tex, StrView filename)
//{
//	auto cDesc = Texture2D::makeCDesc();
//	cDesc.create(filename);
//
//	auto& cmdBuf	= transferCommandBuffer();
//	auto* cmd		= cmdBuf.uploadTexture();
//
//	cmd->dst = tex;
//
//	return cmd;
//
//}
//
//TransferCommand_UploadTexture* 
//TransferRequest::uploadTexture(TextureCube* tex, TextureCube_CreateDesc&& cDesc)
//{
//
//}
//
//
//TransferCommand_UploadTexture*
//TransferRequest::uploadTexture(Texture2D* tex, Texture2D_CreateDesc&& cDesc)
//{
//	return nullptr;
//	//#if 0
//	//RDS_TODO("put the logic to Texture.h");
//	//RDS_TODO("all check funtion should have a dedicated function");
//
//	//RDS_CORE_ASSERT( !(
//	//	!(cDesc._filename.is_empty() && !cDesc._uploadImage.dataPtr()) 
//	//	&& (!cDesc._filename.is_empty() && cDesc._uploadImage.dataPtr())
//	//	), "Create Texture2D should use either filename or imageUpload, not both");
//	//RDS_CORE_ASSERT(_uploadTexCmds, "");
//	//#endif // 0
//
//
//	//cDesc._internal_create(_tsfCtx->renderDevice(), true);
//
//	//UploadTextureJob* job = nullptr;
//	//{
//	//	RDS_TODO("revisit this part, is upload on every thread needed?");
//	//	auto lockedData = _uploadTextureJobs.scopedULock();
//
//	//	RDS_TODO("allocator for UploadTextureJob");
//	//	job = lockedData->emplace_back(makeUPtr<UploadTextureJob>(tex, rds::move(cDesc), uploadTexCmds().uploadTexture()));
//	//}
//
//	//auto hnd = job->dispatch(_uploadTextureJobParentHnd)->setName("up"); RDS_UNUSED(hnd);
//}

void 
TransferRequest::uploadBuffer(RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset, RenderGpuMultiBuffer* rdMultiBuf)
{
	throwIf(!OsTraits::isMainThread(), "transferFrame() is not thread safe");

	RDS_TODO("put the logic to RenderGpuBuffer.h");

	TransferCommand_UploadBuffer temp;
	TransferCommand_UploadBuffer* cmd = nullptr;
	if (!BitUtil::has(rdBuf->typeFlags(), RenderGpuBufferTypeFlags::Const))
	{
		auto& cmdBuf = transferCommandBuffer();
		cmd = cmdBuf.uploadBuffer();
	}
	else
	{
		cmd = &temp;
	}

	cmd->dst	= rdBuf;
	cmd->data	= data;
	cmd->offset = offset;
	//cmd->parent = rdMultiBuf;

	rdBuf->onUploadToGpu(cmd);
}

void 
TransferRequest::uploadBuffer(RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset)
{
	uploadBuffer(rdBuf, data, offset, nullptr);
}

void 
TransferRequest::uploadBufferAsync(RenderGpuBuffer* rdBuf, Vector<u8>&& data)
{
	_notYetSupported(RDS_SRCLOC);
}

TransferCommandBuffer& TransferRequest::transferCommandBuffer()
{
	checkMainThreadExclusive(RDS_SRCLOC);
	return _tsfCmdBuf;
}

#endif

}