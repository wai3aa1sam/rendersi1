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
	_tsfCmdBuf		= _tsfCtx ?  _tsfCtx->device()->transferFrame().requestCommandBuffer()	: nullptr;
	_uploadBufCmds	= _tsfCtx ? &_tsfCtx->device()->transferFrame()._uploadBufCmds			: nullptr;
	_uploadTexCmds	= _tsfCtx ? &_tsfCtx->device()->transferFrame()._uploadTexCmds			: nullptr;

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
}

void 
TransferRequest::commit()
{
	_tsfCtx->commit(*this);
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
	RDS_CORE_ASSERT(!cDesc._filename.is_empty() && !cDesc._uploadImage.dataPtr(), "Create Texture2D should use either filename or imageUpload, not both");
	RDS_CORE_ASSERT(_uploadTexCmds, "");

	UploadTextureJob* job = nullptr;
	{
		RDS_TODO("revisit this part, is upload on every thread needed?");
		auto lockedData = _uploadTextureJobs.scopedULock();

		RDS_TODO("allocator for UploadTextureJob");
		job = lockedData->emplace_back(makeUPtr<UploadTextureJob>(tex, rds::move(cDesc), uploadTexCmds().uploadTexture()));
	}

	auto hnd = job->dispatch(_uploadTextureJobParentHnd)->setName("up"); RDS_UNUSED(hnd);
	//_uploadTextureJobHnds.emplace_back(hnd);
}

void 
TransferRequest::waitUploadTextureCompleted() const
{
	RDS_TODO("make uploadjob in transfer frame to reduce blocking");
	JobSystem::instance()->waitForComplete(_uploadTextureJobParentHnd);
}

#endif

}