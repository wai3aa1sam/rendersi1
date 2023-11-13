#include "rds_render_api_layer-pch.h"
#include "rdsTransferRequest.h"
#include "rds_render_api_layer/transfer/rdsTransferContext.h"

#include "rds_render_api_layer/rdsRenderer.h"

namespace rds
{

class TransferRequest_UploadTextureJob : public Job_Base
{
public:
	TransferRequest_UploadTextureJob(Texture2D* tex, Texture2D_CreateDesc&& cDesc, TransferRequest* tsfReq)
	{
		_tex	= tex;
		_cDesc	= rds::move(cDesc);
		_tsfReq = tsfReq;
	}

	virtual void execute() override
	{
		auto* cmd = _tsfReq->transferCommandBuffer().uploadTexture();
		_tex->_internal_uploadToGpu(_cDesc, cmd);
	}

public:
	TransferRequest*		_tsfReq = nullptr;
	SPtr<Texture2D>			_tex;
	Texture2D_CreateDesc	_cDesc;
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

	waitUploadTextureCompleted();

	_tsfCtx		= tsfCtx;
	_tsfCmdBuf	= _tsfCtx ? _tsfCtx->device()->transferFrame().requestCommandBuffer() : nullptr;
	if (_tsfCmdBuf)
	{
		_tsfCmdBuf->clear();
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

	UploadTextureJob* job = nullptr;
	{
		RDS_TODO("revisit this part, is upload on every thread needed?");
		auto lockedData = _uploadTextureJobs.scopedULock();

		RDS_TODO("allocator for UploadTextureJob");
		job = lockedData->emplace_back(makeUPtr<UploadTextureJob>(tex, rds::move(cDesc), this));
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