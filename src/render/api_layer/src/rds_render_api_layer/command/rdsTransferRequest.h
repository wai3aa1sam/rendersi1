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
class TransferRequest_UploadBufferJob;

class Texture2D;
class RenderGpuBuffer;

class TransferRequest : public NonCopyable
{
	RDS_RENDER_API_LAYER_COMMON_BODY();
	friend class Texture2D;
	friend class RenderGpuBuffer;
public:
	using UploadTextureJob	= TransferRequest_UploadTextureJob;
	using UploadTextureJobs = MutexProtected<Vector<UPtr<UploadTextureJob>, 32 > >;

	using UploadBufferJob	= TransferRequest_UploadBufferJob;
	using UploadBufferJobs	= MutexProtected<Vector<UPtr<UploadBufferJob>, 32 > >;

public:
	TransferRequest();
	~TransferRequest();

	void reset(TransferContext* tsfCtx, u64 frameCount);
	void commit(bool isWaitImmediate = false);

	void uploadTexture(Texture2D* tex, StrView filename);
	void uploadTexture(Texture2D* tex, Texture2D_CreateDesc&& cDesc);

	void uploadBuffer		(RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset, RenderGpuMultiBuffer* rdMultiBuf);
	void uploadBuffer		(RenderGpuBuffer* rdBuf, ByteSpan data, SizeType offset = 0);
	void uploadBufferAsync	(RenderGpuBuffer* rdBuf, Vector<u8>&& data); // template<class DATA, size_t N>, DATA -> u8

	TransferCommandBuffer& transferCommandBuffer();
	TransferCommandBuffer& uploadBufCmds();
	TransferCommandBuffer& uploadTexCmds();

	bool isUploadTextureCompleted() const;
	void waitUploadTextureCompleted() const;

private:
	TransferContext* _tsfCtx = nullptr;

	//Vector<TransferCommandBuffer, s_kThreadCount> _transferCommandBuffers;

	TransferCommandBuffer* _tsfCmdBuf		= nullptr;
	TransferCommandBuffer* _uploadBufCmds	= nullptr;
	TransferCommandBuffer* _uploadTexCmds	= nullptr;

	/*
	TODO: UploadJob put to per frame, but the intended purpose is to not block
	, using per frame would still block currently
	, should have a completed and a pending UploadJob Vectors for non-blocking
	, only wait when it must submit in this frame and the frame cycle is maxInFlightFrame - 1 (iFrame % maxInFlightFrame job will be cleared)

	also, the Jobs could replace the cmd, the cmd seems redundant, only loop the jobs also beat the game
	*/
	UploadTextureJobs		_uploadTextureJobs;
	JobHandle				_uploadTextureJobParentHnd = {};

	UploadBufferJobs		_uploadBufferJobs;
	JobHandle				_uploadBufferJobParentHnd = {};
};

inline
TransferCommandBuffer& TransferRequest::transferCommandBuffer()
{
	return *_tsfCmdBuf;
	//auto tlid = OsTraits::threadLocalId();
	//return _transferCommandBuffers[tlid];
}
inline TransferCommandBuffer& TransferRequest::uploadBufCmds() { return *_uploadBufCmds; }
inline TransferCommandBuffer& TransferRequest::uploadTexCmds() { return *_uploadTexCmds; }

inline bool TransferRequest::isUploadTextureCompleted() const { return _uploadTextureJobParentHnd->isCompleted(); }


#endif


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
		_notYetSupported(RDS_SRCLOC);
		//throwIf(true, "not ")
		//_tex->uploadToGpu(_cDesc, _cmd);
	}

public:
	TransferCommand_UploadTexture*	_cmd = nullptr;
	SPtr<Texture2D>					_tex;
	Texture2D_CreateDesc			_cDesc;
};

}