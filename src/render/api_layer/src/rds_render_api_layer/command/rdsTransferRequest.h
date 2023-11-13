#pragma once

#include "rds_render_api_layer/common/rds_render_api_layer_common.h"
#include "rdsTransferCommand.h"

namespace rds
{

#if 0
#pragma mark --- rdsTransferRequest-Decl ---
#endif // 0
#if 1

class TransferContext;

class TransferRequest_UploadTextureJob;

class TransferRequest : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
public:
	using UploadTextureJob = TransferRequest_UploadTextureJob;
	using UploadTextureJobs = MutexProtected<Vector<UPtr<UploadTextureJob>, 32 > >;

public:
	TransferRequest();
	~TransferRequest();

	void reset(TransferContext* tsfCtx);
	void commit();

	void uploadTexture(Texture2D* tex, StrView filename);
	void uploadTexture(Texture2D* tex, Texture2D_CreateDesc&& cDesc);
	void uploadBuffer();

	TransferCommandBuffer& transferCommandBuffer();

	bool isUploadTextureCompleted() const;
	void waitUploadTextureCompleted() const;

private:
	TransferContext* _tsfCtx = nullptr;

	//Vector<TransferCommandBuffer, s_kThreadCount> _transferCommandBuffers;

	TransferCommandBuffer* _tsfCmdBuf = nullptr;

	UploadTextureJobs		_uploadTextureJobs;
	JobHandle				_uploadTextureJobParentHnd = {};
	//Vector<JobHandle, 64>	_uploadTextureJobHnds;		// use this or _uploadTextureJobParentHnd to dectect when all the jobs are finished
};

inline
TransferCommandBuffer& TransferRequest::transferCommandBuffer()
{
	return *_tsfCmdBuf;
	//auto tlid = OsTraits::threadLocalId();
	//return _transferCommandBuffers[tlid];
}

inline bool TransferRequest::isUploadTextureCompleted() const { return _uploadTextureJobParentHnd->isCompleted(); }


#endif

}