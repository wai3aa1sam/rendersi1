#include "rds_render_api_layer-pch.h"
#include "rdsTransferRequest.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

class TransferRequest_UploadTextureJob : public Job_Base
{
public:
	TransferRequest_UploadTextureJob(Texture2D* tex, Texture2D_CreateDesc&& cDesc, TransferCommand_UploadTexture* cmd)
	{
		_tex	= tex;
		_cDesc	= rds::move(cDesc);
		_cmd = cmd;
	}

	virtual void execute() override
	{
		_tex->_internal_uploadToGpu(_cDesc, _cmd);
	}

public:
	TransferCommand_UploadTexture*	_cmd = nullptr;
	SPtr<Texture2D>					_tex;
	Texture2D_CreateDesc			_cDesc;
};

class TransferRequest_UploadBufferJob : public Job_Base
{
public:
	TransferRequest_UploadBufferJob(RenderGpuBuffer* rdBuf, TransferCommand_UploadBuffer* cmd)
	{
		_rdBuf	= rdBuf;
		//_cDesc	= rds::move(cDesc);
		_cmd = cmd;
	}

	virtual void execute() override
	{
		_rdBuf->onUploadToGpu(_cmd);
		//_tex->_internal_uploadToGpu(_cDesc, _cmd);
	}

public:
	TransferCommand_UploadBuffer*	_cmd = nullptr;
	SPtr<RenderGpuBuffer>			_rdBuf;
	//Texture2D_CreateDesc			_cDesc;
};

#if 0
#pragma mark --- rdsTransferRequest-Impl ---
#endif // 0
#if 1

TransferRequest::TransferRequest()
{
	
}

TransferRequest::~TransferRequest()
{
	reset(nullptr);
}

void 
TransferRequest::reset(TransferContext* tsfCtx)
{
	/*if (_uploadTextureJobParentHnd && !isUploadTextureCompleted())
	{
		RDS_THROW("upload jobs must be completed before reset");
	}*/

	RDS_TODO("per frame, then no need to wait too frequently");
	waitUploadTextureCompleted();

	_tsfCtx		= tsfCtx;
	auto* rdDev = _tsfCtx ?  _tsfCtx->renderDevice() : nullptr;

	_tsfCmdBuf		= _tsfCtx ?  rdDev->transferFrame().requestCommandBuffer()	: nullptr;
	_uploadBufCmds	= _tsfCtx ? &rdDev->transferFrame()._uploadBufCmds			: nullptr;
	_uploadTexCmds	= _tsfCtx ? &rdDev->transferFrame()._uploadTexCmds			: nullptr;

	if (_tsfCtx)
	{
		_tsfCmdBuf->clear();
		_uploadBufCmds->clear();
		_uploadTexCmds->clear();
	}

	_uploadTextureJobParentHnd = JobSystem::instance()->createParentJob()->setName("upParent");

	{
		auto lockedData = _uploadTextureJobs.scopedULock();
		lockedData->clear();
	}

	{
		auto lockedData = _uploadBufferJobs.scopedULock();
		lockedData->clear();
	}
}

void 
TransferRequest::commit()
{
	_tsfCtx->commit(*this);
	RDS_WARN_ONCE("TransferRequest commit success");
}

void 
TransferRequest::uploadTexture(Texture2D* tex, StrView filename)
{
	auto cDesc = Texture2D::makeCDesc();
	cDesc.create(filename);
	uploadTexture(tex, rds::move(cDesc));
}

void 
TransferRequest::uploadTexture(Texture2D* tex, Texture2D_CreateDesc&& cDesc)
{
	throwIf(!OsTraits::isMainThread(), "transferFrame() is not thread safe");

	RDS_TODO("put the logic to Texture.h");
	RDS_TODO("all check funtion should have a dedicated function");

	RDS_CORE_ASSERT( !(
					 !(cDesc._filename.is_empty() && !cDesc._uploadImage.dataPtr()) 
					 && (!cDesc._filename.is_empty() && cDesc._uploadImage.dataPtr())
					 ), "Create Texture2D should use either filename or imageUpload, not both");
	RDS_CORE_ASSERT(_uploadTexCmds, "");

	cDesc._internal_create(_tsfCtx->renderDevice(), true);

	UploadTextureJob* job = nullptr;
	{
		RDS_TODO("revisit this part, is upload on every thread needed?");
		auto lockedData = _uploadTextureJobs.scopedULock();

		RDS_TODO("allocator for UploadTextureJob");
		job = lockedData->emplace_back(makeUPtr<UploadTextureJob>(tex, rds::move(cDesc), uploadTexCmds().uploadTexture()));
	}

	auto hnd = job->dispatch(_uploadTextureJobParentHnd)->setName("up"); RDS_UNUSED(hnd);
}

void 
TransferRequest::uploadBuffer(RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset, RenderGpuMultiBuffer* rdMultiBuf)
{
	throwIf(!OsTraits::isMainThread(), "transferFrame() is not thread safe");

	RDS_TODO("put the logic to RenderGpuBuffer.h");

	TransferCommand_UploadBuffer temp;
	TransferCommand_UploadBuffer* cmd = nullptr;
	if (!BitUtil::has(rdBuf->typeFlags(), RenderGpuBufferTypeFlags::Const))
	{
		cmd = uploadBufCmds().uploadBuffer();
	}
	else
	{
		cmd = &temp;
	}

	cmd->dst	= rdBuf;
	cmd->data	= data;
	cmd->offset = offset;
	cmd->parent = rdMultiBuf;

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
	throwIf(!OsTraits::isMainThread(), "transferFrame() is not thread safe");

}

void 
TransferRequest::waitUploadTextureCompleted() const
{
	RDS_TODO("make uploadjob in transfer frame to reduce blocking");
	JobSystem::instance()->waitForComplete(_uploadTextureJobParentHnd);
}

#endif

}